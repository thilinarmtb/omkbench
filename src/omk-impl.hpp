#if !defined(__OMK_IMPL_HPP__)
#define __OMK_IMPL_HPP__

#include "omk.hpp"
#include <assert.h>
#include <err.h>
#include <linux/limits.h>
#include <occa.hpp>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct omk {
  // occa device object.
  occa::device device;
  // User input parameters for input space for benchmarking.
  unsigned start, threshold, end, trials;
  unsigned am_inc;
  double gm_inc;
  // Prefix for the result file name.
  char prefix[BUFSIZ + 1];
  // OMK install dir.
  char install_dir[PATH_MAX + 1];
  // Verbose level.
  unsigned verbose;
};

unsigned omk_inc(const struct omk *omk, const unsigned i);

FILE *omk_open_file(const struct omk *omk, const char *suffix);

double *omk_create_host_vec(const unsigned size);

occa::memory omk_create_device_vec(struct omk *omk, const unsigned size);

occa::kernel omk_build_knl(struct omk *omk, const char *name,
                           occa::json &props);

double omk_time_between(struct omk *omk, occa::streamTag &st,
                        occa::streamTag &et);

void omk_bench_h2d_d2h(struct omk *omk);

void omk_bench_d2d(struct omk *omk);

void omk_bench_reduction(struct omk *omk);

void omk_bench_daxpy(struct omk *omk);

// Host memory allocation function.
#define omk_calloc(T, n) (T *)calloc(n, sizeof(T))

void omk_free_(void **p);

// Host memory free function.
#define omk_free(p) omk_free_((void **)p)

#ifdef __cplusplus
}
#endif

#endif // __OMK_IMPL_HPP__
