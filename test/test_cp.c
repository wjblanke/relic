/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2019 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or modify it under the
 * terms of the version 2.1 (or later) of the GNU Lesser General Public License
 * as published by the Free Software Foundation; or version 2.0 of the Apache
 * License as published by the Apache Software Foundation. See the LICENSE files
 * for more details.
 *
 * RELIC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the LICENSE files for more details.
 *
 * You should have received a copy of the GNU Lesser General Public or the
 * Apache License along with RELIC. If not, see <https://www.gnu.org/licenses/>
 * or <https://www.apache.org/licenses/>.
 */

/**
 * @file
 *
 * Tests for implementation of cryptographic protocols.
 *
 * @version $Id$
 * @ingroup test
 */

#include <stdio.h>

#include "relic.h"
#include "relic_test.h"

static int rsa(void) {
	int code = RLC_ERR;
	rsa_t pub, prv;
	uint8_t in[10], out[RLC_BN_BITS / 8 + 1], h[RLC_MD_LEN];
	int il, ol;
	int result;

	rsa_null(pub);
	rsa_null(prv);

	TRY {
		rsa_new(pub);
		rsa_new(prv);

		result = cp_rsa_gen(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("rsa encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_enc(out, &ol, in, il, pub) == RLC_OK, end);
			TEST_ASSERT(cp_rsa_dec(out, &ol, out, ol, prv) == RLC_OK, end);
			TEST_ASSERT(memcmp(in, out, ol) == 0, end);
		} TEST_END;

#if CP_RSA == BASIC || !defined(STRIP)
		result = cp_rsa_gen_basic(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("basic rsa encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_enc(out, &ol, in, il, pub) == RLC_OK, end);
			TEST_ASSERT(cp_rsa_dec_basic(out, &ol, out, ol, prv) == RLC_OK,
					end);
			TEST_ASSERT(memcmp(in, out, ol) == 0, end);
		} TEST_END;
#endif

#if CP_RSA == QUICK || !defined(STRIP)
		result = cp_rsa_gen_quick(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("fast rsa encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_enc(out, &ol, in, il, pub) == RLC_OK, end);
			TEST_ASSERT(cp_rsa_dec_quick(out, &ol, out, ol, prv) == RLC_OK,
					end);
			TEST_ASSERT(memcmp(in, out, ol) == 0, end);
		} TEST_END;
#endif

		result = cp_rsa_gen(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("rsa signature/verification is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_sig(out, &ol, in, il, 0, prv) == RLC_OK, end);
			TEST_ASSERT(cp_rsa_ver(out, ol, in, il, 0, pub) == 1, end);
			md_map(h, in, il);
			TEST_ASSERT(cp_rsa_sig(out, &ol, h, RLC_MD_LEN, 1, prv) == RLC_OK, end);
			TEST_ASSERT(cp_rsa_ver(out, ol, h, RLC_MD_LEN, 1, pub) == 1, end);
		} TEST_END;

#if CP_RSA == BASIC || !defined(STRIP)
		result = cp_rsa_gen_basic(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("basic rsa signature/verification is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_sig_basic(out, &ol, in, il, 0, prv) == RLC_OK,
					end);
			TEST_ASSERT(cp_rsa_ver(out, ol, in, il, 0, pub) == 1, end);
			md_map(h, in, il);
			TEST_ASSERT(cp_rsa_sig_basic(out, &ol, h, RLC_MD_LEN, 1, prv) == RLC_OK,
					end);
			TEST_ASSERT(cp_rsa_ver(out, ol, h, RLC_MD_LEN, 1, pub) == 1, end);
		} TEST_END;
#endif

#if CP_RSA == QUICK || !defined(STRIP)
		result = cp_rsa_gen_quick(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("fast rsa signature/verification is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = 10;
			ol = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, il);
			TEST_ASSERT(cp_rsa_sig_quick(out, &ol, in, il, 0, prv) == RLC_OK,
					end);
			TEST_ASSERT(cp_rsa_ver(out, ol, in, il, 0, pub) == 1, end);
			md_map(h, in, il);
			TEST_ASSERT(cp_rsa_sig_quick(out, &ol, h, RLC_MD_LEN, 1, prv) == RLC_OK,
					end);
			TEST_ASSERT(cp_rsa_ver(out, ol, h, RLC_MD_LEN, 1, pub) == 1, end);
		} TEST_END;
#endif
	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	rsa_free(pub);
	rsa_free(prv);
	return code;
}

static int rabin(void) {
	int code = RLC_ERR;
	rabin_t pub, prv;
	uint8_t in[10];
	uint8_t out[RLC_BN_BITS / 8 + 1];
	int in_len, out_len;
	int result;

	rabin_null(pub);
	rabin_null(prv);

	TRY {
		rabin_new(pub);
		rabin_new(prv);

		result = cp_rabin_gen(pub, prv, RLC_BN_BITS);

		TEST_BEGIN("rabin encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			in_len = 10;
			out_len = RLC_BN_BITS / 8 + 1;
			rand_bytes(in, in_len);
			TEST_ASSERT(cp_rabin_enc(out, &out_len, in, in_len, pub) == RLC_OK,
					end);
			TEST_ASSERT(cp_rabin_dec(out, &out_len, out, out_len,
							prv) == RLC_OK, end);
			TEST_ASSERT(memcmp(in, out, out_len) == 0, end);
		} TEST_END;
	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	rabin_free(pub);
	rabin_free(prv);
	return code;
}

static int benaloh(void) {
	int code = RLC_ERR;
	bdpe_t pub, prv;
	bn_t a, b;
	dig_t in, out;
	uint8_t buf[RLC_BN_BITS / 8 + 1];
	int len;
	int result;

	bn_null(a);
	bn_null(b);
	bdpe_null(pub);
	bdpe_null(prv);

	TRY {
		bn_new(a);
		bn_new(b);
		bdpe_new(pub);
		bdpe_new(prv);

		result = cp_bdpe_gen(pub, prv, bn_get_prime(47), RLC_BN_BITS);

		TEST_BEGIN("benaloh encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			len = RLC_BN_BITS / 8 + 1;
			rand_bytes(buf, 1);
			in = buf[0] % bn_get_prime(47);
			TEST_ASSERT(cp_bdpe_enc(buf, &len, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bdpe_dec(&out, buf, len, prv) == RLC_OK, end);
			TEST_ASSERT(in == out, end);
		} TEST_END;

		TEST_BEGIN("benaloh encryption/decryption is homomorphic") {
			TEST_ASSERT(result == RLC_OK, end);
			len = RLC_BN_BITS / 8 + 1;
			rand_bytes(buf, 1);
			in = buf[0] % bn_get_prime(47);
			TEST_ASSERT(cp_bdpe_enc(buf, &len, in, pub) == RLC_OK, end);
			bn_read_bin(a, buf, len);
			rand_bytes(buf, 1);
			out = (buf[0] % bn_get_prime(47));
			in = (in + out) % bn_get_prime(47);
			TEST_ASSERT(cp_bdpe_enc(buf, &len, out, pub) == RLC_OK, end);
			bn_read_bin(b, buf, len);
			bn_mul(a, a, b);
			bn_mod(a, a, pub->n);
			len = bn_size_bin(pub->n);
			bn_write_bin(buf, len, a);
			TEST_ASSERT(cp_bdpe_dec(&out, buf, len, prv) == RLC_OK, end);
			TEST_ASSERT(in == out, end);
		} TEST_END;
	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(a);
	bn_free(b);
	bdpe_free(pub);
	bdpe_free(prv);
	return code;
}

static int paillier(void) {
	int code = RLC_ERR;
	bn_t a, b, c, d, n, l, s;
	uint8_t in[RLC_BN_BITS / 8 + 1], out[RLC_BN_BITS / 8 + 1];
	int in_len, out_len;
	int result;

	bn_null(a);
	bn_null(b);
	bn_null(c);
	bn_null(d);
	bn_null(n);
	bn_null(l);
	bn_null(s);

	TRY {
		bn_new(a);
		bn_new(b);
		bn_new(c);
		bn_new(d);
		bn_new(n);
		bn_new(l);
		bn_new(s);

		result = cp_phpe_gen(n, l, RLC_BN_BITS / 2);

		TEST_BEGIN("paillier encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			in_len = bn_size_bin(n);
			out_len = RLC_BN_BITS / 8 + 1;
			memset(in, 0, sizeof(in));
			rand_bytes(in + (in_len - 10), 10);
			TEST_ASSERT(cp_phpe_enc(out, &out_len, in, in_len, n) == RLC_OK,
					end);
			TEST_ASSERT(cp_phpe_dec(out, in_len, out, out_len, n, l) == RLC_OK,
					end);
			TEST_ASSERT(memcmp(in, out, in_len) == 0, end);
		}
		TEST_END;

		TEST_BEGIN("paillier encryption/decryption is homomorphic") {
			TEST_ASSERT(result == RLC_OK, end);
			in_len = bn_size_bin(n);
			out_len = RLC_BN_BITS / 8 + 1;
			memset(in, 0, sizeof(in));
			rand_bytes(in + (in_len - 10), 10);
			bn_read_bin(a, in, in_len);
			TEST_ASSERT(cp_phpe_enc(out, &out_len, in, in_len, n) == RLC_OK,
					end);
			bn_read_bin(b, out, out_len);
			memset(in, 0, sizeof(in));
			rand_bytes(in + (in_len - 10), 10);
			bn_read_bin(c, in, in_len);
			out_len = RLC_BN_BITS / 8 + 1;
			TEST_ASSERT(cp_phpe_enc(out, &out_len, in, in_len, n) == RLC_OK,
					end);
			bn_read_bin(d, out, out_len);
			bn_mul(b, b, d);
			bn_sqr(s, n);
			bn_mod(b, b, s);
			bn_write_bin(out, out_len, b);
			TEST_ASSERT(cp_phpe_dec(out, in_len, out, out_len, n, l) == RLC_OK,
					end);
			bn_add(a, a, c);
			bn_write_bin(in, in_len, a);
			bn_read_bin(a, out, in_len);
			TEST_ASSERT(memcmp(in, out, in_len) == 0, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(a);
	bn_free(b);
	bn_free(c);
	bn_free(d);
	bn_free(n);
	bn_free(l);
	bn_free(s);
	return code;
}

#if defined(WITH_EC)

#if defined(EP_PLAIN) && FP_PRIME == 160

/* Test vectors taken from SECG GEC 2. */

#define SECG_P160_A		"AA374FFC3CE144E6B073307972CB6D57B2A4E982"
#define SECG_P160_B		"45FB58A92A17AD4B15101C66E74F277E2B460866"
#define SECG_P160_A_X	"51B4496FECC406ED0E75A24A3C03206251419DC0"
#define SECG_P160_A_Y	"C28DCB4B73A514B468D793894F381CCC1756AA6C"
#define SECG_P160_B_X	"49B41E0E9C0369C2328739D90F63D56707C6E5BC"
#define SECG_P160_B_Y	"26E008B567015ED96D232A03111C3EDC0E9C8F83"

uint8_t resultp[] = {
	0x74, 0x4A, 0xB7, 0x03, 0xF5, 0xBC, 0x08, 0x2E, 0x59, 0x18, 0x5F, 0x6D,
	0x04, 0x9D, 0x2D, 0x36, 0x7D, 0xB2, 0x45, 0xC2
};

#endif

#if defined(EB_KBLTZ) && FB_POLYN == 163

/* Test vectors taken from SECG GEC 2. */

#define NIST_K163_A		"3A41434AA99C2EF40C8495B2ED9739CB2155A1E0D"
#define NIST_K163_B		"057E8A78E842BF4ACD5C315AA0569DB1703541D96"
#define NIST_K163_A_X	"37D529FA37E42195F10111127FFB2BB38644806BC"
#define NIST_K163_A_Y	"447026EEE8B34157F3EB51BE5185D2BE0249ED776"
#define NIST_K163_B_X	"72783FAAB9549002B4F13140B88132D1C75B3886C"
#define NIST_K163_B_Y	"5A976794EA79A4DE26E2E19418F097942C08641C7"

uint8_t resultk[] = {
	0x59, 0x79, 0x85, 0x28, 0x08, 0x3F, 0x50, 0xB0, 0x75, 0x28, 0x35, 0x3C,
	0xDA, 0x99, 0xD0, 0xE4, 0x60, 0xA7, 0x22, 0x9D
};

#endif

/* Test vectors generated by BouncyCastle. */

#if defined(EP_PLAIN) && FP_PRIME == 256

#define NIST_P256_A		"DA818E65859F3997D4CD287945363B14A0030665B8ABD19719D57952E3A2BEAD"
#define NIST_P256_B		"66BF67EDF1ABDC8178C8A07644FDD5C88EFD4954FD6D2691933B5F0EA0AE2153"
#define NIST_P256_A_X	"9A2E9583CCBDD502933709D3ED1764E79D1C2EE601DF75A40C486BE3DAB3CDCA"
#define NIST_P256_A_Y	"D025EA9D9BDA94C0DC7F3813ECA72B369F52CA87E92948BCD76984F44D319F8F"
#define NIST_P256_B_X	"B8F245FC8A1C7E933D5CAD6E77102C72B0C1F393F779F3F504DA1CA776434B10"
#define NIST_P256_B_Y	"5373FA01BC13FF5843D4A31E40833785C598C0BBC2F6AF7317C327BE09883799"

uint8_t result[] = {
	0xC0, 0xEC, 0x2B, 0xAC, 0xEB, 0x3C, 0x6E, 0xE3, 0x21, 0x96, 0xD5, 0x43,
	0x0E, 0xE6, 0xDA, 0xBB, 0x50, 0xAE, 0xEE, 0xBE, 0xBA, 0xCE, 0x6B, 0x86,
	0x09, 0xD7, 0xEB, 0x07, 0xD6, 0x45, 0xF6, 0x34, 0xD4, 0xE0, 0xD1, 0x9A,
	0xAB, 0xA0, 0xD2, 0x90, 0x2F, 0x4A, 0xDC, 0x20, 0x1B, 0x0F, 0x35, 0x8D
};

#endif

#define ASSIGNP(CURVE)														\
	RLC_GET(str, CURVE##_A, sizeof(CURVE##_A));								\
	bn_read_str(da, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_A_X, sizeof(CURVE##_A_X));							\
	fp_read_str(qa->x, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_A_Y, sizeof(CURVE##_A_Y));							\
	fp_read_str(qa->y, str, strlen(str), 16);									\
	fp_set_dig(qa->z, 1);													\
	RLC_GET(str, CURVE##_B, sizeof(CURVE##_B));								\
	bn_read_str(d_b, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_B_X, sizeof(CURVE##_B_X));							\
	fp_read_str(q_b->x, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_B_Y, sizeof(CURVE##_B_Y));							\
	fp_read_str(q_b->y, str, strlen(str), 16);									\
	fp_set_dig(q_b->z, 1);													\
	qa->norm = q_b->norm = 1;												\

#define ASSIGNK(CURVE)														\
	RLC_GET(str, CURVE##_A, sizeof(CURVE##_A));								\
	bn_read_str(da, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_A_X, sizeof(CURVE##_A_X));							\
	fb_read_str(qa->x, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_A_Y, sizeof(CURVE##_A_Y));							\
	fb_read_str(qa->y, str, strlen(str), 16);									\
	fb_set_dig(qa->z, 1);													\
	RLC_GET(str, CURVE##_B, sizeof(CURVE##_B));								\
	bn_read_str(d_b, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_B_X, sizeof(CURVE##_B_X));							\
	fb_read_str(q_b->x, str, strlen(str), 16);									\
	RLC_GET(str, CURVE##_B_Y, sizeof(CURVE##_B_Y));							\
	fb_read_str(q_b->y, str, strlen(str), 16);									\
	fb_set_dig(q_b->z, 1);													\
	qa->norm = q_b->norm = 1;												\

static int ecdh(void) {
	int code = RLC_ERR;
	char str[2 * FC_BYTES + 1];
	bn_t da, d_b;
	ec_t qa, q_b;
	uint8_t key[RLC_MD_LEN], k1[RLC_MD_LEN], k2[RLC_MD_LEN];

	bn_null(da);
	bn_null(d_b);
	ec_null(qa);
	ec_null(q_b);

	TRY {
		bn_new(da);
		bn_new(d_b);
		ec_new(qa);
		ec_new(q_b);

		TEST_BEGIN("ecdh key agreement is correct") {
			TEST_ASSERT(cp_ecdh_gen(da, qa) == RLC_OK, end);
			TEST_ASSERT(cp_ecdh_gen(d_b, q_b) == RLC_OK, end);
			TEST_ASSERT(cp_ecdh_key(k1, RLC_MD_LEN, d_b, qa) == RLC_OK, end);
			TEST_ASSERT(cp_ecdh_key(k2, RLC_MD_LEN, da, q_b) == RLC_OK, end);
			TEST_ASSERT(memcmp(k1, k2, RLC_MD_LEN) == 0, end);
		} TEST_END;

#if MD_MAP == SHONE

		switch (ec_param_get()) {

#if EC_CUR == PRIME

#if defined(EP_PLAIN) && FP_PRIME == 160
			case SECG_P160:
				ASSIGNP(SECG_P160);
				memcpy(key, resultp, RLC_MD_LEN);
				break;
#endif

#else /* EC_CUR == CHAR2 */

#if defined(EB_KBLTZ) && FB_POLYN == 163
			case NIST_K163:
				ASSIGNK(NIST_K163);
				memcpy(key, resultk, RLC_MD_LEN);
				break;
#endif

#endif
			default:
				code = RLC_OK;
				break;
		}

		if (code != RLC_OK) {
			TEST_ONCE("ecdh satisfies test vectors") {
				TEST_ASSERT(ec_is_valid(qa) == 1, end);
				TEST_ASSERT(ec_is_valid(q_b) == 1, end);
				TEST_ASSERT(cp_ecdh_key(k1, RLC_MD_LEN, d_b, qa) == RLC_OK, end);
				TEST_ASSERT(cp_ecdh_key(k2, RLC_MD_LEN, da, q_b) == RLC_OK, end);
				TEST_ASSERT(memcmp(k1, key, RLC_MD_LEN) == 0, end);
				TEST_ASSERT(memcmp(k2, key, RLC_MD_LEN) == 0, end);
			}
			TEST_END;
		}
#endif
		(void)str;
		(void)key;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(da);
	bn_free(d_b);
	ec_free(qa);
	ec_free(q_b);
	return code;
}

static int ecmqv(void) {
	int code = RLC_ERR;
	bn_t d1a, d1_b;
	bn_t d2a, d2_b;
	ec_t q1a, q1_b;
	ec_t q2a, q2_b;
	uint8_t key1[RLC_MD_LEN], key2[RLC_MD_LEN];

	bn_null(d1a);
	bn_null(d1_b);
	ec_null(q1a);
	ec_null(q1_b);
	bn_null(d2a);
	bn_null(d2_b);
	ec_null(q2a);
	ec_null(q2_b);

	TRY {
		bn_new(d1a);
		bn_new(d1_b);
		ec_new(q1a);
		ec_new(q1_b);
		bn_new(d2a);
		bn_new(d2_b);
		ec_new(q2a);
		ec_new(q2_b);

		TEST_BEGIN("ecmqv authenticated key agreement is correct") {
			TEST_ASSERT(cp_ecmqv_gen(d1a, q1a) == RLC_OK, end);
			TEST_ASSERT(cp_ecmqv_gen(d2a, q2a) == RLC_OK, end);
			TEST_ASSERT(cp_ecmqv_gen(d1_b, q1_b) == RLC_OK, end);
			TEST_ASSERT(cp_ecmqv_gen(d2_b, q2_b) == RLC_OK, end);
			TEST_ASSERT(cp_ecmqv_key(key1, RLC_MD_LEN, d1_b, d2_b, q2_b, q1a,
							q2a) == RLC_OK, end);
			TEST_ASSERT(cp_ecmqv_key(key2, RLC_MD_LEN, d1a, d2a, q2a, q1_b,
							q2_b) == RLC_OK, end);
			TEST_ASSERT(memcmp(key1, key2, RLC_MD_LEN) == 0, end);
		} TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d1a);
	bn_free(d1_b);
	ec_free(q1a);
	ec_free(q1_b);
	bn_free(d2a);
	bn_free(d2_b);
	ec_free(q2a);
	ec_free(q2_b);
	return code;
}

static int ecies(void) {
	int code = RLC_ERR;
	ec_t r;
	bn_t da, d_b;
	ec_t qa, q_b;
	int l, in_len, out_len;
	uint8_t in[RLC_BC_LEN - 1], out[RLC_BC_LEN + RLC_MD_LEN];

	ec_null(r);
	bn_null(da);
	bn_null(d_b);
	ec_null(qa);
	ec_null(q_b);

	TRY {
		ec_new(r);
		bn_new(da);
		bn_new(d_b);
		ec_new(qa);
		ec_new(q_b);

		l = ec_param_level();
		if (l == 128 || l == 192 || l == 256) {
			TEST_BEGIN("ecies encryption/decryption is correct") {
				TEST_ASSERT(cp_ecies_gen(da, qa) == RLC_OK, end);
				in_len = RLC_BC_LEN - 1;
				out_len = RLC_BC_LEN + RLC_MD_LEN;
				rand_bytes(in, in_len);
				TEST_ASSERT(cp_ecies_enc(r, out, &out_len, in, in_len, qa)
						== RLC_OK, end);
				TEST_ASSERT(cp_ecies_dec(out, &out_len, r, out, out_len, da)
						== RLC_OK, end);
				TEST_ASSERT(memcmp(in, out, out_len) == 0, end);
			}
			TEST_END;
		}
#if MD_MAP == SH256
		uint8_t msg[RLC_BC_LEN + RLC_MD_LEN];
		char str[2 * FC_BYTES + 1];

		switch (ec_param_get()) {

#if defined(EP_PLAIN) && FP_PRIME == 256
			case NIST_P256:
				ASSIGNP(NIST_P256);
				memcpy(msg, result, sizeof(result));
				break;
#endif
			default:
				(void)str;
				code = RLC_OK;
				break;
		}

		if (code != RLC_OK) {

			TEST_ONCE("ecies satisfies test vectors") {
				uint8_t in[] = {
					0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF
				};
				TEST_ASSERT(ec_is_valid(qa) == 1, end);
				TEST_ASSERT(ec_is_valid(q_b) == 1, end);
				out_len = 16;
				TEST_ASSERT(cp_ecies_dec(out, &out_len, q_b, msg, sizeof(msg),
								da) == RLC_OK, end);
				TEST_ASSERT(out_len == sizeof(in), end);
				TEST_ASSERT(memcmp(out, in, sizeof(in)) == RLC_OK, end);
				out_len = 16;
				TEST_ASSERT(cp_ecies_dec(out, &out_len, qa, msg, sizeof(msg),
								d_b) == RLC_OK, end);
				TEST_ASSERT(out_len == sizeof(in), end);
				TEST_ASSERT(memcmp(out, in, sizeof(in)) == RLC_OK, end);
			}
			TEST_END;
		}
#endif
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	ec_free(r);
	bn_free(da);
	bn_free(d_b);
	ec_free(qa);
	ec_free(q_b);
	return code;
}

static int ecdsa(void) {
	int code = RLC_ERR;
	bn_t d, r, s;
	ec_t q;
	uint8_t m[5] = { 0, 1, 2, 3, 4 }, h[RLC_MD_LEN];

	bn_null(d);
	bn_null(r);
	bn_null(s);
	ec_null(q);

	TRY {
		bn_new(d);
		bn_new(r);
		bn_new(s);
		ec_new(q);

		TEST_BEGIN("ecdsa signature is correct") {
			TEST_ASSERT(cp_ecdsa_gen(d, q) == RLC_OK, end);
			TEST_ASSERT(cp_ecdsa_sig(r, s, m, sizeof(m), 0, d) == RLC_OK, end);
			TEST_ASSERT(cp_ecdsa_ver(r, s, m, sizeof(m), 0, q) == 1, end);
			md_map(h, m, sizeof(m));
			TEST_ASSERT(cp_ecdsa_sig(r, s, h, RLC_MD_LEN, 1, d) == RLC_OK, end);
			TEST_ASSERT(cp_ecdsa_ver(r, s, h, RLC_MD_LEN, 1, q) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d);
	bn_free(r);
	bn_free(s);
	ec_free(q);
	return code;
}

static int ecss(void) {
	int code = RLC_ERR;
	bn_t d, r;
	ec_t q;
	uint8_t m[5] = { 0, 1, 2, 3, 4 };

	bn_null(d);
	bn_null(r);
	ec_null(q);

	TRY {
		bn_new(d);
		bn_new(r);
		ec_new(q);

		TEST_BEGIN("ecss signature is correct") {
			TEST_ASSERT(cp_ecss_gen(d, q) == RLC_OK, end);
			TEST_ASSERT(cp_ecss_sig(r, d, m, sizeof(m), d) == RLC_OK, end);
			TEST_ASSERT(cp_ecss_ver(r, d, m, sizeof(m), q) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d);
	bn_free(r);
	ec_free(q);
	return code;
}

static int vbnn(void) {
	int code = RLC_ERR;
	uint8_t ida[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	uint8_t idb[] = { 5, 6, 7, 8, 9, 0, 1, 2, 3, 4 };
	vbnn_user_t a;
	vbnn_user_t b;
	vbnn_kgc_t s;

	uint8_t m[] = "Thrice the brinded cat hath mew'd.";

	ec_t r;
	bn_t z;
	bn_t h;

	ec_null(r);
	bn_null(z);
	bn_null(h);

	TRY {
		vbnn_kgc_new(s);

		vbnn_user_new(a);
		vbnn_user_new(b);

		ec_new(r);
		bn_new(z);
		bn_new(h);

		TEST_BEGIN("vbnn is correct") {
			TEST_ASSERT(cp_vbnn_gen(s) == RLC_OK, end);
			TEST_ASSERT(cp_vbnn_gen_prv(a, s, ida, sizeof(ida)) == RLC_OK, end);
			TEST_ASSERT(cp_vbnn_gen_prv(b, s, idb, sizeof(idb)) == RLC_OK, end);
			TEST_ASSERT(cp_vbnn_sig(r, z, h, ida, sizeof(ida), m, sizeof(m), a) == RLC_OK, end);
			TEST_ASSERT(cp_vbnn_ver(r, z, h, ida, sizeof(ida), m, sizeof(m), s->mpk) == 1, end);
			TEST_ASSERT(cp_vbnn_ver(r, z, h, idb, sizeof(idb), m, sizeof(m), s->mpk) == 0, end);
			TEST_ASSERT(cp_vbnn_sig(r, z, h, ida, sizeof(ida), m, sizeof(m), b) == RLC_OK, end);
			TEST_ASSERT(cp_vbnn_ver(r, z, h, ida, sizeof(ida), m, sizeof(m), s->mpk) == 0, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

end:
	ec_free(r);
	bn_free(z);
	bn_free(h);

	vbnn_kgc_free(s);
	vbnn_user_free(a);
	vbnn_user_free(b);
	return code;
}

#endif

#if defined(WITH_PC)

static int sokaka(void) {
	int code = RLC_ERR, l = RLC_MD_LEN;
	sokaka_t k;
	bn_t s;
	uint8_t k1[RLC_MD_LEN], k2[RLC_MD_LEN];
	char ia[5] = { 'A', 'l', 'i', 'c', 'e' };
	char ib[3] = { 'B', 'o', 'b' };

	sokaka_null(k);
	bn_null(s);

	TRY {
		sokaka_new(k);
		bn_new(s);

		cp_sokaka_gen(s);

		TEST_BEGIN
				("sakai-ohgishi-kasahara authenticated key agreement is correct")
		{
			TEST_ASSERT(cp_sokaka_gen_prv(k, ia, 5, s) == RLC_OK, end);
			TEST_ASSERT(cp_sokaka_key(k1, l, ia, 5, k, ib, 3) == RLC_OK, end);
			TEST_ASSERT(cp_sokaka_gen_prv(k, ib, 3, s) == RLC_OK, end);
			TEST_ASSERT(cp_sokaka_key(k2, l, ib, 3, k, ia, 5) == RLC_OK, end);
			TEST_ASSERT(memcmp(k1, k2, l) == 0, end);
		} TEST_END;

	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	sokaka_free(k);
	bn_free(s);
	return code;
}

static int ibe(void) {
	int code = RLC_ERR;
	bn_t s;
	g1_t pub;
	g2_t prv;
	uint8_t in[10], out[10 + 2 * RLC_FP_BYTES + 1];
	char id[5] = { 'A', 'l', 'i', 'c', 'e' };
	int il, ol;
	int result;

	bn_null(s);
	g1_null(pub);
	g2_null(prv);

	TRY {
		bn_new(s);
		g1_new(pub);
		g2_new(prv);

		result = cp_ibe_gen(s, pub);

		TEST_BEGIN("boneh-franklin identity-based encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);
			il = ol = 10;
			ol += 1 + 2 * RLC_FP_BYTES;
			rand_bytes(in, il);
			TEST_ASSERT(cp_ibe_gen_prv(prv, id, 5, s) == RLC_OK, end);
			TEST_ASSERT(cp_ibe_enc(out, &ol, in, il, id, 5, pub) == RLC_OK, end);
			TEST_ASSERT(cp_ibe_dec(out, &il, out, ol, prv) == RLC_OK, end);
			TEST_ASSERT(memcmp(in, out, il) == 0, end);
		} TEST_END;
	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(s);
	g1_free(pub);
	g2_free(prv);
	return code;
}

static int bgn(void) {
	int result, code = RLC_ERR;
	g1_t c[2], d[2];
	g2_t e[2], f[2];
	gt_t g[4];
	bgn_t pub, prv;
	dig_t in, out, t;

	g1_null(c[0]);
	g1_null(c[1]);
	g1_null(d[0]);
	g1_null(d[1]);
	g2_null(e[0]);
	g2_null(e[1]);
	g2_null(f[0]);
	g2_null(f[1]);
	bgn_null(pub);
	bgn_null(prv);

	TRY {
		g1_new(c[0]);
		g1_new(c[1]);
		g1_new(d[0]);
		g1_new(d[1]);
		g2_new(e[0]);
		g2_new(e[1]);
		g2_new(f[0]);
		g2_new(f[1]);
		bgn_new(pub);
		bgn_new(prv);
		for (int i = 0; i < 4; i++) {
			gt_null(g[i]);
			gt_new(g[i]);
		}

		result = cp_bgn_gen(pub, prv);

		TEST_BEGIN("boneh-go-nissim encryption/decryption is correct") {
			TEST_ASSERT(result == RLC_OK, end);

			rand_bytes((unsigned char *)&in, sizeof(dig_t));
			in = in % 11;

			TEST_ASSERT(cp_bgn_enc1(c, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_dec1(&out, c, prv) == RLC_OK, end);
			TEST_ASSERT(in == out, end);
			TEST_ASSERT(cp_bgn_enc2(e, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_dec2(&out, e, prv) == RLC_OK, end);
			TEST_ASSERT(in == out, end);
		} TEST_END;

		TEST_BEGIN("boneh-go-nissim encryption is additively homomorphic") {
			rand_bytes((unsigned char *)&in, sizeof(dig_t));
			in = in % 11;
			out = in % 7;
			TEST_ASSERT(cp_bgn_enc1(c, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_enc1(d, out, pub) == RLC_OK, end);
			g1_add(c[0], c[0], d[0]);
			g1_add(c[1], c[1], d[1]);
			g1_norm(c[0], c[0]);
			g1_norm(c[1], c[1]);
			TEST_ASSERT(cp_bgn_dec1(&t, c, prv) == RLC_OK, end);
			TEST_ASSERT(in + out == t, end);
			TEST_ASSERT(cp_bgn_enc2(e, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_enc2(f, out, pub) == RLC_OK, end);
			g2_add(e[0], e[0], f[0]);
			g2_add(e[1], e[1], f[1]);
			g2_norm(e[0], e[0]);
			g2_norm(e[1], e[1]);
			TEST_ASSERT(cp_bgn_dec2(&t, e, prv) == RLC_OK, end);
			TEST_ASSERT(in + out == t, end);
		} TEST_END;

		TEST_BEGIN("boneh-go-nissim encryption is multiplicatively homomorphic") {
			rand_bytes((unsigned char *)&in, sizeof(dig_t));
			in = in % 11;
			out = in % 17;
			TEST_ASSERT(cp_bgn_enc1(c, in, pub) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_enc2(e, out, pub) == RLC_OK, end);
			in = in * out;
			TEST_ASSERT(cp_bgn_mul(g, c, e) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_dec(&t, g, prv) == RLC_OK, end);
			TEST_ASSERT(in == t, end);
			TEST_ASSERT(cp_bgn_add(g, g, g) == RLC_OK, end);
			TEST_ASSERT(cp_bgn_dec(&t, g, prv) == RLC_OK, end);
			TEST_ASSERT(in + in == t, end);
		} TEST_END;

	} CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	g1_free(c[0]);
	g1_free(c[1]);
	g1_free(d[0]);
	g1_free(d[1]);
	g2_free(e[0]);
	g2_free(e[1]);
	g2_free(f[0]);
	g2_free(f[1]);
	bgn_free(pub);
	bgn_free(prv);
	for (int i = 0; i < 4; i++) {
		gt_free(g[i]);
	}
	return code;
}

static int bls(void) {
	int code = RLC_ERR;
	bn_t d;
	g1_t s;
	g2_t q;
	uint8_t m[5] = { 0, 1, 2, 3, 4 };

	bn_null(d);
	g1_null(s);
	g2_null(q);

	TRY {
		bn_new(d);
		g1_new(s);
		g2_new(q);

		TEST_BEGIN("boneh-lynn-schacham short signature is correct") {
			TEST_ASSERT(cp_bls_gen(d, q) == RLC_OK, end);
			TEST_ASSERT(cp_bls_sig(s, m, sizeof(m), d) == RLC_OK, end);
			TEST_ASSERT(cp_bls_ver(s, m, sizeof(m), q) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d);
	g1_free(s);
	g2_free(q);
	return code;
}

static int bbs(void) {
	int code = RLC_ERR;
	bn_t d;
	g1_t s;
	g2_t q;
	gt_t z;
	uint8_t m[5] = { 0, 1, 2, 3, 4 }, h[RLC_MD_LEN];

	bn_null(d);
	g1_null(s);
	g2_null(q);
	gt_null(z);

	TRY {
		bn_new(d);
		g1_new(s);
		g2_new(q);
		gt_new(z);

		TEST_BEGIN("boneh-boyen short signature is correct") {
			TEST_ASSERT(cp_bbs_gen(d, q, z) == RLC_OK, end);
			TEST_ASSERT(cp_bbs_sig(s, m, sizeof(m), 0, d) == RLC_OK, end);
			TEST_ASSERT(cp_bbs_ver(s, m, sizeof(m), 0, q, z) == 1, end);
			md_map(h, m, sizeof(m));
			TEST_ASSERT(cp_bbs_sig(s, m, sizeof(m), 1, d) == RLC_OK, end);
			TEST_ASSERT(cp_bbs_ver(s, m, sizeof(m), 1, q, z) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d);
	g1_free(s);
	g2_free(q);
	gt_free(z);
	return code;
}

static int cls(void) {
	int i, code = RLC_ERR;
	bn_t r, t, u, v, vs[4];
	g1_t a, A, b, B, c, As[4], Bs[4];
	g2_t x, y, z, zs[4];
	uint8_t m[5] = { 0, 1, 2, 3, 4 };
	uint8_t *msgs[5] = {m, m, m, m, m};
	int lens[5] = {sizeof(m), sizeof(m), sizeof(m), sizeof(m), sizeof(m)};

	bn_null(r);
	bn_null(t);
	bn_null(u);
	bn_null(v);
	g1_null(a);
	g1_null(A);
	g1_null(b);
	g1_null(B);
	g1_null(c);
	g2_null(x);
	g2_null(y);
	g2_null(z);
	for (i = 0; i < 4; i++) {
		bn_null(vs[i]);
		g1_null(As[i]);
		g1_null(Bs[i]);
		g2_null(zs[i]);
	}

	TRY {
		bn_new(r);
		bn_new(t);
		bn_new(u);
		bn_new(v);
		g1_new(a);
		g1_new(A);
		g1_new(b);
		g1_new(B);
		g1_new(c);
		g2_new(x);
		g2_new(y);
		g2_new(z);
		for (i = 0; i < 4; i++) {
			bn_new(vs[i]);
			g1_new(As[i]);
			g1_new(Bs[i]);
			g2_new(zs[i]);
		}

		TEST_BEGIN("camenisch-lysyanskaya simple signature is correct") {
			TEST_ASSERT(cp_cls_gen(u, v, x, y) == RLC_OK, end);
			TEST_ASSERT(cp_cls_sig(a, b, c, m, sizeof(m), u, v) == RLC_OK, end);
			TEST_ASSERT(cp_cls_ver(a, b, c, m, sizeof(m), x, y) == 1, end);
			/* Check adversarial signature. */
			g1_set_infty(a);
			g1_set_infty(b);
			g1_set_infty(c);
			TEST_ASSERT(cp_cls_ver(a, b, c, m, sizeof(m), x, y) == 0, end);
		}
		TEST_END;

		TEST_BEGIN("camenisch-lysyanskaya message-independent signature is correct") {
			bn_rand(r, RLC_POS, 2 * pc_param_level());
			TEST_ASSERT(cp_cli_gen(t, u, v, x, y, z) == RLC_OK, end);
			TEST_ASSERT(cp_cli_sig(a, A, b, B, c, m, sizeof(m), r, t, u, v) == RLC_OK, end);
			TEST_ASSERT(cp_cli_ver(a, A, b, B, c, m, sizeof(m), r, x, y, z) == 1, end);
			/* Check adversarial signature. */
			g1_set_infty(a);
			g1_set_infty(A);
			g1_set_infty(b);
			g1_set_infty(B);
			g1_set_infty(c);
			TEST_ASSERT(cp_cli_ver(a, A, b, B, c, m, sizeof(m), r, x, y, z) == 0, end);
		}
		TEST_END;

		TEST_BEGIN("camenisch-lysyanskaya message-block signature is correct") {
			TEST_ASSERT(cp_clb_gen(t, u, vs, x, y, zs, 5) == RLC_OK, end);
			TEST_ASSERT(cp_clb_sig(a, As, b, Bs, c, msgs, lens, t, u, vs, 5) == RLC_OK, end);
			TEST_ASSERT(cp_clb_ver(a, As, b, Bs, c, msgs, lens, x, y, zs, 5) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
  	bn_free(r);
  	bn_free(t);
	bn_free(u);
	bn_free(v);
	g1_free(a);
	g1_free(A);
	g1_free(b);
	g1_free(B);
	g1_free(c);
	g2_free(x);
	g2_free(y);
	g2_free(z);
	for (i = 0; i < 4; i++) {
		bn_free(vs[i]);
		g1_free(As[i]);
		g1_free(Bs[i]);
		g2_free(zs[i]);
	}
  	return code;
}

static int pss(void) {
	int i, code = RLC_ERR;
	bn_t u, v, _v[5];
	g1_t a, b;
	g2_t g, x, y, _y[5];
	uint8_t m[5] = { 0, 1, 2, 3, 4 };
	uint8_t *msgs[5] = {m, m, m, m, m};
	int lens[5] = {sizeof(m), sizeof(m), sizeof(m), sizeof(m), sizeof(m)};

	bn_null(u);
	bn_null(v);
	g1_null(a);
	g1_null(b);
	g2_null(g);
	g2_null(x);
	g2_null(y);
	for (i = 0; i < 5; i++) {
		bn_null(_v[i]);
		g2_null(_y[i]);
	}

	TRY {
		bn_new(u);
		bn_new(v);
		g1_new(a);
		g1_new(b);
		g2_new(g);
		g2_new(x);
		g2_new(y);
		for (i = 0; i < 5; i++) {
			bn_new(_v[i]);
			g2_new(_y[i]);
		}

		TEST_BEGIN("pointcheval-sanders simple signature is correct") {
			TEST_ASSERT(cp_pss_gen(u, v, g, x, y) == RLC_OK, end);
			TEST_ASSERT(cp_pss_sig(a, b, m, sizeof(m), u, v) == RLC_OK, end);
			TEST_ASSERT(cp_pss_ver(a, b, m, sizeof(m), g, x, y) == 1, end);
			/* Check adversarial signature. */
			g1_set_infty(a);
			g1_set_infty(b);
			TEST_ASSERT(cp_pss_ver(a, b, m, sizeof(m), g, x, y) == 0, end);
		}
		TEST_END;

		TEST_BEGIN("pointcheval-sanders message-block signature is correct") {
			TEST_ASSERT(cp_psb_gen(u, _v, g, x, _y, 5) == RLC_OK, end);
			TEST_ASSERT(cp_psb_sig(a, b, msgs, lens, u, _v, 5) == RLC_OK, end);
			TEST_ASSERT(cp_psb_ver(a, b, msgs, lens, g, x, _y, 5) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(u);
	bn_free(v);
	g1_free(a);
	g1_free(b);
	g2_free(g);
	g2_free(x);
	g2_free(y);
	for (i = 0; i < 5; i++) {
		bn_free(_v[i]);
		g2_free(_y[i]);
	}
  	return code;
}

static int zss(void) {
	int code = RLC_ERR;
	bn_t d;
	g1_t q;
	g2_t s;
	gt_t z;
	uint8_t m[5] = { 0, 1, 2, 3, 4 }, h[RLC_MD_LEN];

	bn_null(d);
	g1_null(q);
	g2_null(s);
	gt_null(z);

	TRY {
		bn_new(d);
		g1_new(q);
		g2_new(s);
		gt_new(z);

		TEST_BEGIN("zhang-safavi-naini-susilo signature is correct") {
			TEST_ASSERT(cp_zss_gen(d, q, z) == RLC_OK, end);
			TEST_ASSERT(cp_zss_sig(s, m, sizeof(m), 0, d) == RLC_OK, end);
			TEST_ASSERT(cp_zss_ver(s, m, sizeof(m), 0, q, z) == 1, end);
			md_map(h, m, sizeof(m));
			TEST_ASSERT(cp_zss_sig(s, m, sizeof(m), 1, d) == RLC_OK, end);
			TEST_ASSERT(cp_zss_ver(s, m, sizeof(m), 1, q, z) == 1, end);
		}
		TEST_END;
	}
	CATCH_ANY {
		ERROR(end);
	}
	code = RLC_OK;

  end:
	bn_free(d);
	g1_free(q);
	g2_free(s);
	gt_free(z);
	return code;
}

int main(void) {
	if (core_init() != RLC_OK) {
		core_clean();
		return 1;
	}

	util_banner("Tests for the CP module", 0);

#if defined(WITH_BN)
	util_banner("Protocols based on integer factorization:\n", 0);

	if (rsa() != RLC_OK) {
		core_clean();
		return 1;
	}

	if (rabin() != RLC_OK) {
		core_clean();
		return 1;
	}

	if (benaloh() != RLC_OK) {
		core_clean();
		return 1;
	}

	if (paillier() != RLC_OK) {
		core_clean();
		return 1;
	}

#endif

#if defined(WITH_EC)
	util_banner("Protocols based on elliptic curves:\n", 0);
	if (ec_param_set_any() == RLC_OK) {

		if (ecdh() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (ecmqv() != RLC_OK) {
			core_clean();
			return 1;
		}
#if defined(WITH_BC)
		if (ecies() != RLC_OK) {
			core_clean();
			return 1;
		}
#endif

		if (ecdsa() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (ecss() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (vbnn() != RLC_OK) {
			core_clean();
			return 1;
		}

	} else {
		THROW(ERR_NO_CURVE);
	}
#endif

#if defined(WITH_PC)
	util_banner("Protocols based on pairings:\n", 0);
	if (pc_param_set_any() == RLC_OK) {
		if (sokaka() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (ibe() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (bgn() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (bls() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (bbs() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (cls() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (pss() != RLC_OK) {
			core_clean();
			return 1;
		}

		if (zss() != RLC_OK) {
			core_clean();
			return 1;
		}
	} else {
		THROW(ERR_NO_CURVE);
	}
#endif

	util_banner("All tests have passed.\n", 0);

	core_clean();
	return 0;
}
