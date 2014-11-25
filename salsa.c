/* This program implements the SALSA20 algorithm.
 * Salsa20 developed Daniel J. Bernstein. Winner the eSTREAM.
 * The SALSA20 home page - http://www.ecrypt.eu.org/stream/.
 * ----------------------
 * Author: Rostislav Gashin (rost1993). The State University of Syktyvkar (Amplab).
 * Assistant project manager: Lipin Boris (dzruyk).
 * Project manager: Grisha Sitkarev.
 * ----------------------
 * Salsa20 operations based on a 32-bit summation bitwise (XOR) and shift operations.
 * The algorithm uses a hash function with 20 cycles.
 * ----------------------
 * Russia, Komi Republic, Syktyvkar - 29.09.2014, version 2.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "salsa.h"

#define SALSA16		16
#define	SALSA32		32

#define	ROTL32(v, n)	((v << n) | (v >> (32 - n)))
#define LITTLEENDIAN(b)	(b[0] + (b[1] << 8) + (b[2] << 16) + (b[3] << 24))


/* Salsa context
 * keylen - chiper key length
 * key - chiper key
 * nonce - 16-byte array with a unique number. 8 bytes are filled by the user
 * x - intermediate array
*/
struct salsa_context {
	int keylen;
	uint8_t key[SALSA32];
	uint8_t nonce[16];
	uint32_t x[16];
};

// Allocates memory for the salsa context
struct salsa_context * 
salsa_context_new(void)
{
	struct salsa_context *ctx;
	ctx = malloc(sizeof(*ctx));

	if(ctx == NULL)
		return NULL;

	memset(ctx, 0, sizeof(ctx));
	
	return ctx;
}

// Delete salsa context
void
salsa_context_free(struct salsa_context **ctx)
{
	free(*ctx);
	*ctx = NULL;
}

// Fill the salsa context (key and nonce)
// Return value: 0 (if all is well), -1 (if all bad)
int
salsa_set_key_and_nonce(struct salsa_context *ctx, const uint8_t *key, const int keylen, const uint8_t nonce[8])
{
	int i, j;
	uint8_t *expand;

	uint8_t key_expand_16 [] = {
		'e', 'x', 'p', 'a',
		'n', 'd', ' ', '1',
		'6', '-', 'b', 'y',
		't', 'e', ' ', 'k'
	};

	uint8_t key_expand_32 [] = {
		'e', 'x', 'p', 'a',
		'n', 'd', ' ', '3',
		'2', '-', 'b', 'y',
		't', 'e', ' ', 'k'
	};

	if(keylen <= SALSA16) {
		ctx->keylen = SALSA16;
		expand = (uint8_t *)key_expand_16;
		j = 0;
	}
	else if(keylen <= SALSA32) {
		ctx->keylen = SALSA32;
		expand = (uint8_t *)key_expand_32;
		j = 4;
	}
	else
	     	return -1;

	memcpy(ctx->key, key, keylen);

	// Fill the nonce user data: nonce[0] - nonce[7].
	memcpy(ctx->nonce, nonce, 8);
	
	// Fill the nonce: nonce[8] - nonce[15].
	for(i = 8; i < 16; i++)
		ctx->nonce[i] = 0;
		
	for(i = 0; i < 4; i++) {
		ctx->x[i *  5] = LITTLEENDIAN((expand + (i * 4)));
		ctx->x[i +  1] = LITTLEENDIAN((ctx->key + (i * 4)));
		ctx->x[i +  6] = LITTLEENDIAN((ctx->nonce + (i * 4)));
		ctx->x[i + 11] = LITTLEENDIAN((ctx->key + ((j + i) * 4)));
	}

	return 0;
}

