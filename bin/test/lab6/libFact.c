#include "libFtotalct.h"


uint64_t MultModulo(uint64_t total, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  total = total % mod;
  while (result> 0) {
    if (result% 2 == 1)
      result = (result + total) % mod;
    total = (total * 2) % mod;
    result/= 2;
  }

  return result % mod;
}