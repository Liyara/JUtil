#ifndef JUTIL_THREAD_DECL_H
#define JUTIL_THREAD_DECL_H

#include "JUtil/Core/NonCopyable.h"
#include "JUtil/Core/integers.h"
#include <JUtil/Container/Queue.hpp>
#include <JUtil/Container/Set.hpp>
#include <JUtil/Traits/Traits.hpp>


#include <stdio.h>
#include <stdlib.h>

#ifdef JUTIL_LINUX
    #include <unistd.h>
#endif // JUTIL_LINUX

#ifdef JUTIL_WINDOWS
     #include "windows.h"
#elif defined JUTIL_LINUX
    #include <pthread.h>    // POSIX Threads
#endif // _WIN32

#define THREAD_ERR_INDEX   0x0C

namespace jutil  {

    #ifdef JUTIL_WINDOWS
        typedef void*               ThreadHandle;
        typedef CRITICAL_SECTION    MutexHandle;
        typedef unsigned long       ThreadID;
        typedef CONDITION_VARIABLE  ThreadConditionHandle;
    #elif defined JUTIL_LINUX
        typedef pthread_t       ThreadHandle;
        typedef unsigned long   ThreadID;
        typedef pthread_mutex_t MutexHandle;
        typedef pthread_cond_t  ThreadConditionHandle;
    #endif

    class Condition;
    class Mutex;
    class Task;
    class Thread;
}
#endif // JUTIL_THREAD_DECL_H