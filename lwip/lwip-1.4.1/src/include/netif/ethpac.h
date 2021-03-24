#ifndef __NETIF_ETHPAC_H__
#define __NETIF_ETHPAC_H__

#include "lwip/opt.h"

#include "lwip/netif.h"
#include "netif/etharp.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
/** the pac message packet format
 * sequence(2B) pkt_len(2B) func_code(1B) sub_func_code(1B)*/
struct ethpac_hdr {
	PACK_STRUCT_FIELD(u16_t sequence);
	PACK_STRUCT_FIELD(u16_t pkt_len);
	PACK_STRUCT_FIELD(u8_t  func_code);
	PACK_STRUCT_FIELD(u8_t  sub_func_code);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define SIZEOF_ETHPAC_HDR			6

#define MIN_ETHPAC_PKT_LEN			60


#define ethpac_init() /* Compatibility define, not init needed. */

err_t ethpac_get_fpga_ver(u8_t fpga_ver[9]);

err_t ethpac_set_group_buf(u8_t group_id, u16_t group_size,
		const u8_t *group_data, u32_t group_data_len);

void ethpac_fpga_input(struct netif *netif, const void *pkt, u16_t pkt_len);

#ifdef __cplusplus
}
#endif


#endif /* __NETIF_ETHPAC_H__ */
