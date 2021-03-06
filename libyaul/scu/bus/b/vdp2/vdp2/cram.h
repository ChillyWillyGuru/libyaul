/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CRAM_H_
#define _CRAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CRAM_BANK(x, y)         (0x25F00000 + ((x) << 5) + ((y) << 1))

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CRAM_H_ */
