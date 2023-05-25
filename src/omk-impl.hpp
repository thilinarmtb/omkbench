#if !defined(__OMEM_IMPL_HPP__)
#define __OMEM_IMPL_HPP__

#include "omk.hpp"
#include <assert.h>
#include <err.h>
#include <occa.hpp>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omk {
  // occa device and json object to store configurations.
  occa::device device;
  occa::json props;
  // User input parameters.
  int start, threshold, end, am_inc, trials, verbose;
  double gm_inc;
};

// Host memory allocation function.
#define omk_calloc(T, n) (T *)calloc(n, sizeof(T))

// Host memory free function.
static inline int omk_free_(void **p) {
  free(*p), *p = NULL;
  return 0;
}

#define omk_free(p) omk_free_((void **)p);

#ifdef __cplusplus
}
#endif
#endif // __OMEM_IMPL_HPP__
