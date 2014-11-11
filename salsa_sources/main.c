// This program tests the library Salsa20 developer eSTREAM.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "ecrypt-sync.h"

#define BUFLEN	5000000

// Struct for time value 
struct timeval t1, t2, dtv;

uint8_t m[BUFLEN];
uint8_t c[BUFLEN];
uint8_t d[BUFLEN];
uint8_t k[32];
uint8_t v[8];

void
time_start()
{
	gettimeofday(&t1, NULL);
}

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

int
main()
{
	ECRYPT_ctx x;
	long int bytes = BUFLEN, len = 256;
	
	memset(m, 'q', sizeof(m));
	memset(k, 'k', sizeof(k));
	memset(v, 1, sizeof(v));
	
	time_start();
	
	if(len == 256)
		ECRYPT_keysetup(&x, k, 256, 64);
	else
		ECRYPT_keysetup(&x, k, 128, 64);

	ECRYPT_ivsetup(&x, v);
	ECRYPT_encrypt_bytes(&x, m, c, bytes);
	
	ECRYPT_ivsetup(&x, v);
	ECRYPT_decrypt_bytes(&x, c, d, bytes);
		
	printf("Run time = %ld\n\n", time_stop());
	
	return 0;
}

