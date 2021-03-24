/**
 * @file
 * Address Resolution Protocol module for IP over Ethernet
 *
 * Functionally, ARP is divided into two parts. The first maps an IP address
 * to a physical address when sending a packet, and the second part answers
 * requests from other machines for our physical address.
 *
 * This implementation complies with RFC 826 (Ethernet ARP). It supports
 * Gratuitious ARP from RFC3220 (IP Mobility Support for IPv4) section 4.6
 * if an interface calls etharp_gratuitous(our_netif) upon address change.
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * Copyright (c) 2003-2004 Leon Woestenberg <leon.woestenberg@axon.tv>
 * Copyright (c) 2003-2004 Axon Digital Design B.V., The Netherlands.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */

#include "lwip/opt.h"

#include <string.h>
#include <math.h>

#include <drv_delay.h>

#include "lwip/def.h"
#include "lwip/mem.h"
#include "netif/etharp.h"
#include "netif/ethpac.h"

#define MAX_RX_DATA_LEN	32

/* Private typedef -----------------------------------------------------------*/

struct shared_ethpac_pkt_info {
	u8_t			done;
	u8_t			rx_data[MAX_RX_DATA_LEN];
	u8_t			rx_data_len;
	u16_t			sequence;
	u16_t			pkt_len;
	u8_t			func_code;
	u8_t			sub_func_code;
	u8_t			request_type;
};

struct ethpac_pkt_len_entry {
	u16_t	pkt_len;
	u8_t	func_code;
	u8_t	sub_func_code;
};

/* Private define ------------------------------------------------------------*/
#define SEQUENCE_LEN							2
#define PKT_LEN_LEN								2
#define FUNC_CODE_LEN							1
#define SUB_FUNC_CODE_LEN						1

// FPGA version
#define FUNC_CODE_GET_FPGA_VER					0x00U
#define SUB_FUNC_CODE_GET_FPGA_VER				0x80U

// phase coding group buffer
#define FUNC_CODE_SET_PC_GROUP_BUF				0x01U
#define SUB_FUNC_CODE_SET_PC_GROUP_BUF			0x01U

#define FUNC_CODE_SET_RESPONSE					0x80U
#define SUB_FUNC_CODE_SET_RESPONSE				0x00U

// request type
#define ETHPAC_REQUEST_TYPE_SET					0x00U
#define ETHPAC_REQUEST_TYPE_GET					0x01U

#define MAX_GROUP_DATA_LEN_PER_PKT				1400

// ETHPAC packet length table size
#define ETHPAC_REQUEST_PKT_LEN_TABLE_SIZE		2
#define ETHPAC_RESPONSE_PKT_LEN_TABLE_SIZE		2

// packet length table
static const struct ethpac_pkt_len_entry ethpac_request_pkt_len_table[ETHPAC_REQUEST_PKT_LEN_TABLE_SIZE] = {
		{0x02, FUNC_CODE_GET_FPGA_VER, SUB_FUNC_CODE_GET_FPGA_VER},
		{0x00, FUNC_CODE_SET_PC_GROUP_BUF, SUB_FUNC_CODE_SET_PC_GROUP_BUF}	// packet length 0x00 means None
};

static const struct ethpac_pkt_len_entry ethpac_response_pkt_len_table[ETHPAC_RESPONSE_PKT_LEN_TABLE_SIZE] = {
		{0x0B, FUNC_CODE_GET_FPGA_VER, SUB_FUNC_CODE_GET_FPGA_VER},
		{0x06, FUNC_CODE_SET_RESPONSE, SUB_FUNC_CODE_SET_RESPONSE}
};

static u16_t sequence = 0x0000U;

static struct shared_ethpac_pkt_info shared_ethpac_pkt_info;

//static const struct eth_addr ethfpga = {{0x84, 0xa9, 0x3e, 0x17, 0x77, 0x16}};

static u8_t ethpac_data_buf[MAX_GROUP_DATA_LEN_PER_PKT + 7];

