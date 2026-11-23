#include "Python.h"

extern void PyMarshal_Init(void);
extern void initimp(void);
extern void initgc(void);
extern void init_ast(void);
extern void _PyWarnings_Init(void);
extern void init_weakref(void);
extern void initmath(void);
extern void init_sre(void);
extern void init_codecs(void);
extern void init_struct(void);
extern void initcStringIO(void);
extern void initbinascii(void);
extern void init_md5(void);
extern void init_sha(void);
extern void init_sha256(void);
extern void init_sha512(void);
extern void initoperator(void);
extern void init_collections(void);
extern void inititertools(void);
extern void init_functools(void);
extern void init_locale(void);
extern void initerrno(void);
extern void initposix(void);
extern void inittime(void);
extern void initdatetime(void);
extern void init_random(void);

struct _inittab _PyImport_Inittab[] = {
    {"marshal", PyMarshal_Init},
    {"imp", initimp},
    {"_ast", init_ast},
    {"__main__", 0},
    {"__builtin__", 0},
    {"sys", 0},
    {"exceptions", 0},
    {"gc", initgc},
    {"_warnings", _PyWarnings_Init},
    {"_weakref", init_weakref},
    {"math", initmath},
    {"_sre", init_sre},
    {"_codecs", init_codecs},
    {"_struct", init_struct},
    {"cStringIO", initcStringIO},
    {"binascii", initbinascii},
    {"_md5", init_md5},
    {"_sha", init_sha},
    {"_sha256", init_sha256},
    {"_sha512", init_sha512},
    {"operator", initoperator},
    {"_collections", init_collections},
    {"itertools", inititertools},
    {"_functools", init_functools},
    {"_locale", init_locale},
    {"errno", initerrno},
    {"posix", initposix},
    {"time", inittime},
    {"datetime", initdatetime},
    {"_random", init_random},
    {0, 0}
};
