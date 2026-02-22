#include "core/terminal.hpp"


#ifdef SPD_PLATFORM_LINUX
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "posix_input_parser.hpp"

spd::Terminal::Terminal() {
    tcgetattr(STDIN_FILENO, &m_originalTermios);
}

spd::Terminal::~Terminal() {
    DisableRawMode();
}

void spd::Terminal::EnableRawMode() {
    struct termios raw = m_originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void spd::Terminal::DisableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_originalTermios);
}

spd::TermSize spd::Terminal::GetSize() const {
    TermSize size{};
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        size.cols = 80; size.rows = 24; // Default fallback
    }
    else {
        size.cols = ws.ws_col;
        size.rows = ws.ws_row;
    }
    return size;
}

spd::KeyEvent spd::Terminal::ReadKey() const {
    char c;
    while (read(STDIN_FILENO, &c, 1) != 1);

    if (c == '\x1b') {
        KeyEvent ke = internal::ParseEscapeSequence();
        if (ke.key == Key::Unknown) {
            LOG_E("unknown key pressed\n");
        }

        return ke;
    }

    return internal::MapByteToKey(c);
}

#endif
