#include "omk-impl.hpp"
#include <err.h>
#include <getopt.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

static void print_help(const char *name) {
  printf("Usage: %s [OPTIONS]\n", name);
  printf("Options:\n");
  printf("  --device_id <ID>, Device id, Values: 0, 1, 2, ... \n");
  printf("  --backend <BACKEND>, Backend name, Values: Cuda, OpenCL, etc.\n");
  printf("  --start=<START>, Start array size, Values: 1, 2, ... \n");
  printf("  --threshold=<THRESHOLD>, AM to GM switching threshold, Values: 1, "
         "2, ... \n");
  printf("  --end=<END>, End array size, Values: 1, 2, ... \n");
  printf("  --am_inc=<DISTANCE>, AM distance, Values: 1, 2, ... \n");
  printf("  --gm_inc=<RATIO>, GM ratio, Values: 0.1, 0.15, ... \n");
  printf("  --trials=<TRIALS>, Number of trials, Values: 1, 2, ...\n");
  printf("  --verbose=<VERBOSITY>, Values: 0, 1, 2, ...\n");
  printf("  --prefix=<PREFIX>, Prefix for the result files.\n");
  printf("  --help\n");
}

struct omk *omk_init(int argc, char *argv[]) {
  static struct option long_options[] = {
      {"device_id", required_argument, 0, 10},
      {"backend", required_argument, 0, 20},
      {"start", optional_argument, 0, 30},
      {"threshold", optional_argument, 0, 31},
      {"end", optional_argument, 0, 32},
      {"am_inc", optional_argument, 0, 33},
      {"gm_inc", optional_argument, 0, 34},
      {"trials", optional_argument, 0, 40},
      {"verbose", optional_argument, 0, 50},
      {"prefix", optional_argument, 0, 60},
      {"install_dir", optional_argument, 0, 70},
      {"help", no_argument, 0, 99},
      {0, 0, 0, 0}};

  struct omk *omk = new struct omk();
  omk->start = 1, omk->threshold = 1000, omk->end = 1e6;
  omk->am_inc = 1, omk->gm_inc = 1.03;
  strncpy(omk->prefix, "omk", 8);
  omk->trials = 500, omk->verbose = 0;
  omk->install_dir = NULL;

  // Parse the command line arguments.
  char *backend = NULL;
  int device_id = 0;
  for (;;) {
    int c = getopt_long(argc, argv, "", long_options, NULL);
    if (c == -1)
      break;

    switch (c) {
    case 10:
      device_id = atoi(optarg);
      break;
    case 20:
      backend = strndup(optarg, BUFSIZ);
      break;
    case 30:
      omk->start = atoi(optarg);
      break;
    case 31:
      omk->threshold = atoi(optarg);
      break;
    case 32:
      omk->end = atoi(optarg);
      break;
    case 33:
      omk->am_inc = atoi(optarg);
      break;
    case 34:
      omk->gm_inc = atof(optarg);
      break;
    case 40:
      omk->trials = atoi(optarg);
      break;
    case 50:
      omk->verbose = atoi(optarg);
      break;
    case 60:
      strncpy(omk->prefix, optarg, BUFSIZ);
      break;
    case 70:
      omk->install_dir = strndup(optarg, PATH_MAX);
      break;
    case 99:
      print_help(argv[0]);
      exit(EXIT_SUCCESS);
    default:
      print_help(argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }

  // Sanity checks.
  if (backend == NULL)
    errx(EXIT_FAILURE, "Backend was not provided. Try `--help`.");

  omk->device.setup({{"mode", std::string(backend)}, {"device_id", device_id}});
  omk_free(&backend);

  return omk;
}

void omk_free_(void **p) { free(*p), *p = NULL; }

unsigned omk_inc(const struct omk *omk, const unsigned i) {
  if (i < omk->threshold)
    return i + omk->am_inc;
  else
    return (unsigned)(omk->gm_inc * i);
}

double *omk_create_rand_vec(const unsigned size) {
  double *x = omk_calloc(double, size);
  for (unsigned i = 0; i < size; i++)
    x[i] = (rand() + 1.0) / RAND_MAX;
  return x;
}

FILE *omk_open_file(const struct omk *omk, const char *suffix) {
  char fname[2 * BUFSIZ];
  strncpy(fname, omk->prefix, BUFSIZ);
  strncat(fname, "_", 2);
  strncat(fname, suffix, BUFSIZ);
  strncat(fname, ".txt", 5);

  FILE *fp = fopen(fname, "w+");
  if (!fp)
    errx(EXIT_FAILURE, "Unable to open file: \"%s\" for writing.\n", fname);
  return fp;
}

occa::kernel omk_build_knl(struct omk *omk, const char *name,
                           occa::json &props) {
  if (!omk->install_dir) {
    char *tmp = NULL;
    if ((tmp = getenv("OMK_INSTALL_DIR")))
      omk->install_dir = strndup(tmp, PATH_MAX);
    else
      errx(EXIT_FAILURE, "Unable to find omk install directory.\n");
  }

  char *okl = omk_calloc(char, PATH_MAX + BUFSIZ);
  strncpy(okl, omk->install_dir, PATH_MAX);
  strncat(okl, "/okl/kernels.okl", BUFSIZ);

  occa::kernel knl = omk->device.buildKernel(okl, name, props);

  omk_free(&okl);

  return knl;
}

void omk_bench(struct omk *omk) {
  omk_bench_h2d_d2h(omk);
  omk_bench_reduction(omk);
}

void omk_finalize(struct omk **omk) {
  struct omk *omk_ = *omk;
  if (omk_)
    omk_free(&omk_->install_dir);

  delete omk_;
  *omk = nullptr;
}
