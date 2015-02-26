#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "salsa.h"

int
main(void)
{
	uint8_t key1[32] = { 0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00 };
	
	uint8_t key2[32] = { 0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00,
			     0x00, 0x00, 0x00, 0x00 };
	
	uint8_t iv1[8] = { 0x00, 0x00, 0x00, 0x00,
			   0x00, 0x00, 0x00, 0x00 };
	
	uint8_t iv2[8] = { 0x01, 0x23, 0x45, 0x67,
			   0x89, 0xAB, 0xCD, 0xEF };
	
	struct salsa_context *ctx;
	
	if((ctx = salsa_context_new()) == NULL) {
		printf("Memory allocation error!\n");
		exit(1);
	}

	if(salsa_set_key_and_iv(ctx, key1, 32, iv1)) {
		printf("Salsa context filling error!\n");
		exit(1);
	}
	
	salsa_test_vectors(ctx);

	if(salsa_set_key_and_iv(ctx, key2, 32, iv2)) {
		printf("Salsa context filling error!\n");
		exit(1);
	}

	salsa_test_vectors(ctx);

	salsa_context_free(&ctx);

	return 0;
}
