#ifndef SALSA_H_
#define SALSA_H_

struct salsa_context;

struct salsa_context *salsa_context_new();

void salsa_context_free(struct salsa_context **ctx);

int salsa_set_key_and_nonce(struct salsa_context *ctx, uint8_t *key, int keylen, uint8_t nonce[16]);

void salsa_encrypt(struct salsa_context *ctx, uint8_t *buf, int buflen);
void salsa_decrypt(struct salsa_context *ctx, uint8_t *buf, int buflen);

void test(struct salsa_context *ctx);

//void salsa20(uint8_t seq[64]);
#endif