//内存池(4字节对齐)
static __align(4) u8 group_0_data[0xFFFF] __attribute__((at(0x78000000)));		//外部SRAM内存池
static u16_t group_0_size;
static u16_t rx_group_data_len;

/* Private function prototypes -----------------------------------------------*/
static s8_t get_ethpac_request_pkt_len(u8_t func_code, u8_t sub_func_code, u16_t *pkt_len);

static err_t build_packet(const struct eth_addr *dst, const struct eth_addr *src,
		u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u8_t request_type, u16_t pkt_len,
		const void **pkt, u16_t *actual_pkt_len);

static err_t ethpac_raw(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u8_t request_type, u16_t pkt_len);

static err_t ethpac_set_no_ack(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u16_t pkt_len);

static err_t ethpac_set(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u16_t pkt_len);

static err_t ethpac_get(u8_t func_code, u8_t sub_func_code,
		u8_t *data, u16_t data_len, u16_t pkt_len);

/* Private functions ---------------------------------------------------------*/
static err_t get_ethpac_request_pkt_len(u8_t func_code, u8_t sub_func_code, u16_t *pkt_len)
{
	u8_t	i;

	LWIP_ASSERT("pkt_len != NULL", pkt_len != NULL);

	for (i = 0; i < ETHPAC_REQUEST_PKT_LEN_TABLE_SIZE; i++)
	{
		if ((ethpac_request_pkt_len_table[i].func_code == func_code)
				&& (ethpac_request_pkt_len_table[i].sub_func_code == sub_func_code))
		{
			*pkt_len = ethpac_request_pkt_len_table[i].pkt_len;
			return ERR_OK;
		}
	}

	return ERR_ARG;
}

/* PlanarArrayController request = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 * 		sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B) data
 * */
static err_t build_packet(const struct eth_addr *dst, const struct eth_addr *src,
		u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u8_t request_type, u16_t pkt_len,
		const void **pkt, u16_t *actual_pkt_len)
{
	u16_t total_pkt_len;
	err_t result = ERR_OK;
	struct eth_hdr *ethhdr;
	struct ethpac_hdr *hdr;
	u8_t pad_size;
	void *ptr;

	if (!pkt_len)
	{
		result = get_ethpac_request_pkt_len(func_code, sub_func_code, &pkt_len);

		if (result != ERR_OK)
			return result;
	}

	total_pkt_len = pkt_len + 6 + 6 + 2 + 2 + 2;
	if (total_pkt_len < MIN_ETHPAC_PKT_LEN)
		pad_size = total_pkt_len - (pkt_len + 6 + 6 + 2 + 2 + 2);
	else
		pad_size = 0;

	/* allocate a pbuf for the outgoing pac request packet */
	ptr = mem_malloc(total_pkt_len + pad_size);
	/* could allocate a pbuf for an pac request? */
	if (ptr == NULL) {
		return ERR_MEM;
	}

	ethhdr = (struct eth_hdr *)ptr;
	hdr = (struct ethpac_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR);

	ethhdr->dest = *dst;
	ethhdr->src = *src;
	ethhdr->type = ((ETHTYPE_FPGA & 0xff) << 8) | ((ETHTYPE_FPGA & 0xff00) >> 8);

	hdr->sequence = ((sequence & 0xff) << 8) | ((sequence & 0xff00) >> 8);
	hdr->pkt_len = ((pkt_len & 0xff) << 8) | ((pkt_len & 0xff00) >> 8);
	hdr->func_code = func_code;
	hdr->sub_func_code = sub_func_code;

	if (data)
		memcpy((void *)(hdr + SIZEOF_ETHPAC_HDR), (const void *)data, data_len);

	if (pad_size > 0)
		memset((void *)(hdr + SIZEOF_ETHPAC_HDR + data_len), 0, pad_size);

	shared_ethpac_pkt_info.done = 0;
	shared_ethpac_pkt_info.sequence = sequence++;
	shared_ethpac_pkt_info.pkt_len = pkt_len;
	shared_ethpac_pkt_info.func_code = func_code;
	shared_ethpac_pkt_info.sub_func_code = sub_func_code;
	shared_ethpac_pkt_info.request_type = request_type;

	*pkt = (const void *)ptr;
	*actual_pkt_len = total_pkt_len + pad_size;

	return ERR_OK;
}

