#include "omem.hpp"

int main(int argc, char **argv) {
  struct omem *omem = omem_init(argc, argv);
  omem_bench("memcpy.txt", omem);
  omem_finalize(&omem);

  return 0;
}
