#include "rp.h"
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <time.h>
#include <vector>
#include <string.h>

#define MAX_BID 10000
const static uint8_t g_prefix[PREFIX_SIZE] = {0}; // Array with all 0s


std::string print_hex(uint8_t *bytes, int size) {

  char s[size*2+1];
  
  for (int i = 0; i < size; ++i){
    sprintf(s + 2*i, "%02X", bytes[i]);
  }

  return std::string(s);
}


typedef std::vector<std::vector<uint8_t> > checkpoints;

int
find_closest_cp(int num, int cp)
{
  int closest = 0;

  for (int i = 0; i < cp; ++i) {
    if ((MAX_BID/cp) * i <= num)
      closest = i;
    else
      break;
  }
  return closest;
}

void
test_checkpoints(int num, int cp)
{
  for (int i = 0; i < num; ++i) {
    std::vector<uint8_t> seed;
    checkpoints cps;
    std::vector<uint8_t> c;

    seed.resize(SEED_SIZE);
    c.resize(HASH_SIZE);
    cps.reserve(cp);     // cp - 1 checkpoints + seed = cp

  
    vex::setup(HASH_SIZE, g_prefix, seed.data(), c.data());
    cps.push_back(seed);

    for (int j = 0; j < cp-1; ++j) {
      vex::gen_commit(MAX_BID/cp, c.data(), HASH_SIZE, c.data());
      cps.push_back(c);
    }

  // Test

   uint8_t commit[HASH_SIZE];
   uint32_t q = rand() % MAX_BID;
  
   vex::gen_commit(q+1, seed.data(), seed.size(), commit);

   int closest = find_closest_cp(q, cp); 
   int remaining = q - (closest * (MAX_BID/cp));

   std::vector<uint8_t> closest_cp = cps.at(closest);
   
  if (closest == 0) 
      remaining++;

   if (vex::verify_proof((uint32_t) remaining, commit, HASH_SIZE, closest_cp.data(),
    closest_cp.size()) != 1) {
      
      std::cout << "q: " << q << "\n";
      std::cout << "closest: " << closest << "\n";
      std::cout << "remaining: " << remaining << "\n";  

    }
  }
}