/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
extern struct netif lwip_netif;

static err_t ethpac_raw(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u8_t request_type, u16_t pkt_len)
{
	struct netif *netif;
	const struct eth_addr *dst;
	const struct eth_addr *src;
	const void *pkt;
	u16_t actual_pkt_len;
	err_t result = ERR_OK;

	netif = &lwip_netif;
	dst = (const struct eth_addr *)netif->fpga_hwaddr;;
	src = (const struct eth_addr *)netif->hwaddr;

	result = build_packet(dst, src, func_code, sub_func_code,
			data, data_len, request_type, pkt_len, &pkt, &actual_pkt_len);
	if (result != ERR_OK)
		return result;

	/* send the packet */
	netif->linkoutput(netif, pkt, actual_pkt_len);

	mem_free((void *)pkt);

	return ERR_OK;
}

static err_t ethpac_set_no_ack(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u16_t pkt_len)
{
	return ethpac_raw(func_code, sub_func_code, data, data_len, ETHPAC_REQUEST_TYPE_SET, pkt_len);
}


static err_t ethpac_set(u8_t func_code, u8_t sub_func_code,
		const u8_t *data, u16_t data_len, u16_t pkt_len)
{
	err_t result = ERR_OK;
	u8_t i;

	result = ethpac_raw(func_code, sub_func_code, data, data_len, ETHPAC_REQUEST_TYPE_SET, pkt_len);
	if (result != ERR_OK)
		return result;

	/* check the done flag */
	for (i = 0; i < 20; i++)
	{
		if (shared_ethpac_pkt_info.done)
			return ERR_OK;

		delay_ms(100);
	}

	return ERR_TIMEOUT;
}

static err_t ethpac_get(u8_t func_code, u8_t sub_func_code,
		u8_t *data, u16_t data_len, u16_t pkt_len)
{
	err_t result = ERR_OK;
	u8_t i;

	result = ethpac_raw(func_code, sub_func_code, data, data_len, ETHPAC_REQUEST_TYPE_GET, pkt_len);
	if (result != ERR_OK)
		return result;

	/* check the done flag */
	for (i = 0; i < 20; i++)
	{
		if (shared_ethpac_pkt_info.done)
			return ERR_OK;

		delay_ms(100);
	}

	return ERR_TIMEOUT;
}

err_t ethpac_get_fpga_ver(u8_t fpga_ver[9])
{
	err_t result = ERR_OK;

	result = ethpac_get(FUNC_CODE_GET_FPGA_VER, SUB_FUNC_CODE_GET_FPGA_VER,
			NULL, 0, 2);
	if (result != ERR_OK)
		return result;

	if (shared_ethpac_pkt_info.rx_data_len != 9)
		return ERR_VAL;

	memcpy((void *)fpga_ver, (const void *)shared_ethpac_pkt_info.rx_data, 9);

	return ERR_OK;
}

