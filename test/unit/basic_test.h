#ifndef CAE_TEST_UNIT_BASIC_TEST_H_
#define CAE_TEST_UNIT_BASIC_TEST_H_

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

namespace cl = Catch::Clara;
cl::Parser define_options();

#include <cstdlib>
#include <iostream>

static inline bool VerifyBuffer(char *ptr, size_t size, char nonce) {
  for (size_t i = 0; i < size; ++i) {
    if (ptr[i] != nonce) {
      std::cout << (int)ptr[i] << std::endl;
      return false;
    }
  }
  return true;
}

static inline bool CompareBuffers(char *p1, size_t s1, char *p2, size_t s2,
                                  size_t off) {
  if (s1 != s2) {
    return false;
  }
  for (size_t i = off; i < s1; ++i) {
    if (p1[i] != p2[i]) {
      std::cout << "Mismatch at: " << (int)i << std::endl;
      return false;
    }
  }
  return true;
}

void MainPretest();
void MainPosttest();

#define PAGE_DIVIDE(TEXT)

#endif // CAE_TEST_UNIT_BASIC_TEST_H_