int main(int argc, char *argv[])
{
  uint8_t prefix[PREFIX_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 
                                 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 
                                 9, 0, 1, 2};
  uint8_t sprime[HASH_SIZE + PREFIX_SIZE];
  uint8_t s[HASH_SIZE];
  uint8_t c[HASH_SIZE];
  uint8_t p[HASH_SIZE];  
  int num, query, res, eq_res;

  srand(time(NULL));

  std::cout << "\nTest 1\n";

  for (int i = 0; i < 10; ++i) {

    num = rand() % 10000;
    query = rand() % (num+1);

    vex::setup(HASH_SIZE, prefix, sprime, s);
    vex::gen_commit(num, s, HASH_SIZE, c);
    vex::gen_proof(query, num, s, HASH_SIZE, p);
  
    res = vex::verify_proof(query, c, HASH_SIZE, p, HASH_SIZE);
    eq_res = vex::verify_eq_proof(num, c, HASH_SIZE, sprime, HASH_SIZE + PREFIX_SIZE, prefix);
  
   
    if (!(res && eq_res)) {
      std::cout << "Run Details: \n";

      std::cout << "num:\t\t" << num << "\n";
      std::cout << "query:\t\t" << query << "\n";
    
      std::cout << "Prefix:\t\t" << print_hex(prefix, PREFIX_SIZE) << "\n";
      std::cout << "S':\t\t" << print_hex(sprime, HASH_SIZE + PREFIX_SIZE) << "\n";
      std::cout << "S:\t\t" << print_hex(s, HASH_SIZE) << "\n";
      std::cout << "C:\t\t" << print_hex(c, HASH_SIZE) << "\n";
      std::cout << "P:\t\t" << print_hex(p, HASH_SIZE) << "\n";
      std::cout << num << " > " << query << "?\t" << (res? "valid proof": "invalid proof") << "\n";
      std::cout << num << " == " << num << "?\t"  << (eq_res? "valid proof": "invalid proof") << "\n";
    }
  }

  std::cout << "Test 1 done\n";
  std::cout << "Test 2\n";

  for (int i = 0; i < 10; ++i) {

    num = rand() % 10000;
    query = rand() % 10000;

    vex::setup(HASH_SIZE, prefix, sprime, s);
    vex::gen_commit(num, s, HASH_SIZE, c);
    vex::gen_proof(query, num, s, HASH_SIZE, p);
  
    res = vex::verify_proof(query, c, HASH_SIZE, p, HASH_SIZE);
    eq_res = vex::verify_eq_proof(query, c, HASH_SIZE, sprime, HASH_SIZE + PREFIX_SIZE, prefix);
  
   
    if ((res || eq_res) && num < query) {
      std::cout << "Run Details: \n";

      std::cout << "num:\t\t" << num << "\n";
      std::cout << "query:\t\t" << query << "\n";
    
      std::cout << "Prefix:\t\t" << print_hex(prefix, PREFIX_SIZE) << "\n";
      std::cout << "S':\t\t" << print_hex(sprime, HASH_SIZE + PREFIX_SIZE) << "\n";
      std::cout << "S:\t\t" << print_hex(s, HASH_SIZE) << "\n";
      std::cout << "C:\t\t" << print_hex(c, HASH_SIZE) << "\n";
      std::cout << "P:\t\t" << print_hex(p, HASH_SIZE) << "\n";
      std::cout << num << " > " << query << "?\t" << (res? "valid proof": "invalid proof") << "\n";
      std::cout << num << " == " << num << "?\t"  << (eq_res? "valid proof": "invalid proof") << "\n";
    }
  }
  std::cout << "Test 2 done\n";
  std::cout << "Test 3\n";
  
  uint8_t salt[SALT_SIZE];
  
  for (int i = 0; i < 1000; ++i) {

    num = rand() % 10000;
    query = rand() % (num+1);
    
    vex::random_bits(SALT_SIZE*8, salt);
    vex::setup(HASH_SIZE, prefix, sprime, s);
    vex::gen_salted_commit(num, s, HASH_SIZE, salt, SALT_SIZE, c);
    vex::gen_proof(query, num, s, HASH_SIZE, p);
  
    res = vex::verify_salted_proof(query, c, HASH_SIZE, p, HASH_SIZE, salt, SALT_SIZE);
    eq_res = vex::verify_eq_salted_proof(num, c, HASH_SIZE, sprime, HASH_SIZE + PREFIX_SIZE, 
      salt, SALT_SIZE, prefix);
  
    if (!(res && eq_res)) {
      std::cout << "Run Details: \n";

      std::cout << "num:\t\t" << num << "\n";
      std::cout << "query:\t\t" << query << "\n";
    
      std::cout << "Prefix:\t\t" << print_hex(prefix, PREFIX_SIZE) << "\n";
      std::cout << "S':\t\t" << print_hex(sprime, HASH_SIZE + PREFIX_SIZE) << "\n";
      std::cout << "S:\t\t" << print_hex(s, HASH_SIZE) << "\n";
      std::cout << "C:\t\t" << print_hex(c, HASH_SIZE) << "\n";
      std::cout << "P:\t\t" << print_hex(p, HASH_SIZE) << "\n";
      std::cout << num << " > " << query << "?\t" << (res? "valid proof": "invalid proof") << "\n";
      std::cout << num << " == " << num << "?\t"  << (eq_res? "valid proof": "invalid proof") << "\n";
    }
  }
  std::cout << "Test 3 done\n\n";


  std::cout << "Test Checkpoints\n\n";
  test_checkpoints(100, 16);

  return 0;
}
