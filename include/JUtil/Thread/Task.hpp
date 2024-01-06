#ifndef _JUTIL_TASK_HPP_
#define _JUTIL_TASK_HPP_

#include <JUtil/Thread/thread_decl.h>

namespace jutil  {
    struct Task : public NonCopyable {

        virtual ~Task() {}

        virtual void run() = 0;
    };
}

#endif // _JUTIL_TASK_HPP_