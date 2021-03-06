/* ====================================================================
 * Copyright (c) 1995-2006 Carnegie Mellon University.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/*********************************************************************
 *
 * File: v4_feat.c
 * 
 * Description: 
 *
 * Author: 
 * 	Eric H. Thayer (eht@cs.cmu.edu)
 *********************************************************************/

/* static char rcsid[] = "@(#)$Id$"; */

#include "v4_feat.h"

#include <s3/feat.h>

#include "cep_frame.h"
#include "dcep_frame.h"
#include "ddcep_frame.h"

#include <s3/agc.h>
#include <s3/cmn.h>
#include <s3/silcomp.h>

#include <s3/ckd_alloc.h>
#include <s3/cmd_ln.h>
#include <s3/s3.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#define N_FEAT		1

static uint32 n_feat = N_FEAT;
static uint32 vecsize[1];
static uint32 mfcc_len;
static int32 window_size;

const char *
v4_feat_doc()
{
    return "1 stream :== < input feature vector >";
}

uint32
v4_feat_id()
{
    return FEAT_ID_V4;
}

uint32
v4_feat_n_stream()
{
    return n_feat;
}

uint32
v4_feat_blksize()
{
    return vecsize[0];
}

const uint32 *
v4_feat_vecsize()
{
    return vecsize;
}

void
v4_feat_set_in_veclen(uint32 veclen)
{
    mfcc_len = veclen;
    window_size = cmd_ln_int32("-cepwin");

    vecsize[0] = veclen * (window_size * 2 + 1);

    cmn_set_veclen(veclen);
    agc_set_veclen(veclen);
}

vector_t **
v4_feat_compute(vector_t *mfcc,
		uint32 *inout_n_frame)
{
    vector_t **out;
    uint32 svd_n_frame;
    uint32 n_frame;
    const char *comp_type = cmd_ln_access("-silcomp");
    int32 i, j;
    uint32 mfcc_len;
    void v4_mfcc_print(vector_t *mfcc, uint32 n_frame);
    
    mfcc_len = feat_mfcc_len();		/* # of coefficients c[0..MFCC_LEN-1] per frame */

    n_frame = svd_n_frame = *inout_n_frame;

    if (strcmp(comp_type, "none") != 0) {
	n_frame = sil_compression(comp_type, mfcc, n_frame);
    }

    cmn(&mfcc[0][0], n_frame);
    agc(&mfcc[0][0], n_frame);

    out = feat_alloc(n_frame);

    for (i = 0; i < n_frame; ++i) {
	uint32 spos = 0;

	for (j = -window_size; j <= window_size; ++j) {
	    int32 ii;

	    /* Duplicate the first and last frames for out-of-bounds
	       window indices. */
	    if (i + j < 0)
		ii = -j;
	    else if (i + j >= n_frame)
		ii = n_frame - j - 1;
	    else
		ii = i;

	    memcpy(out[i][0] + spos, mfcc[(int32)ii + j], mfcc_len * sizeof(float32));
	    spos += mfcc_len;
	}
    }

    *inout_n_frame = n_frame;

    return out;
}

void
v4_mfcc_print(vector_t *mfcc, uint32 n_frame)
{
    uint32 i, k;
    uint32 mfcc_len;

    mfcc_len = feat_mfcc_len();

    for (i = 0; i < n_frame; i++) {
	printf("mfcc[%04u]: ", i);
	for (k = 0; k < mfcc_len; k++) {
	    printf("%6.3f ", mfcc[i][k]);
	}
	printf("\n");
    }
}


void
v4_feat_print(const char *label,
	      vector_t **f,
	      uint32 n_frames)
{
    uint32 i;
    int32 j;
    uint32 k;
    char *name[] = { "" };
    vector_t *frame;
    vector_t stream;

    for (i = 0; i < n_frames; i++) {
	frame = f[i];

	for (j = 0; j < n_feat; j++) {
	    stream = frame[j];

	    printf("%s%s[%04u]: ", label, name[j], i);
	    for (k = 0; k < vecsize[j]; k++) {
		printf("%6.3f ", stream[k]);
	    }
	    printf("\n");
	}
	printf("\n");
    }
}

/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.4  2004/07/21  18:05:38  egouvea
 * Changed the license terms to make it the same as sphinx2 and sphinx3.
 * 
 * Revision 1.3  2001/04/05 20:02:30  awb
 * *** empty log message ***
 *
 * Revision 1.2  2000/09/29 22:35:13  awb
 * *** empty log message ***
 *
 * Revision 1.1  2000/09/24 21:38:31  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:36:22  eht
 * Initial revision
 * 
 */
