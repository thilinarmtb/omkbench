#include "omk-impl.hpp"

void omk_bench_scalar_mul_div(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "scalar_mul_div");
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel mul = omk_build_knl(omk, "scalar_mul", props);
    occa::kernel div = omk_build_knl(omk, "scalar_div", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);

      // Warmup.
      double scalar = (rand() + 1) / RAND_MAX;
      for (unsigned t = 0; t < omk->trials; t++) {
        mul(i, o_x, scalar, o_y);
        div(i, o_x, scalar, o_y);
      }

      // Time mul.
      occa::streamTag st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        mul(i, o_x, scalar, o_y);
      occa::streamTag et = omk->device.tagStream();
      double t_mul = omk_time_between(omk, st, et) / omk->trials;

      // Time div.
      st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        div(i, o_x, scalar, o_y);
      et = omk->device.tagStream();
      double t_div = omk_time_between(omk, st, et) / omk->trials;

      fprintf(fp, "%s,%u,%u,%e\n", "div", bsize, i, t_div);
      fprintf(fp, "%s,%u,%u,%e\n", "mul", bsize, i, t_mul);
      o_x.free(), o_y.free();
    }
    mul.free(), div.free();
  }
  fclose(fp);
}

void omk_bench_reduction(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "reduction");
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel glsc3 = omk_build_knl(omk, "glsc3", props);
    occa::kernel sum = omk_build_knl(omk, "sum", props);
    occa::kernel dot = omk_build_knl(omk, "dot", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      unsigned nblks = (i + bsize - 1) / bsize;
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);
      occa::memory o_z = omk_create_device_vec(omk, i);
      occa::memory o_blk = omk_create_device_vec(omk, nblks);

      // Warmup.
      for (unsigned t = 0; t < omk->trials; t++) {
        sum(i, o_x, o_blk);
        dot(i, o_x, o_y, o_blk);
        glsc3(i, o_x, o_y, o_z, o_blk);
      }

      // Time sum.
      occa::streamTag st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        sum(i, o_x, o_blk);
      occa::streamTag et = omk->device.tagStream();
      double t_sum = omk_time_between(omk, st, et) / omk->trials;

      // Time dot.
      st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        dot(i, o_x, o_y, o_blk);
      et = omk->device.tagStream();
      double t_dot = omk_time_between(omk, st, et) / omk->trials;

      // Time glsc3.
      st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        glsc3(i, o_x, o_y, o_z, o_blk);
      et = omk->device.tagStream();
      double t_glsc3 = omk_time_between(omk, st, et) / omk->trials;

      fprintf(fp, "%s,%u,%u,%e\n", "glsc3", bsize, i, t_glsc3);
      fprintf(fp, "%s,%u,%u,%e\n", "sum", bsize, i, t_sum);
      fprintf(fp, "%s,%u,%u,%e\n", "dot", bsize, i, t_dot);
      o_x.free(), o_y.free(), o_z.free(), o_blk.free();
    }

    sum.free(), dot.free(), glsc3.free();
  }

  fclose(fp);
}

void omk_bench_daxpy(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "daxpy");
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel add2s1 = omk_build_knl(omk, "add2s1", props);
    occa::kernel add2s2 = omk_build_knl(omk, "add2s2", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);
      occa::memory o_z = omk_create_device_vec(omk, i);

      double alpha = 1.0;

      // Warmup.
      for (unsigned t = 0; t < omk->trials; t++) {
        add2s1(i, o_x, alpha, o_y, o_z);
        add2s2(i, o_x, alpha, o_y, o_z);
      }

      // Time add2s1.
      occa::streamTag st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        add2s1(i, o_x, alpha, o_y, o_z);
      occa::streamTag et = omk->device.tagStream();
      double t_add2s1 = omk_time_between(omk, st, et) / omk->trials;

      // Time add2s2.
      st = omk->device.tagStream();
      for (unsigned t = 0; t < omk->trials; t++)
        add2s2(i, o_x, alpha, o_y, o_z);
      et = omk->device.tagStream();
      double t_add2s2 = omk_time_between(omk, st, et) / omk->trials;

      fprintf(fp, "%s,%u,%u,%e\n", "add2s1", bsize, i, t_add2s1);
      fprintf(fp, "%s,%u,%u,%e\n", "add2s2", bsize, i, t_add2s2);
      o_x.free(), o_y.free(), o_z.free();
    }
    add2s1.free(), add2s2.free();
  }

  fclose(fp);
}

void omk_bench_d2d(struct omk *omk) {
  FILE *fp = omk_open_file(omk, "d2d");
  occa::json props;

  for (unsigned bsize = 32; bsize <= 512; bsize *= 2) {
    props["defines/BLOCKSIZE"] = bsize;
    occa::kernel d2d = omk_build_knl(omk, "d2d", props);

    for (unsigned i = omk->start; i < omk->end; i = omk_inc(omk, i)) {
      // Allocate memory on the device.
      occa::memory o_x = omk_create_device_vec(omk, i);
      occa::memory o_y = omk_create_device_vec(omk, i);

      // Warmup.
      for (unsigned j = 0; j < omk->trials; j++)
        d2d(i, o_x, o_y);

      // Time d2d.
      occa::streamTag st = omk->device.tagStream();
      for (unsigned j = 0; j < omk->trials; j++)
        d2d(i, o_x, o_y);
      occa::streamTag et = omk->device.tagStream();
      double t_d2d = omk_time_between(omk, st, et) / omk->trials;

      fprintf(fp, "%u,%u,%e\n", bsize, i, t_d2d);
      o_x.free(), o_y.free();
    }
    d2d.free();
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