err_t ethpac_set_group_buf(u8_t group_id, u16_t group_size,
		const u8_t *group_data, u32_t group_data_len)
{
	u32_t pkt_num;
	u32_t pkt_id;
	u16_t tmp;
	u16_t ethpac_data_len;
	err_t result = ERR_OK;

	pkt_num = (u32_t)ceil((double)group_data_len / MAX_GROUP_DATA_LEN_PER_PKT);

	for (pkt_id = 0; pkt_id < pkt_num; pkt_id++)
	{
		ethpac_data_buf[0] = group_id;
		ethpac_data_buf[1] = (group_size & 0xff00) >> 8;
		ethpac_data_buf[2] = group_size & 0xff;

		if (pkt_num == 1)
		{
			ethpac_data_buf[3] = 0x00;
			ethpac_data_buf[4] = 0x00;
			ethpac_data_buf[5] = (group_data_len & 0xff00) >> 8;
			ethpac_data_buf[6] = group_data_len & 0xff;
			memcpy((void *)&ethpac_data_buf[7], (const void *)group_data, group_data_len);

			ethpac_data_len = 7 + group_data_len;
		}
		else if (pkt_id < (pkt_num - 1))
		{
			ethpac_data_buf[3] = ((pkt_id + 1) & 0xff00) >> 8;;
			ethpac_data_buf[4] = (pkt_id + 1) & 0xff;
			ethpac_data_buf[5] = (MAX_GROUP_DATA_LEN_PER_PKT & 0xff00) >> 8;
			ethpac_data_buf[6] = MAX_GROUP_DATA_LEN_PER_PKT & 0xff;
			memcpy((void *)&ethpac_data_buf[7],
					(const void *)&group_data[MAX_GROUP_DATA_LEN_PER_PKT * pkt_id], MAX_GROUP_DATA_LEN_PER_PKT);

			ethpac_data_len = 7 + MAX_GROUP_DATA_LEN_PER_PKT;
		}
		else if (pkt_id == (pkt_num - 1))
		{
			tmp = group_data_len - MAX_GROUP_DATA_LEN_PER_PKT * pkt_id;

			ethpac_data_buf[3] = ((pkt_id + 1) & 0xff00) >> 8;;
			ethpac_data_buf[4] = (pkt_id + 1) & 0xff;
			ethpac_data_buf[5] = (tmp & 0xff00) >> 8;
			ethpac_data_buf[6] = tmp & 0xff;
			memcpy((void *)&ethpac_data_buf[7],
					(const void *)&group_data[MAX_GROUP_DATA_LEN_PER_PKT * pkt_id], tmp);

			ethpac_data_len = 7 + tmp;
		}

		if (pkt_id < (pkt_num - 1))
		{
			result = ethpac_set_no_ack(FUNC_CODE_SET_PC_GROUP_BUF, SUB_FUNC_CODE_SET_PC_GROUP_BUF,
					ethpac_data_buf, ethpac_data_len, 2 + ethpac_data_len);
			if (result != ERR_OK)
				return result;
//			delay_ms(50);

		}
		else
		{
			result = ethpac_set(FUNC_CODE_SET_PC_GROUP_BUF, SUB_FUNC_CODE_SET_PC_GROUP_BUF,
					ethpac_data_buf, ethpac_data_len, 2 + ethpac_data_len);
			if (result != ERR_OK)
				return result;
		}
	}

	return ERR_OK;
}

/**
 * Responds to ARP requests to us. Upon ARP replies to us, add entry to cache
 * send out queued IP packets. Updates cache with snooped address pairs.
 *
 * Should be called for incoming ARP packets. The pbuf in the argument
 * is freed by this function.
 *    PlanarArrayController request = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *        sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B) data
 *
 *    PlanarArrayController response for set = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *        sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B)
 *        data (sequence_request(2B) func_code_request(1B) sub_func_code_request(1B))
 *
 *        note: pkt_len = 0x06 func_code = 0x80 sub_func_code = 0x00
 *
 *    PlanarArrayController response for get = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *       sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B) data
 * *
 * @param netif The lwIP network interface on which the ARP packet pbuf arrived.
 * @param p The ARP packet that arrived on netif. Is freed by this function.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
void ethpac_fpga_input(struct netif *netif, const void *pkt, u16_t pkt_len)
{
	const struct ethpac_hdr *hdr;
	const struct eth_hdr *ethhdr;
	u8_t			request_type;
	u8_t			i;
	u16_t			pkt_len_response;
	u8_t			func_code_response;
	u8_t			sub_func_code_response;
	u16_t			sequence_response;
	const u8_t		*rx_data;
	u8_t			rx_data_len;

	LWIP_ERROR("netif != NULL", (netif != NULL), return;);

	/* drop short ARP packets: we have to check for pkt_len instead of p->tot_len here
		since a struct etharp_hdr is pointed to p->payload, so it musn't be chained! */
	if (pkt_len < (SIZEOF_ETH_HDR + SIZEOF_ETHPAC_HDR)) {
		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
				("ethpac_fpga_input: packet dropped, too short (%"S16_F"/%"S16_F")\r\n", pkt_len,
						(s16_t)(SIZEOF_ETH_HDR + SIZEOF_ETHPAC_HDR)));

		mem_free((void *)pkt);
		return;
	}

	ethhdr = (struct eth_hdr *)pkt;
	hdr = (struct ethpac_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR);
	rx_data = (const u8_t *)((u8_t*)hdr + SIZEOF_ETHPAC_HDR);
