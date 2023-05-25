#if !defined(__OMEM_HPP__)
#define __OMEM_HPP__

#ifdef __cplusplus
extern "C" {
#endif

struct omk;
struct omk *omk_init(int argc, char *argv[]);
void omk_bench(struct omk *omk);
void omk_finalize(struct omk **omk);

#ifdef __cplusplus
}
#endif
#endif // __OMEM_HPP__
