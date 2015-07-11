#ifndef _VEX_ESIGN_H_
#define _VEX_ESIGN_H_

#include "sfslite/esign.h"
#include <vector>
#include <string.h>
#define BASE_16 16
#define ESIGN_E 8

// 684-bit primes p and q
const static char* g_q_hex = "47aef0267ea58fcb6b5a74715833c4fbec55f67c9b6\
aba75c28b23ade545e081a323afaf3447f74a89abdfb306c681b796e859ca42e9a301c5a8\
d19262cb008d455a7f7db4ff5f1c6f2d48732a76ba5cb6d2bec8941";

const static char* g_p_hex = "5eb937ef182a08239e021b9062331d6c7c42081ebb8\
ff8115fdcf6474d39efab3aa81b317bffa92d21d448656e6d786c37f7112812f5df8c5f93\
1e96c4a109a1b0cf6826277d1c4d93fa829a5e3dd13c462fa47ccb1";

// 2048-bit public key = p*p*q
const static char* g_n_hex = "9d06dfa84e50d770613f0a42fa8c19cf202cffe860e"
"2109d6a01493d801f25e44a104787738a771cf06b589cf014820a72669166fead86132680"
"d5ee6a3f9279740d33b42cc82b27321b12a51dd84a9b62fcda6652debacf2ac2c0f08100b"
"c94c4586fa87910aae4593d017115f2fb23731607573284fca79850fa69238096d5cd4c17"
"7ff70c740702c634d0416ac5a37d733720388b6d5ffa18c8cc6eff43bca790f61c550e236"
"8a72b362af1fc86bbb6519d42ae803d10f9b8b353450a585cd8a0d84b66075ac7827a4164"
"55005b604a149c647ff694f33cad8e22ba8260a18f09ed9a54572572c35b9e14214d9a8d1"
"1de59e20a3617d729f327107abf13a1";

static esign_pub get_public_key(void) {
  static const bigint n(g_n_hex, BASE_16);
  return esign_pub (n, ESIGN_E);
}

static esign_priv get_private_key(void) {
  static const bigint p(g_p_hex, BASE_16);
  static const bigint q(g_q_hex, BASE_16);
  return esign_priv(p, q, ESIGN_E);
}

// returns a std::vector<uint8_t> from a bigint
static inline void get_raw_bigint(const bigint &x, std::vector<uint8_t> *buf) {
  str tmp = x.getraw();
  buf->assign((uint8_t*)tmp.cstr(), (uint8_t*)tmp.cstr() + tmp.len());
}

// return a bigint from a std::vector<uint8_t>
static bigint get_bigint_raw(const std::vector<uint8_t> &buf) {
  str tmp((char*)buf.data(), buf.size());
  bigint x;
  x.setraw(tmp);
  return x;
}

//NOTE: You can call esign_keygen(bitsize) if you wish to generate fresh keys

#endif
