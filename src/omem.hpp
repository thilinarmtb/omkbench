#if !defined(__OMEM_HPP__)
#define __OMEM_HPP__

#ifdef __cplusplus
extern "C" {
#endif

struct omem;
struct omem *omem_init(int argc, char *argv[]);
void omem_bench(const char *filename, struct omem *omem);
void omem_finalize(struct omem **omem);

#ifdef __cplusplus
}
#endif
#endif // __OMEM_HPP__
