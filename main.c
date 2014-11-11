/* This program tests the library Salsa20 - salsa.h
 * Minimum data. Only for test.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/time.h>

#include "salsa.h"

#define	BUFLEN	5000000
#define KEYLEN	32

// Struct for time value
struct timeval t1, t2, dtv;

// Function time measurement
void
time_start()
{
	gettimeofday(&t1, NULL);
}

// Function time measurement
long
time_stop()
{
	gettimeofday(&t2, NULL);

	dtv.tv_sec = t2.tv_sec - t1.tv_sec;
	dtv.tv_usec = t2.tv_usec - t1.tv_usec;

	if(dtv.tv_usec < 0) {
		dtv.tv_sec--;
		dtv.tv_usec += 1000000;
	}

	return dtv.tv_sec * 1000 + dtv.tv_usec/1000;
}

int main()
{
	struct salsa_context *ctx;
	uint8_t *buf, nonce[8], key[KEYLEN];
	char s[BUFLEN];
	long int buflen, keylen;
	
	keylen = KEYLEN;
	buflen = BUFLEN;
	
	memset(s, 'q', sizeof(s));
	memset(key, 'k', sizeof(key));
	memset(nonce, 1, sizeof(nonce));
	
	time_start();
	
	if((ctx = salsa_context_new()) == NULL) {
		printf("Memory allocation error!\n");
		exit(1);
	}
	
	buf = (uint8_t *)&s[0];

	if (salsa_set_key_and_nonce(ctx, (uint8_t *)key, keylen, nonce)) {
		printf("Salsa context filling error!\n");
		exit(1);
	}
	
	salsa_encrypt(ctx, (uint8_t *)s, buflen);

	if(salsa_set_key_and_nonce(ctx, (uint8_t *)key, keylen, nonce)) {
		printf("Salsa context filling error!\n");
		exit(1);
	}

	salsa_decrypt(ctx, buf, buflen);

	printf("Run time = %ld\n\n", time_stop());
	
	salsa_context_free(&ctx);

	return 0;
}

