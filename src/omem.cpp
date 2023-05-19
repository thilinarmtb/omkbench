#include "omem-impl.hpp"
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

static void print_help() {
  printf("Usage: %s [OPTIONS]\n");
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
  printf("  --help\n");
}

struct omem *omem_init(int argc, char *argv[]) {
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
      {"help", no_argument, 0, 99},
      {0, 0, 0, 0}};

  struct omem *omem = new struct omem();
  omem->start = 1, omem->threshold = 1000, omem->end = 1e6;
  omem->am_inc = 1, omem->gm_inc = 1.03;
  omem->trials = 100, omem->verbose = 0;

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
      omem->start = atoi(optarg);
      break;
    case 31:
      omem->threshold = atoi(optarg);
      break;
    case 32:
      omem->end = atoi(optarg);
      break;
    case 33:
      omem->am_inc = atoi(optarg);
      break;
    case 34:
      omem->gm_inc = atof(optarg);
      break;
    case 40:
      omem->trials = atoi(optarg);
      break;
    case 50:
      omem->verbose = atoi(optarg);
      break;
    case 99:
      print_help();
      exit(EXIT_SUCCESS);
    default:
      print_help();
      exit(EXIT_FAILURE);
      break;
    }
  }

  // Sanity checks.
  if (backend == NULL)
    errx(EXIT_FAILURE, "Backend was not provided. Try `--help`.");

  omem->device.setup(
      {{"mode", std::string(backend)}, {"device_id", device_id}});
  omem_free(&backend);

  return omem;
}

void omem_bench(const char *filename, struct omem *omem) {
  FILE *fp = fopen(filename, "w+");
  if (!fp)
    errx(EXIT_FAILURE, "Unable to open file: \"%s\" for writing.\n", filename);

  double *a = omem_calloc(double, omem->end);
  for (unsigned i = 0; i < omem->end; i++)
    a[i] = (i + 1.0) / (i + 2.0);

  unsigned trials = omem->trials;
  for (unsigned i = omem->start; i < omem->end;) {
    // Allocate memory on the device.
    occa::memory o_a = omem->device.malloc<double>(i);

    // Warmup.
    for (unsigned j = 0; j < trials; j++)
      o_a.copyFrom(a);
    clock_t st = clock();
    for (unsigned j = 0; j < trials; j++)
      o_a.copyFrom(a);
    clock_t et = clock();
    double h2d = (double)(et - st) / (trials * CLOCKS_PER_SEC);

    // Time the data copy.
    for (unsigned j = 0; j < trials; j++)
      o_a.copyTo(a);
    st = clock();
    for (unsigned j = 0; j < trials; j++)
      o_a.copyTo(a);
    et = clock();
    double d2h = (double)(et - st) / (trials * CLOCKS_PER_SEC);

    fprintf(fp, "%d,%e,%e\n", i, h2d, d2h);
    o_a.free();
    if (i < omem->threshold)
      i += omem->am_inc;
    else
      i = (unsigned)(omem->gm_inc * i);
  }
  fclose(fp);

  omem_free(&a);
}

void omem_finalize(struct omem **omem) {
  delete *omem;
  *omem = nullptr;
}
