/**
   miller-rabin-main.c

   Examples of randomized primality testing for hashing applications.

   See man random for additional information on random number generation. 
   Please note that "the GNU C Library does not provide a cryptographic 
   random number generator"
   https://www.gnu.org/software/libc/manual/html_node/Unpredictable-Bytes.html
   
   The implementation provides a "no overflow" guarantee given a number of
   type uint32_t, and preserves the generator-provided uniformity in random 
   processes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "miller-rabin.h"

int main(){
  int primes[18] = {9377, 11939, 19391, 19937, 37199, 39119,
		    71993, 91193, 93719, 93911, 99371, 193939,
		    199933, 319993, 331999, 391939, 393919, 919393};
  int carmichael_nums[30] = {561, 1105, 1729, 2465, 2821,
			     6601, 8911, 10585, 15841, 29341,
			     41041, 46657, 52633, 62745, 63973,
			     75361, 101101, 115921, 126217, 162401,
			     172081, 188461, 252601, 278545, 294409,
			     314821, 334153, 340561, 399001, 410041};
  bool result = true;
  printf("TEST PRIMES: ");
  for (int i = 0; i < 18; i++){
    result *= miller_rabin_prime(primes[i]);
  }
  if (result){
    printf("SUCCESS\n");
  }else{
    printf("FAILURE\n");
  }
  printf("TEST CARMICHAEL NUMBERS: ");
  result = false;
  for (int i = 0; i < 30; i++){
    result += miller_rabin_prime(carmichael_nums[i]);
  }
  if (!result){
    printf("SUCCESS\n");
  }else{
    printf("FAILURE\n");
  }
  return 0;
}


