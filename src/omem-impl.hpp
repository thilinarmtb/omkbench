#if !defined(__OMEM_IMPL_HPP__)
#define __OMEM_IMPL_HPP__

#include "omem.hpp"
#include <assert.h>
#include <err.h>
#include <occa.hpp>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omem {
  /* occa related */
  occa::device device;
  occa::json props;
  /* User input */
  int device_id;
  int start, end, trials;
  int verbose;
};

/* Memory allocation functions */
#define omem_calloc(T, n) (T *)calloc(n, sizeof(T))
// FiXME: Call a function instead of the inline macro.
#define omem_free(T, p)                                                        \
  {                                                                            \
    T *p_ = *p;                                                                \
    if (p_) {                                                                  \
      free(p_);                                                                \
      p_ = NULL;                                                               \
    }                                                                          \
  }

#ifdef __cplusplus
}
#endif
#endif // __OMEM_IMPL_HPP__