//	rx_data_len = pkt_len - SIZEOF_ETH_HDR - SIZEOF_ETHPAC_HDR;

	if (!eth_addr_cmp(&ethhdr->dest, (struct eth_addr *)netif->hwaddr) ||
			!eth_addr_cmp(&ethhdr->src, (struct eth_addr *)netif->fpga_hwaddr))
	{
		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
				("ethpac_fpga_input: packet dropped, wrong destination or source\r\n"));
		mem_free((void *)pkt);
		return;
	}

	/* check packet's packet length, function code and sub function code */
	pkt_len_response = ((hdr->pkt_len & 0xff) << 8) | ((hdr->pkt_len & 0xff00) >> 8);
	func_code_response = hdr->func_code;
	sub_func_code_response = hdr->sub_func_code;

	request_type = shared_ethpac_pkt_info.request_type;

	if (request_type == ETHPAC_REQUEST_TYPE_GET)
	{
		if ((func_code_response != shared_ethpac_pkt_info.func_code)
				|| (sub_func_code_response != shared_ethpac_pkt_info.sub_func_code))
		{
			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
					("etharp_arp_input: packet dropped, illegal function code or sub function code\r\n"));
			mem_free((void *)pkt);
			return;
		}
	}
	else
	{
		if ((func_code_response != FUNC_CODE_SET_RESPONSE)
				|| (sub_func_code_response != SUB_FUNC_CODE_SET_RESPONSE))
		{
			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
					("etharp_arp_input: packet dropped, illegal function code or sub function code\r\n"));
			mem_free((void *)pkt);
			return;
		}
	}

	for (i = 0; i< ETHPAC_RESPONSE_PKT_LEN_TABLE_SIZE; i++)
	{
		if ((ethpac_response_pkt_len_table[i].pkt_len == pkt_len_response) &&
				(ethpac_response_pkt_len_table[i].func_code == func_code_response) &&
				(ethpac_response_pkt_len_table[i].sub_func_code == sub_func_code_response))
			break;
	}

	if (i == ETHPAC_RESPONSE_PKT_LEN_TABLE_SIZE)
	{
		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
				("etharp_arp_input: packet dropped, illegal packet length, function code or sub function code\r\n"));
		mem_free((void *)pkt);
		return;
	}

	/* check the response packet when the request type is ETHPAC_REQUEST_TYPE_SET */
	if (request_type == ETHPAC_REQUEST_TYPE_SET)
	{
		sequence_response = (rx_data[0] << 8) | rx_data[1];
		func_code_response = rx_data[2];
		sub_func_code_response = rx_data[3];

		if ((sequence_response != shared_ethpac_pkt_info.sequence) ||
				(func_code_response != shared_ethpac_pkt_info.func_code)
				|| (sub_func_code_response != shared_ethpac_pkt_info.sub_func_code))
		{
			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
					("etharp_arp_input: packet dropped, illegal sequence, function code or sub function code\r\n"));
			mem_free((void *)pkt);
			return;
		}
	}
	else if (request_type == ETHPAC_REQUEST_TYPE_GET)
	{
		rx_data_len = pkt_len_response - 2;
		memcpy((void *)shared_ethpac_pkt_info.rx_data, (const void *)rx_data, rx_data_len);
		shared_ethpac_pkt_info.rx_data_len = rx_data_len;
	}

	shared_ethpac_pkt_info.done = 1;

	/* free ARP packet */
	mem_free((void *)pkt);
}







































