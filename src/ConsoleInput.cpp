#include "ConsoleInput.h"
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#include <cstdlib>
#include <windows.h>
namespace console {
bool keyAvailable() { return _kbhit() != 0; }
char readKey() { return static_cast<char>(_getch()); }
void clearScreen() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    std::system("cls");
}
}
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
namespace {
class RawTerminal {
public:
    RawTerminal() {
        if (tcgetattr(STDIN_FILENO, &old_) == 0) {
            termios raw = old_;
            raw.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            active_ = true;
        }
    }
    ~RawTerminal() { if (active_) tcsetattr(STDIN_FILENO, TCSANOW, &old_); }
private:
    termios old_{};
    bool active_{false};
};
RawTerminal terminal;
}
namespace console {
bool keyAvailable() {
    timeval tv{0, 0};
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) > 0;
}
char readKey() {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == 1) return c;
    return '\0';
}
void clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}
}
#endif
