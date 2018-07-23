#include "Core/jutil.h"

#ifdef _WIN32
    #include "windows.h"
#elif defined __linux__

    #include <termios.h>
    #include <cstdio>

    static struct termios _old, _new;

#endif // _WIN32

namespace jutil {

    #ifdef _WIN32
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif // _WIN32

    char readCharacter() {
        char c = 0;
        #ifdef _WIN32
            HANDLE conin = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
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
            sleep(pauseFor);
            COORD coord = {0, 0};
            CONSOLE_SCREEN_BUFFER_INFO s;
            GetConsoleScreenBufferInfo(console, &s);
            DWORD written;
            DWORD cells = s.dwSize.X * s.dwSize.Y;
            FillConsoleOutputCharacter(console, ' ', cells, coord, &written);
            FillConsoleOutputAttribute(console, s.wAttributes, cells, coord, &written);
            SetConsoleCursorPosition(console, coord);
        #elif defined __linux__
            fputs("\033[2J\033[1;1H", stdout);
        #endif // _WIN32
    }

    void sleep(size_t t) {
        #ifdef _WIN32
            Sleep(t);
        #elif defined __linux__
            usleep(t);
        #endif // _WIN32
    }
}

