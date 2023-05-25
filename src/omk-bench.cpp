#include "omk-impl.hpp"

void omk_bench_reduction(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "reduction");
  double *x = omk_create_rand_vec(omk->end);
  double *blk = omk_calloc(double, (omk->end + 31) / 32);
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel o_knl = omk_build_knl(omk, "sum", props);

    for (unsigned i = omk->start; i < omk->end;) {
      occa::memory o_x = omk->device.malloc<double>(i);
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_blk = omk->device.malloc<double>(nblks);

      // Warmup.
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }

      // Time the reduction.
      clock_t st = clock();
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }
      clock_t et = clock();
      double t_sum = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

      fprintf(fp, "%u,%s,%u,%e\n", i, "sum", bsize, t_sum);
      o_x.free(), o_blk.free(), i = omk_inc(omk, i);
    }
    o_knl.free();
  }

  omk_free(&x), omk_free(&blk), fclose(fp);
}

void omk_bench_h2d_d2h(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "h2d_d2h");
  double *vec = omk_create_rand_vec(omk->end);

  for (unsigned i = omk->start; i < omk->end;) {
    // Allocate memory on the device.
    occa::memory o_vec = omk->device.malloc<double>(i);

    // Warmup.
    for (unsigned j = 0; j < omk->trials; j++)
      o_vec.copyFrom(vec);
    // Time h2d.
    clock_t st = clock();
    for (unsigned j = 0; j < omk->trials; j++)
      o_vec.copyFrom(vec);
    clock_t et = clock();
    double t_h2d = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

    // Warmup.
    for (unsigned j = 0; j < omk->trials; j++)
      o_vec.copyTo(vec);
    // Time d2h.
    st = clock();
    for (unsigned j = 0; j < omk->trials; j++)
      o_vec.copyTo(vec);
    et = clock();
    double t_d2h = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

    fprintf(fp, "%u,%e,%e\n", i, t_h2d, t_d2h);
    o_vec.free(), i = omk_inc(omk, i);
  }

  omk_free(&vec), fclose(fp);
}
