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
  // User input parameters.
  unsigned start;
  unsigned threshold;
  unsigned end;
  unsigned am_inc;
  unsigned trials;
  unsigned verbose;
  double gm_inc;
  char prefix[BUFSIZ + 1];
  // OMK meta data.
  char *install_dir;
};

void omk_free_(void **p);
unsigned omk_inc(const struct omk *omk, const unsigned i);
double *omk_create_rand_vec(const unsigned size);
FILE *omk_open_file(const struct omk *omk, const char *suffix);
occa::kernel omk_build_knl(struct omk *omk, const char *name,
                           occa::json &props);

void omk_bench_h2d_d2h_d2d(struct omk *omk);
void omk_bench_reduction(struct omk *omk);

// Host memory allocation function.
#define omk_calloc(T, n) (T *)calloc(n, sizeof(T))

// Host memory free function.
#define omk_free(p) omk_free_((void **)p)

#ifdef __cplusplus
}
#endif

#endif // __OMEM_IMPL_HPP__
