#include "prng.h"
#include <iostream>


int main(int argc, char* argv[]){
 
  Prng prng(PNG_CHACHA);
  char buf[10];
  prng.get_randomb(buf, 10*8);
  std::cout << buf << "\n";
  prng.get_randomb(buf, 10*8);
  std::cout << buf << "\n";
  return 0;
}
