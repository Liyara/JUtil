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
    particularly <cstdlib>

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


#ifdef _WIN32
    #include "windows.h"
#elif defined __linux__

    #include <termios.h>
    #include <cstdio>
    #include <time.h>
    #include <wchar.h>

    static struct termios _old, _new;

#endif // _WIN32

namespace jutil JUTIL_PUBLIC_ {
    char readCharacter();
    void cls(size_t  = 1);
    void sleep(size_t);

    #ifdef _WIN32
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif // _WIN32

    char readCharacter() {
        char c = 0;
        #ifdef _WIN32
            HANDLE conin = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, JUTIL_NULLPTR, OPEN_EXISTING, 0, JUTIL_NULLPTR);
            DWORD mode, cc;
            GetConsoleMode(conin, &mode);
            SetConsoleMode(conin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
            ReadConsole(conin, &c, 1, &cc, NULL);
            SetConsoleMode(conin, mode);
        #elif defined __linux__
            tcgetattr(0, &_old);
            _new = _old;
            _new.c_lflag &= ~ICANON;
            _new.c_lflag &= ECHO;
            tcsetattr(0, TCSANOW, &_new);
            c = getc(stdin);
            tcsetattr(0, TCSANOW, &_old);
        #endif // _WIN32
        return c;
    }

    void cls(size_t pauseFor) {
        #ifdef _WIN32
            Sleep(pauseFor);
            COORD coord = {0, 0};
            CONSOLE_SCREEN_BUFFER_INFO s;
            GetConsoleScreenBufferInfo(console, &s);
            DWORD written;
            DWORD cells = s.dwSize.X * s.dwSize.Y;
            FillConsoleOutputCharacter(console, ' ', cells, coord, &written);
            FillConsoleOutputAttribute(console, s.wAttributes, cells, coord, &written);
            SetConsoleCursorPosition(console, coord);
        #elif defined __linux__
            sleep(pauseFor);
            if (fwide(stdout, 0) <= 0) fputs("\033[2J\033[1;1H", stdout);
            else fputws(L"\033[2J\033[1;1H", stdout);
        #endif // _WIN32
    }

    void sleep(size_t t) {
        #ifdef _WIN32
            Sleep(t);
        #elif defined __linux__
            timespec ts;
            ts.tv_sec = t / 1000;
            long double r = (t % 1000);
            ts.tv_nsec = (r * (1000 * 1000));
            nanosleep(&ts, NULL);
        #endif // _WIN32
    }
}

#endif // JUTIL_H
