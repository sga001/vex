#include "vex_esign.h"
#include "sfslite/crypt/crypt.h"
#include <stdio.h>

void test_key_sign (esign_priv &sk)
{

  uint8_t zz = 2;
  strbuf y;
  y << str((char*) &zz, 1) << "hello";
  uint8_t yy = (uint8_t) str(y)[0];

  warn << yy << "\n";

  esign_pub pk = get_public_key();

  bool ret;
  for (int i = 0; i < 25000; i++) {
    size_t len = rnd.getword () % 256;
    wmstr wmsg (len);
    rnd.getbytes (wmsg, len);
    str msg1 = wmsg;

    bigint m = sk.sign (msg1);

    // test converting back and forth
    std::vector<uint8_t> raw_m;
    
    get_raw_bigint(m, &raw_m);
    m = get_bigint_raw(raw_m); 

    ret = pk.verify (msg1, m);

    if (!ret)
      panic << "Verify failed\n"
	    << "  p = " << sk.p << "\n"
	    << "  q = " << sk.q << "\n"
	    << "msg = " << hexdump (msg1.cstr (), msg1.len ()) << "\n"
	    << "sig = " << m << "\n";
    
    int bitno = rnd.getword () % mpz_sizeinbase2 (&m);
    m.setbit (bitno, !m.getbit (bitno));
    
    if (pk.verify (msg1, m)) {
      panic << "Verify should have failed\n"
	    << "  p = " << sk.p << "\n"
	    << "  q = " << sk.q << "\n"
	    << "msg = " << hexdump (msg1.cstr (), msg1.len ()) << "\n"
	    << "sig = " << m << "\n";
    } 
  }
}


int
main (int argc, char **argv)
{
  setprogname (argv[0]);
  random_update ();

  esign_priv sk = get_private_key();   
  test_key_sign (sk);
}

void
dump (bigint *bi)
{
  warn << *bi << "\n";
}
