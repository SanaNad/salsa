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
 * seq - array of intermediate values
 * nonce - 16-byte array with a unique number. 8 bytes are filled by the user
*/
struct salsa_context {
	int keylen;
	uint8_t key[SALSA32];
	uint8_t seq[64];
	uint8_t nonce[16];
};

// Allocates memory for the salsa context
struct salsa_context * 
salsa_context_new()
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
salsa_set_key_and_nonce(struct salsa_context *ctx, uint8_t *key, int keylen, uint8_t nonce[8])
{
	int i;

	if(keylen <= SALSA16)
		ctx->keylen = SALSA16;
	else if(keylen <= SALSA32)
		ctx->keylen = SALSA32;
	else
	     	return -1;

	memcpy(ctx->key, key, keylen);

	// Fill the nonce user data: nonce[0] - nonce[7].
	memcpy(ctx->nonce, nonce, 8);
	
	// Fill the nonce: nonce[8] - nonce[15].
	for(i = 8; i < 16; i++)
		ctx->nonce[i] = i;

	return 0;
}

// Salsa hash function
static void
salsa20(struct salsa_context *ctx, uint32_t x[16])
{
	uint32_t z[16];
	int i, j;

	for(i = 0; i < 16; i++)
		z[i] = x[i];

	for(i = 0; i < 10; i++) {
		x[ 4] ^= ROTL32((x[ 0] + x[12]), 7);
		x[ 8] ^= ROTL32((x[ 4] + x[ 0]), 9);
		x[12] ^= ROTL32((x[ 8] + x[ 4]), 13);
		x[ 0] ^= ROTL32((x[12] + x[ 8]), 18);

		x[ 9] ^= ROTL32((x[ 5] + x[ 1]), 7);
		x[13] ^= ROTL32((x[ 9] + x[ 5]), 9);
		x[ 1] ^= ROTL32((x[13] + x[ 9]), 13);
		x[ 5] ^= ROTL32((x[ 1] + x[13]), 18);

		x[14] ^= ROTL32((x[10] + x[ 6]), 7);
		x[ 2] ^= ROTL32((x[14] + x[10]), 9);
		x[ 6] ^= ROTL32((x[ 2] + x[14]), 13);
		x[10] ^= ROTL32((x[ 6] + x[ 2]), 18);

		x[ 3] ^= ROTL32((x[15] + x[11]), 7);
		x[ 7] ^= ROTL32((x[ 3] + x[15]), 9);
		x[11] ^= ROTL32((x[ 7] + x[ 3]), 13);
		x[15] ^= ROTL32((x[11] + x[ 7]), 18);
	
		x[ 1] ^= ROTL32((x[ 0] + x[ 3]), 7);
		x[ 2] ^= ROTL32((x[ 1] + x[ 0]), 9);
		x[ 3] ^= ROTL32((x[ 2] + x[ 1]), 13);
		x[ 0] ^= ROTL32((x[ 3] + x[ 2]), 18);

		x[ 6] ^= ROTL32((x[ 5] + x[ 4]), 7);
		x[ 7] ^= ROTL32((x[ 6] + x[ 5]), 9);
		x[ 4] ^= ROTL32((x[ 7] + x[ 6]), 13);
		x[ 5] ^= ROTL32((x[ 4] + x[ 7]), 18);
	
		x[11] ^= ROTL32((x[10] + x[ 9]), 7);
		x[ 8] ^= ROTL32((x[11] + x[10]), 9);
		x[ 9] ^= ROTL32((x[ 8] + x[11]), 13);
		x[10] ^= ROTL32((x[ 9] + x[ 8]), 18);

		x[12] ^= ROTL32((x[15] + x[14]), 7);
		x[13] ^= ROTL32((x[12] + x[15]), 9);
		x[14] ^= ROTL32((x[13] + x[12]), 13);
		x[15] ^= ROTL32((x[14] + x[13]), 18);
	}
	
	j = 0;

	for(i = 0; i < 16; i++) {
		z[i] += x[i];
		ctx->seq[j++] = (uint8_t)(z[i]);
		ctx->seq[j++] = (uint8_t)(z[i] >> 8);
		ctx->seq[j++] = (uint8_t)(z[i] >> 16);
		ctx->seq[j++] = (uint8_t)(z[i] >> 24);
	}
}

// Salsa expansion of key. Expansion of key depends from key length
static void
salsa_expand_key(struct salsa_context *ctx)
{
	int i, j;
	uint32_t x[16];
	uint8_t *expand;
		
	// Unique constant for key length 16 byte
	uint8_t key_expand_16 [] = {
		'e', 'x', 'p', 'a',
		'n', 'd', ' ', '1',
		'6', '-', 'b', 'y',
		't', 'e', ' ', 'k'
	};

	// Unique constant for key length 32 byte
	uint8_t key_expand_32 [] = {
		'e', 'x', 'p', 'a',
		'n', 'd', ' ', '3',
		'2', '-', 'b', 'y',
		't', 'e', ' ', 'k'
	};
	
	switch(ctx->keylen) {
	case SALSA16 :
		expand = (uint8_t *)key_expand_16;
		j = 0;
		break;
	case SALSA32 :
		expand = (uint8_t *)key_expand_32;
		j = 4;
		break;
	}

	for(i = 0; i < 4; i++) {
		x[i * 5] = LITTLEENDIAN((expand + (i * 4)));
		x[i + 1] = LITTLEENDIAN((ctx->key + (i * 4)));
		x[i + 6] = LITTLEENDIAN((ctx->nonce + (i * 4)));
		x[i + 11] = LITTLEENDIAN((ctx->key + ((j + i) * 4)));
	}

	// Salsa hash function
	salsa20(ctx, x);
}

/* Salsa encrypt algorithm.
 * ctx - pointer on salsa context
 * buf - pointer on buffer data
 * buflen - length the data buffer
*/
void
salsa_encrypt(struct salsa_context *ctx, uint8_t *buf, int buflen)
{
	int i;

	salsa_expand_key(ctx);	
		
	for(i = 0; i < buflen; i++)
		buf[i] ^= ctx->seq[i % 64];
}

// Salsa decrypt function. See salsa_encrypt
void
salsa_decrypt(struct salsa_context *ctx, uint8_t *buf, int buflen)
{
	salsa_encrypt(ctx, buf, buflen);
}

