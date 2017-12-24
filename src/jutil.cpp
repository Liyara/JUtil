#include "Core/jutil.h"

#ifdef _WIN32
    #include "windows.h"
#elif defined __linux__

    #include <termios.h>
    #include <cstdio>

    static struct termios _old, _new;

#endif // _WIN32

namespace jutil {
    char readCharacter() {
        char c = 0;
        #ifdef _WIN32
            DWORD mode, cc;
            HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
            GetConsoleMode(h, &mode);
            SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
            ReadConsole(h, &c, 1, &cc, NULL);
            SetConsoleMode(h, mode);
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
}

