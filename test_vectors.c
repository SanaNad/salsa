#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "salsa.h"


int
main()
{
	struct salsa_context *ctx;
	uint32_t x[16];
	
	// Test vectors for the salsa20 hash function
	x[ 0] = 0x730D9FD3; x[ 1] = 0xB752374C; x[ 2] = 0x25DE7503; x[ 3] = 0x88EABBBF;
	x[ 4] = 0x30B3ED31; x[ 5] = 0xDBB26A01; x[ 6] = 0x30A6C7AF; x[ 7] = 0xCFB31056;
	x[ 8] = 0x3F20F01F; x[ 9] = 0xA15D530F; x[10] = 0x71309374; x[11] = 0x24CC37EE;
	x[12] = 0x4FEBC94F; x[13] = 0x2F9C5103; x[14] = 0xF3F41ACB; x[15] = 0x36687658;
	
	if((ctx = salsa_context_new()) == NULL) {
		printf("Memory allocation error!\n");
		exit(1);
	}

	salsa_test_vectors(ctx, x);

	salsa_context_free(&ctx);

	return 0;

}