// Salsa hash function
static void
salsa20(struct salsa_context *ctx, uint8_t seq[64])
{
	uint32_t z[16];
	int i, j;

	for(i = 0; i < 16; i++)
		z[i] = ctx->x[i];

	for(i = 0; i < 10; i++) {
		z[ 4] ^= ROTL32((z[ 0] + z[12]), 7);
		z[ 8] ^= ROTL32((z[ 4] + z[ 0]), 9);
		z[12] ^= ROTL32((z[ 8] + z[ 4]), 13);
		z[ 0] ^= ROTL32((z[12] + z[ 8]), 18);

		z[ 9] ^= ROTL32((z[ 5] + z[ 1]), 7);
		z[13] ^= ROTL32((z[ 9] + z[ 5]), 9);
		z[ 1] ^= ROTL32((z[13] + z[ 9]), 13);
		z[ 5] ^= ROTL32((z[ 1] + z[13]), 18);

		z[14] ^= ROTL32((z[10] + z[ 6]), 7);
		z[ 2] ^= ROTL32((z[14] + z[10]), 9);
		z[ 6] ^= ROTL32((z[ 2] + z[14]), 13);
		z[10] ^= ROTL32((z[ 6] + z[ 2]), 18);

		z[ 3] ^= ROTL32((z[15] + z[11]), 7);
		z[ 7] ^= ROTL32((z[ 3] + z[15]), 9);
		z[11] ^= ROTL32((z[ 7] + z[ 3]), 13);
		z[15] ^= ROTL32((z[11] + z[ 7]), 18);
	
		z[ 1] ^= ROTL32((z[ 0] + z[ 3]), 7);
		z[ 2] ^= ROTL32((z[ 1] + z[ 0]), 9);
		z[ 3] ^= ROTL32((z[ 2] + z[ 1]), 13);
		z[ 0] ^= ROTL32((z[ 3] + z[ 2]), 18);

		z[ 6] ^= ROTL32((z[ 5] + z[ 4]), 7);
		z[ 7] ^= ROTL32((z[ 6] + z[ 5]), 9);
		z[ 4] ^= ROTL32((z[ 7] + z[ 6]), 13);
		z[ 5] ^= ROTL32((z[ 4] + z[ 7]), 18);
	
		z[11] ^= ROTL32((z[10] + z[ 9]), 7);
		z[ 8] ^= ROTL32((z[11] + z[10]), 9);
		z[ 9] ^= ROTL32((z[ 8] + z[11]), 13);
		z[10] ^= ROTL32((z[ 9] + z[ 8]), 18);

		z[12] ^= ROTL32((z[15] + z[14]), 7);
		z[13] ^= ROTL32((z[12] + z[15]), 9);
		z[14] ^= ROTL32((z[13] + z[12]), 13);
		z[15] ^= ROTL32((z[14] + z[13]), 18);
	}
	
	j = 0;

	for(i = 0; i < 16; i++) {
		z[i] += ctx->x[i];
		seq[j++] = (uint8_t)(z[i]);
		seq[j++] = (uint8_t)(z[i] >> 8);
		seq[j++] = (uint8_t)(z[i] >> 16);
		seq[j++] = (uint8_t)(z[i] >> 24);
	}
}

/* Salsa encrypt algorithm.
 * ctx - pointer on salsa context
 * buf - pointer on buffer data
 * buflen - length the data buffer
*/
void
salsa_encrypt(struct salsa_context *ctx, const uint8_t *buf, uint32_t buflen, uint8_t *out)
{
	int i;
	uint8_t seq[64];

	for(;;) {
		salsa20(ctx, seq);
		ctx->x[8] += 1;

		if(!ctx->x[8])
			ctx->x[9] += 1;

		if(buflen <= 64) {
			for(i = 0; i < buflen; i++)
				out[i] = buf[i] ^ seq[i];
			return;
		}
		
		for(i = 0; i < 64; i++)
			out[i] = buf[i] ^ seq[i];
		
		buflen -= 64;
		buf += 64;
		out += 64;
	}
}

// Salsa decrypt function. See salsa_encrypt
void
salsa_decrypt(struct salsa_context *ctx, const uint8_t *buf, uint32_t buflen, uint8_t *out)
{
	salsa_encrypt(ctx, buf, buflen, out);
}

