#include "omk.hpp"

int main(int argc, char **argv) {
  struct omk *omk = omk_init(argc, argv);
  omk_bench(omk);
  omk_finalize(&omk);

  return 0;
}
