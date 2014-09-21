/* This library implements the SALSA20 algorithm
 * Developer - Daniel J. Bernstein.
 * SALSA20 - the winner eSTREAM. Home page - http://www.ecrypt.eu.org/stream/.
*/

#ifndef SALSA_H_
#define SALSA_H_

struct salsa_context;

struct salsa_context *salsa_context_new();

void salsa_context_free(struct salsa_context **ctx);

int salsa_set_key_and_nonce(struct salsa_context *ctx, uint8_t *key, int keylen, uint8_t nonce[16]);

void salsa_encrypt(struct salsa_context *ctx, uint8_t *buf, int buflen);
void salsa_decrypt(struct salsa_context *ctx, uint8_t *buf, int buflen);

#endif
