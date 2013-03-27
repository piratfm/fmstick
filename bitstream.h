/*
 * FILE:    bitstream.h
 * PROGRAM: RAT
 * AUTHOR:  Orion Hodson
 *
 * Copyright (c) 1998-2001 University College London
 * All rights reserved.
 *
 * $Id$
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_bitstream bitstream_t;

int  bs_create     (bitstream_t **b);

int  bs_destroy    (bitstream_t **b);

int  bs_attach     (bitstream_t *b,
                    uint8_t *buf,
                    int blen);

int  bs_put        (bitstream_t *b,
                    uint8_t       bits,
                    uint8_t       nbits);

uint8_t bs_get      (bitstream_t *b,
                    uint8_t  nbits);

int  bs_bytes_used (bitstream_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BITSTREAM_H */
