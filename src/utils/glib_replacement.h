#ifndef _GLIB_REPLACEMENT_H
#define _GLIB_REPLACEMENT_H

#include <WinSock2.h>
#include <windows.h>

#define FALSE 0
#define TRUE 1

// fluidsynth_priv.h replacements
#  define FLUID_DECLARE_VLA(_type, _name, _len) \
    _type* _name = _alloca(sizeof(_type) * (_len))

#define fluid_thread_calling_convention WINAPI

#define FLUID_LIKELY(expr) expr
#define FLUID_UNLIKELY(expr) expr

// fluid_sys.h and fluid_sys.c replacements
#define FLUID_IS_BIG_ENDIAN       0

#define FLUID_LE32TOH(x)          (x)
#define FLUID_LE16TOH(x)          (x)

/* Regular mutex */
typedef SRWLOCK fluid_mutex_t;
#define FLUID_MUTEX_INIT          { 0 }
#define fluid_mutex_init(_m)      InitializeSRWLock(&(_m))
#define fluid_mutex_destroy(_m)
#define fluid_mutex_lock(_m)      AcquireSRWLockExclusive(&(_m))
#define fluid_mutex_unlock(_m)    ReleaseSRWLockExclusive(&(_m))


/* Recursive lock capable mutex */
typedef SRWLOCK fluid_rec_mutex_t;
#define fluid_rec_mutex_init(_m)      InitializeSRWLock(&(_m))
#define fluid_rec_mutex_destroy(_m)
#define fluid_rec_mutex_lock(_m)      AcquireSRWLockShared(&(_m))
#define fluid_rec_mutex_unlock(_m)    ReleaseSRWLockShared(&(_m))

/* Dynamically allocated mutex suitable for fluid_cond_t use */
typedef CRITICAL_SECTION fluid_cond_mutex_t;
#define fluid_cond_mutex_init(_m)      InitializeCriticalSection(_m)
#define fluid_cond_mutex_destroy(_m)   DeleteCriticalSection(_m)
#define fluid_cond_mutex_lock(_m)      EnterCriticalSection(_m)
#define fluid_cond_mutex_unlock(_m)    LeaveCriticalSection(_m)


static inline fluid_cond_mutex_t *
new_fluid_cond_mutex(void)
{
    fluid_cond_mutex_t *mutex;
    mutex = malloc(sizeof(fluid_cond_mutex_t));
    fluid_cond_mutex_init(mutex);
    return mutex;
}

static inline void
delete_fluid_cond_mutex(fluid_cond_mutex_t *m)
{
    fluid_cond_mutex_destroy(m);
    free(m);
}

/* Thread condition signaling */
typedef CONDITION_VARIABLE fluid_cond_t;
#define fluid_cond_init(cond)           InitializeConditionVariable(cond)
#define fluid_cond_destroy(cond)
#define fluid_cond_signal(cond)         WakeConditionVariable(cond)
#define fluid_cond_broadcast(cond)      WakeAllConditionVariable(cond)
#define fluid_cond_wait(cond, mutex)    SleepConditionVariableCS(cond, mutex, INFINITE)

static inline fluid_cond_t *
new_fluid_cond(void)
{
    fluid_cond_t *cond;
    cond = malloc(sizeof(fluid_cond_t));
    fluid_cond_init(cond);
    return cond;
}

static inline void
delete_fluid_cond(fluid_cond_t *cond)
{
    fluid_cond_destroy(cond);
    free(cond);
}

/* Thread private data */
typedef DWORD fluid_private_t;
#define fluid_private_init(_priv)                  (_priv = TlsAlloc())
#define fluid_private_free(_priv)                  TlsFree(_priv);
#define fluid_private_get(_priv)                   TlsGetValue(_priv)
#define fluid_private_set(_priv, _data)            TlsSetValue(_priv, _data)


#define fluid_atomic_int_inc(atomic) InterlockedIncrement((atomic))
#define fluid_atomic_int_get(atomic) (*(LONG*)(atomic))
#define fluid_atomic_int_set(atomic, val) InterlockedExchange((atomic), (val))
#define fluid_atomic_int_dec_and_test(_pi) InterlockedDecrement(_pi)

// TODO: Swapped ??
#define fluid_atomic_int_compare_and_exchange(_pi, _old, _new) InterlockedCompareExchange(_pi, _new, _old)
#define fluid_atomic_int_add(_pi, _add) InterlockedAdd(_pi, _add)

typedef DWORD fluid_thread_return_t;
/* static return value for thread functions which requires a return value */
#define FLUID_THREAD_RETURN_VALUE (0)

typedef void* fluid_thread_t;
typedef fluid_thread_return_t(WINAPI *fluid_thread_func_t)(void *data);

#define FLUID_THREAD_ID_NULL            NULL                    /* A NULL "ID" value */
#define fluid_thread_id_t               DWORD               /* Data type for a thread ID */
#define fluid_thread_get_id()           GetCurrentThreadId()         /* Get unique "ID" for current thread */

fluid_thread_t *new_fluid_thread(const char *name, fluid_thread_func_t func, void *data,
    int prio_level, int detach);
void delete_fluid_thread(fluid_thread_t *thread);
void fluid_thread_self_set_prio(int prio_level);
int fluid_thread_join(fluid_thread_t *thread);

// TODO
#define FLUID_FILE_TEST_EXISTS 1
#define FLUID_FILE_TEST_IS_REGULAR 4
#define fluid_file_test(path, flags) (path, flags)

inline double
fluid_utime(void)
{
    FILETIME ft;
    UINT64 time;

    GetSystemTimeAsFileTime(&ft);
    time = ft.dwHighDateTime;
    time <<= 32ULL;
    time |= ft.dwLowDateTime;
    time /= 10; // time is 100ns, convert to us
    time -= 11644473600000000ULL; // Microseconds between Windows epoch (1601/01/01) and Unix

    return time / 1000000.0;
}

#define fluid_atomic_int_exchange_and_add(_pi, _add) (*_pi) += _add

// TODO
inline int
fluid_shell_parse_argv(const char *command_line,
    int *argcp,
    char ***argvp)
{
    return 0;
}

// TODO
inline void fluid_strfreev(char **str_array)
{

}

typedef struct _fluid_stat_buf_t{int st_mtime;} fluid_stat_buf_t;

// TODO
inline int
fluid_stat(const char *filename,
    fluid_stat_buf_t *buf)
{
    buf->st_mtime = 0;
    return 0;
}

fluid_thread_t *
new_fluid_thread(const char *name, fluid_thread_func_t func, void *data, int prio_level, int detach);


inline void fluid_msleep(unsigned int msecs)
{
    Sleep(msecs);
}

inline int fluid_thread_join(fluid_thread_t *thread)
{
    WaitForSingleObject(*thread, INFINITE);
    CloseHandle(*thread);
    return 0;
}

#endif
