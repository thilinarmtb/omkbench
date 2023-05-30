#include "omk-impl.hpp"

void omk_bench_glsc3_reduction(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "reduction");
  double *x = omk_create_host_vec(omk->end);
  double *blk = omk_calloc(double, (omk->end + 31) / 32);
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel o_knl = omk_build_knl(omk, "glsc3", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);
      occa::memory o_z = omk_create_device_vec(omk, i);
      occa::memory o_blk = omk_create_device_vec(omk, nblks);

      // Warmup.
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_y, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }

      // Time the reduction.
      clock_t st = clock();
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_y, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }
      clock_t et = clock();
      double t_glsc3 = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

      fprintf(fp, "%s,%u,%u,%e\n", "glsc3", bsize, i, t_glsc3);
      o_x.free(), o_y.free(), o_z.free(), o_blk.free();
    }
    o_knl.free();
  }

  omk_free(&x), omk_free(&blk);
  fclose(fp);
}

void omk_bench_dot_reduction(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "reduction");
  double *x = omk_create_host_vec(omk->end);
  double *blk = omk_calloc(double, (omk->end + 31) / 32);
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel o_knl = omk_build_knl(omk, "dot", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);
      occa::memory o_blk = omk_create_device_vec(omk, nblks);

      // Warmup.
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_y, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }

      // Time the reduction.
      clock_t st = clock();
      for (unsigned i = 0; i < omk->trials; i++) {
        o_knl(i, o_x, o_y, o_blk);
        o_blk.copyTo(blk);
        for (unsigned j = 1; j < nblks; j++)
          blk[0] += blk[j];
      }
      clock_t et = clock();
      double t_dot = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

      fprintf(fp, "%s,%u,%u,%e\n", "dot", bsize, i, t_dot);
      o_x.free(), o_y.free(), o_blk.free();
    }
    o_knl.free();
  }

  omk_free(&x), omk_free(&blk);
  fclose(fp);
}

void omk_bench_sum_reduction(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "reduction");
  double *x = omk_create_host_vec(omk->end);
  double *blk = omk_calloc(double, (omk->end + 31) / 32);
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel o_knl = omk_build_knl(omk, "sum", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_blk = omk_create_device_vec(omk, nblks);

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

      fprintf(fp, "%s,%u,%u,%e\n", "sum", bsize, i, t_sum);
      o_x.free(), o_blk.free();
    }
    o_knl.free();
  }

  omk_free(&x), omk_free(&blk);
  fclose(fp);
}

void omk_bench_d2d(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "d2d");
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel o_knl = omk_build_knl(omk, "d2d", props);
    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      // Allocate memory on the device.
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);

      // Warmup.
      for (unsigned j = 0; j < omk->trials; j++)
        o_knl(i, o_x, o_y);
      // Time d2d.
      occa::streamTag o_st = omk->device.tagStream();
      for (unsigned j = 0; j < omk->trials; j++)
        o_knl(i, o_x, o_y);
      occa::streamTag o_et = omk->device.tagStream();
      double t_d2d = (double)omk->device.timeBetween(o_st, o_et) / omk->trials;

      fprintf(fp, "%u,%u,%e\n", i, bsize, t_d2d);
      o_x.free(), o_y.free();
    }
    o_knl.free();
  }

  fclose(fp);
}

void omk_bench_h2d_d2h(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "h2d_d2h");
  double *x = omk_create_host_vec(omk->end);

  for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
    // Allocate memory on the device.
    occa::memory o_x = omk_create_device_vec(omk, i);

    // Warmup.
    for (unsigned j = 0; j < omk->trials; j++)
      o_x.copyFrom(x);
    // Time h2d.
    clock_t st = clock();
    for (unsigned j = 0; j < omk->trials; j++)
      o_x.copyFrom(x);
    clock_t et = clock();
    double t_h2d = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

    // Warmup.
    for (unsigned j = 0; j < omk->trials; j++)
      o_x.copyTo(x);
    // Time d2h.
    st = clock();
    for (unsigned j = 0; j < omk->trials; j++)
      o_x.copyTo(x);
    et = clock();
    double t_d2h = (double)(et - st) / (omk->trials * CLOCKS_PER_SEC);

    fprintf(fp, "%u,%e,%e\n", i, t_h2d, t_d2h);
    o_x.free();
  }

  omk_free(&x);
  fclose(fp);
}
