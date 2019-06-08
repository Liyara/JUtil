#ifndef JUTIL_H
#define JUTIL_H

/**
    @author Jarrett "Liyara" Kastler.

    @section DESCRIPTION

    Jutil is a C++ library develped as an alternative to the
    C++ standard library.

    JUtil does not depend on the C++ standard library in declaration
    or implementatiom.

    JUtil does, however, depend on the C standard library,
    particularly <cstdlib>, but only in implementation.

    All of the header files in the JUtil library are
    rigorously commented for ease of use.

    JUtil WILL compile under C++98, but use of
    C++11 or above is strongly recommended.
*/

#include "JUtil/Core/integers.h"

#define JUTIL_ERR_INDEX 0x04

#include "JUtil/Core/NonCopyable.h"
#include "JUtil/Container/Queue.hpp"
#include "JUtil/IO/IO.h"
#ifdef JUTIL_CPP11
    #include "JUtil/Container/Map.hpp"
    #include "JUtil/Core/Thread.h"
    #include "JUtil/Core/RNG.h"
#endif
#include "JUtil/Core/Timer.h"

namespace jutil JUTIL_PUBLIC_ {
    char readCharacter();
    void cls(size_t  = 1);
    void sleep(size_t);
}

#endif // JUTIL_H
