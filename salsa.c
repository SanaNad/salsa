#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "salsa.h"

#define	ROTL32(v, n)	((v << n) | (v >> (32 - n)))
#define SALSA16		16
#define	SALSA32		32

#define LITTLEENDIAN(b)	(b[0] + (b[1] << 8) + (b[2] << 16) + (b[3] << 24))

struct salsa_context {
	int keylen;
	uint8_t key[SALSA32];
	uint8_t seq[64];
	uint8_t nonce[16];
};

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

void
salsa_context_free(struct salsa_context **ctx)
{
	memset((*ctx)->key, 0, sizeof((*ctx)->key));
	memset((*ctx)->seq, 0, sizeof((*ctx)->seq));
	memset((*ctx)->nonce, 0, sizeof((*ctx)->nonce));

	free(*ctx);
	*ctx = NULL;
}

int
salsa_set_key_and_nonce(struct salsa_context *ctx, uint8_t *key, int keylen, uint8_t nonce[8])
{
	if(keylen <= SALSA16)
		ctx->keylen = SALSA16;
	else 
		if(keylen <= SALSA32)
			ctx->keylen = SALSA32;
	     	else
	     		return -1;

	memcpy(ctx->key, key, keylen);

	memcpy(ctx->nonce, nonce, 8);

	return 0;
}

void
quarterround(uint32_t *y0, uint32_t *y1, uint32_t *y2, uint32_t *y3)
{
	*y1 = *y1 ^ (ROTL32((*y0 + *y3), 7));
	*y2 = *y2 ^ (ROTL32((*y1 + *y0), 9));
	*y3 = *y3 ^ (ROTL32((*y2 + *y1), 13));
	*y0 = *y0 ^ (ROTL32((*y3 + *y2), 18));
}

void
rowround(uint32_t y[16])
{
	quarterround(&y[0], &y[1], &y[2], &y[3]);
	quarterround(&y[5], &y[6], &y[7], &y[4]);
	quarterround(&y[10], &y[11], &y[8], &y[9]);
	quarterround(&y[15], &y[12], &y[13], &y[14]);
}

void
columnround(uint32_t x[16])
{
	quarterround(&x[0], &x[4], &x[8], &x[12]);
	quarterround(&x[5], &x[9], &x[13], &x[1]);
	quarterround(&x[10], &x[14], &x[2], &x[6]);
	quarterround(&x[15], &x[3], &x[7], &x[11]);
}

void
doubleround(uint32_t x[16])
{
	columnround(x);
	rowround(x);
}

/*uint32_t
littleendian(uint8_t *b)
{
	return (b[0] + (b[1] << 8) + (b[2] << 16) + (b[3] << 24));
}*/

/*void
rev_littleendian(uint32_t x, uint8_t *b)
{
	b[0] = x;
	b[1] = x >> 8;
	b[2] = x >> 16;
	b[3] = x >> 24;
}*/

void
salsa20(uint8_t seq[64])
{
	uint32_t x[16], z[16];
	int i, j;

	for(i = 0; i < 16; i++)
		//x[i] = z[i] = littleendian(seq + (i * 4));
		x[i] = z[i] = LITTLEENDIAN((seq + (i * 4)));
	for(i = 0; i < 10; i++)
		doubleround(z);
	
	j = 0;

	for(i = 0; i < 16; i++) {
		z[i] += x[i];
		seq[j++] = (uint8_t)(z[i]);
		seq[j++] = (uint8_t)(z[i] >> 8);
		seq[j++] = (uint8_t)(z[i] >> 16);
		seq[j++] = (uint8_t)(z[i] >> 24);
	}
}

void
salsa_expand_key(struct salsa_context *ctx)
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
	
	switch(ctx->keylen) {
	case SALSA16 :
		expand = (uint8_t *)key_expand_16;
		break;
	case SALSA32 :
		expand = (uint8_t *)key_expand_32;
		break;
	}

	j = 0;
	
	for(i = 0; i < 64; i+=20) {
		ctx->seq[i] = expand[j++];
		ctx->seq[i + 1] = expand[j++];
		ctx->seq[i + 2] = expand[j++];
		ctx->seq[i + 3] = expand[j++];
	}

	for(i = 0; i < 16; i++) {
		ctx->seq[4 + i] = ctx->key[i];
		ctx->seq[24 + i] = ctx->nonce[i];
		ctx->seq[44 + i] = ctx->key[ctx->keylen - 16 + i];
	}

	salsa20(ctx->seq);
}

void
salsa_encrypt(struct salsa_context *ctx, uint8_t *buf, int buflen)
{
	int i;

	salsa_expand_key(ctx);	
		
	for(i = 0; i < buflen; i++)
		buf[i] ^= ctx->seq[i % 64];
}

void
salsa_decrypt(struct salsa_context *ctx, uint8_t *buf, int buflen)
{
	salsa_encrypt(ctx, buf, buflen);
}

void
test(struct salsa_context *ctx)
{
	int i;

	for(i = 0; i < 32; i++)
		printf("%d ", ctx->key[i]);
	printf("\n\n");

	for(i = 0; i < 8; i++)
		printf("%d - ", ctx->nonce[i]);
	printf("\n\n");
	printf("keylen = %d\n", ctx->keylen);
}


