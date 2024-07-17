#ifndef NU_ERROR_H
#define NU_ERROR_H

#if defined(NU_DEBUG) && defined(NU_STDLIB)
#define NU_ASSERT(x) assert(x)
#else
#define NU_ASSERT(x) (void)(x)
#endif

#define _NU_S(x)      #x
#define _NU_S_(x)     _NU_S(x)
#define _NU_S__LINE__ _NU_S_(__LINE__)

#ifdef NU_DEBUG
#define _NU_CHECK(check, action, file, line) \
    if (!(check))                            \
    {                                        \
        action;                              \
    }
#else
#define _NU_CHECK(check, action, file, line) \
    if (!(check))                            \
    {                                        \
        action;                              \
    }
#endif

#define NU_CHECK(check, action) \
    _NU_CHECK(check, action, __FILE__, _NU_S__LINE__)

#define NU_ERROR_CHECK(error, action) \
    _NU_CHECK(error == NU_ERROR_NONE, action, __FILE__, _NU_S__LINE__)

#define NU_ERROR_ASSERT(error) NU_ASSERT(error == NU_ERROR_NONE)

#endif
