/* This program tests the library Salsa20 - salsa.h
 * Minimum data. Only for test.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "salsa.h"


int main()
{
	struct salsa_context *ctx;
	uint8_t *buf, *k, nonce[8], key[32];
	char s[10000];
	int buflen, keylen, i;
	
	keylen = 32;
	buflen = 10000;
	
	memset(s, 'q', sizeof(s));
	memset(key, 'k', sizeof(key));
	memset(nonce, 1, sizeof(nonce));

	if((ctx = salsa_context_new()) == NULL) {
		printf("Error allocates memory!\n");
		exit(1);
	}
	
	k = &key;
	buf = (uint8_t *)&s[0];

	if (salsa_set_key_and_nonce(ctx, k, keylen, nonce)) {
		printf("Error fill salsa context!\n");
		exit(1);
	}

	salsa_encrypt(ctx, (uint8_t *)s, buflen);
	
	// print encrypted data
	for(i = 0; i < buflen; i++)
		printf(" %c -", buf[i]);
	printf("\n\n");

	salsa_decrypt(ctx, buf, buflen);

	// print decrypted data
	for(i = 0; i < buflen; i++)
		printf(" %c ", buf[i]);
	printf("\n\n");

	salsa_context_free(&ctx);

	return 0;
}

