#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "salsa.h"

int main()
{
	struct salsa_context *ctx;
	uint8_t *buf, *k, nonce[8], key[32];
	char s[10];
	int buflen, keylen, i;
	
	keylen = 32;
	buflen = 10;

	memset(nonce, 1, sizeof(nonce));
	memset(s, 'q', sizeof(s));
	memset(key, 'w', sizeof(key));

	if((ctx = salsa_context_new()) == NULL) {
		printf("Error allocates memory!\n");
		exit(1);
	}
	
	k = &key;
	buf = (uint8_t *)&s[0];

	salsa_set_key_and_nonce(ctx, k, keylen, nonce);

	salsa_encrypt(ctx, (uint8_t *)s, buflen);

	for(i = 0; i < buflen; i++)
		printf(" %c -", buf[i]);
	printf("\n\n");

	salsa_decrypt(ctx, buf, buflen);

	for(i = 0; i < buflen; i++)
		printf(" %c ", buf[i]);
	printf("\n\n");

	salsa_context_free(&ctx);

	return 0;
}