/**
 * Responds to ARP requests to us. Upon ARP replies to us, add entry to cache
 * send out queued IP packets. Updates cache with snooped address pairs.
 *
 * Should be called for incoming ARP packets. The pbuf in the argument
 * is freed by this function.
 *    PlanarArrayController request = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *        sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B) data
 *
 *    PlanarArrayController response for set = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *        sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B)
 *        data (sequence_request(2B) func_code_request(1B) sub_func_code_request(1B))
 *
 *        note: pkt_len = 0x06 func_code = 0x80 sub_func_code = 0x00
 *
 *    PlanarArrayController response for get = dst_mac_addr(6B) src_mac_addr(6B) flag(2B)
 *       sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B) data
 * *
 * @param netif The lwIP network interface on which the ARP packet pbuf arrived.
 * @param p The ARP packet that arrived on netif. Is freed by this function.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
void ethpac_pc_input(struct netif *netif, const void *pkt, u16_t pkt_len)
{
	const struct ethpac_hdr *hdr;
	const struct eth_hdr *ethhdr;
	u8_t			request_type;
	u8_t			i;
	u16_t			pkt_len_request;
	u8_t			func_code_request;
	u8_t			sub_func_code_request;
	u16_t			sequence_request;
	const u8_t		*rx_data;
	u8_t			rx_data_len;

	LWIP_ERROR("netif != NULL", (netif != NULL), return;);

	/* drop short ARP packets: we have to check for pkt_len instead of p->tot_len here
		since a struct etharp_hdr is pointed to p->payload, so it musn't be chained! */
	if (pkt_len < (SIZEOF_ETH_HDR + SIZEOF_ETHPAC_HDR)) {
		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
				("ethpac_fpga_input: packet dropped, too short (%"S16_F"/%"S16_F")\r\n", pkt_len,
						(s16_t)(SIZEOF_ETH_HDR + SIZEOF_ETHPAC_HDR)));

		mem_free((void *)pkt);
		return;
	}

	ethhdr = (struct eth_hdr *)pkt;
	hdr = (struct ethpac_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR);
	rx_data = (const u8_t *)((u8_t*)hdr + SIZEOF_ETHPAC_HDR);

	if (!eth_addr_cmp(&ethhdr->dest, (struct eth_addr *)netif->hwaddr))
	{
		LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
				("ethpac_fpga_input: packet dropped, wrong destination or source\r\n"));
		mem_free((void *)pkt);
		return;
	}

	/* check packet's packet length, function code and sub function code */
	pkt_len_request = ((hdr->pkt_len & 0xff) << 8) | ((hdr->pkt_len & 0xff00) >> 8);
	func_code_request = hdr->func_code;
	sub_func_code_request = hdr->sub_func_code;


//	/* check the request packet when the request type is ETHPAC_REQUEST_TYPE_SET */
//	if (request_type == ETHPAC_REQUEST_TYPE_SET)
//	{
//		sequence_request = (rx_data[0] << 8) | rx_data[1];
//		func_code_request = rx_data[2];
//		sub_func_code_request = rx_data[3];
//
//		if ((sequence_request != shared_ethpac_pkt_info.sequence) ||
//				(func_code_request != shared_ethpac_pkt_info.func_code)
//				|| (sub_func_code_request != shared_ethpac_pkt_info.sub_func_code))
//		{
//			LWIP_DEBUGF(ETHARP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
//					("etharp_arp_input: packet dropped, illegal sequence, function code or sub function code\r\n"));
//			mem_free((void *)pkt);
//			return;
//		}
//	}
//	else if (request_type == ETHPAC_REQUEST_TYPE_GET)
//	{
//		rx_data_len = pkt_len_request - 2;
//		memcpy((void *)shared_ethpac_pkt_info.rx_data, (const void *)rx_data, rx_data_len);
//		shared_ethpac_pkt_info.rx_data_len = rx_data_len;
//	}


	/* free ARP packet */
	mem_free((void *)pkt);
}





























