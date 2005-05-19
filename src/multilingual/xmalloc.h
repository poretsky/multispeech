/* Memory allocation routines prototypes */

#ifndef XMALLOC_H
#define XMALLOC_H 1

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

BEGIN_C_DECLS

void *xrealloc(void *, unsigned int);
void *xmalloc(unsigned int);

END_C_DECLS

#endif
