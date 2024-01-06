# JUtil
JUtil is written to be a replacement for the standard C++ library, built on top the the standard C library. It is not intended as a viable alternative, however, merely as a proof-of-concept project. 

JUtil is now header-only, however, user must add the line "JUTIL_IMPL" to one source file with access to the library.  

If including JUtil headers as needed, rather than using jutil.h, you may need to use "JUTIL_IO_IMPL" and/or "JUTIL_THREAD_IMPL" when including specifically IO.h or Thread.h.  

## Dependencies
The C standard Library  
dirent.h

## Dependees
JML https://github.com/Liyara/jml
