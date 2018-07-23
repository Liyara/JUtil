#ifndef JUTIL_H
#define JUTIL_H

/**
    @author Jarrett "Liyara" Kastler.
    @version 1.0

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

/**
    @section INCLUDES

    The following JUtil library files are included here,
    either directly or through an include chain:

    Macro.h, IO.h, String.h, List.hpp, Error.h, MemoryBlock.h, Pair <C++11>, Tuple.hpp <C++11>, Traits.hpp <C++11>
*/
#include "Macro.h"
#include "NonCopyable.h"
#include "integers.h"
#include "Container/Queue.hpp"
#include "IO/IO.h"
#ifdef JUTIL_CPP11
    #include "Container/Map.hpp"
    #include "Core/Thread.h"
    #include "Core/RNG.h"
#endif
#include "Container/List.hpp"
#include "Core/Timer.h"

namespace jutil JUTIL_PUBLIC_ {
    char readCharacter();
    void cls(size_t  = 1);
    void sleep(size_t);
}

#endif // JUTIL_H
