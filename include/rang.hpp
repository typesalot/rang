#ifndef RANG_DOT_HPP
#define RANG_DOT_HPP

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define OS_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#else
#error Unknown Platform
#endif

#if defined(OS_LINUX) || defined(OS_MAC)
#include <unistd.h>
#include <cstring>

#elif defined(OS_WIN)
#include <windows.h>
#include <io.h>
#include <VersionHelpers.h>

// Only defined in windows 10 onwards, redefining in lower windows since it
// doesn't gets used in lower versions
// https://docs.microsoft.com/en-us/windows/console/getconsolemode
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>

namespace rang {

enum class style {
    reset     = 0,
    bold      = 1,
    dim       = 2,
    italic    = 3,
    underline = 4,
    blink     = 5,
    rblink    = 6,
    reversed  = 7,
    conceal   = 8,
    crossed   = 9
};

enum class fg {
    black   = 30,
    red     = 31,
    green   = 32,
    yellow  = 33,
    blue    = 34,
    magenta = 35,
    cyan    = 36,
    gray    = 37,
    reset   = 39
};

enum class bg {
    black   = 40,
    red     = 41,
    green   = 42,
    yellow  = 43,
    blue    = 44,
    magenta = 45,
    cyan    = 46,
    gray    = 47,
    reset   = 49
};

enum class fgB {
    black   = 90,
    red     = 91,
    green   = 92,
    yellow  = 93,
    blue    = 94,
    magenta = 95,
    cyan    = 96,
    gray    = 97
};

enum class bgB {
    black   = 100,
    red     = 101,
    green   = 102,
    yellow  = 103,
    blue    = 104,
    magenta = 105,
    cyan    = 106,
    gray    = 107
};

enum class control { offColor = 0, autoColor = 1, forceColor = 2 };


namespace rang_implementation {

    inline std::atomic<int> &controlValue() noexcept
    {
        static std::atomic<int> value(1);
        return value;
    }

    inline bool supportsColor() noexcept
    {
#if defined(OS_LINUX) || defined(OS_MAC)

        static const bool result = [] {
            const char *Terms[]
              = { "ansi",    "color",  "console", "cygwin", "gnome",
                  "konsole", "kterm",  "linux",   "msys",   "putty",
                  "rxvt",    "screen", "vt100",   "xterm" };

            const char *env_p = std::getenv("TERM");
            if (env_p == nullptr) {
                return false;
            } else {
                return std::any_of(
                  std::begin(Terms), std::end(Terms), [&](const char *term) {
                      return std::strstr(env_p, term) != nullptr;
                  });
            }
        }();

#elif defined(OS_WIN)
        // GetVersion() is deprecated in newer Visual Studio compilers
        static constexpr bool result = true;
#endif
        return result;
    }


    inline bool isTerminal(const std::streambuf *osbuf) noexcept
    {
        using std::cerr;
        using std::clog;
        using std::cout;

#if defined(OS_LINUX) || defined(OS_MAC)
        auto fn_tty    = isatty;
        auto fn_fileno = fileno;
#elif defined(OS_WIN)
        auto fn_tty    = _isatty;
        auto fn_fileno = _fileno;
#endif
        static const bool result = [&] {
            if (osbuf == cout.rdbuf()) {
                return fn_tty(fn_fileno(stdout)) ? true : false;
            } else if (osbuf == cerr.rdbuf() || osbuf == clog.rdbuf()) {
                return fn_tty(fn_fileno(stderr)) ? true : false;
            } else {
                return false;
            }
        }();
        return result;
    }


    template <typename T>
    using enableStd = typename std::enable_if<
      std::is_same<T, rang::style>::value || std::is_same<T, rang::fg>::value
        || std::is_same<T, rang::bg>::value || std::is_same<T, rang::fgB>::value
        || std::is_same<T, rang::bgB>::value,
      std::ostream &>::type;


#ifdef OS_WIN

    inline HANDLE getConsoleHandle() noexcept
    {
        static const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        return h;
    }

    inline bool setWinTermAnsiColors() noexcept
    {
        HANDLE h = getConsoleHandle();
        if (h == INVALID_HANDLE_VALUE) {
            return false;
        }
        if (IsWindowsVersionOrGreater(10, 0, 0)) {
            DWORD dwMode = 0;
            if (!GetConsoleMode(h, &dwMode)) {
                return false;
            }
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(h, dwMode)) {
                return false;
            }
            return true;
        } else {
            return false;
        }
    }

    inline bool supportsAnsi() noexcept
    {
        static const bool f = setWinTermAnsiColors();
        return f;
    }

    inline WORD defaultState() noexcept
    {
        static const WORD defaultAttributes = []() -> WORD {
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(getConsoleHandle(), &info))
                return (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
            return info.wAttributes;
        }();
        return defaultAttributes;
    }

    inline WORD reverseRGB(WORD rgb) noexcept
    {
        static const WORD rev[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
        return rev[rgb];
    }

    inline void setWinAttribute(rang::bg col, WORD &state) noexcept
    {
        state &= 0xFF0F;
        if (col != rang::bg::reset) {
            state |= reverseRGB(static_cast<WORD>(col) - 40) << 4;
        } else {
            state |= (defaultState() & 0xF0);
        }
    }

    inline void setWinAttribute(rang::fg col, WORD &state) noexcept
    {
        state &= 0xFFF0;
        if (col != rang::fg::reset) {
            state |= reverseRGB(static_cast<WORD>(col) - 30);
        } else {
            state |= (defaultState() & 0x0F);
        }
    }

    inline void setWinAttribute(rang::bgB col, WORD &state) noexcept
    {
        state &= 0xFF0F;
        state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 100)) << 4;
    }

    inline void setWinAttribute(rang::fgB col, WORD &state) noexcept
    {
        state &= 0xFFF0;
        state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 90));
    }

    inline void setWinAttribute(rang::style style, WORD &state) noexcept
    {
        if (style == rang::style::reset) {
            state = defaultState();
        }
    }

    inline WORD &current_state() noexcept
    {
        static WORD state = defaultState();
        return state;
    }

    template <typename T>
    inline enableStd<T> setColor(std::ostream &os, T const value)
    {
        if (supportsAnsi()) {
            return os << "\033[" << static_cast<int>(value) << "m";
        } else {
            const HANDLE h = getConsoleHandle();
            if (h != INVALID_HANDLE_VALUE && isTerminal(os.rdbuf())) {
                setWinAttribute(value, current_state());
                SetConsoleTextAttribute(h, current_state());
            }
            return os;
        }
    }
#else
    template <typename T>
    inline enableStd<T> setColor(std::ostream &os, T const value)
    {
        return os << "\033[" << static_cast<int>(value) << "m";
    }
#endif
}  // namespace rang_implementation

template <typename T>
inline rang_implementation::enableStd<T> operator<<(std::ostream &os,
                                                    const T value)
{
    const int option = rang_implementation::controlValue();
    switch (option) {
        case 0: return os;
        case 1:
            return rang_implementation::supportsColor()
                && rang_implementation::isTerminal(os.rdbuf())
              ? rang_implementation::setColor(os, value)
              : os;
        case 2: return rang_implementation::setColor(os, value);
        default: return os;
    }
}

inline std::ostream &operator<<(std::ostream &os, const rang::control value)
{
    if (value == rang::control::offColor) {
        rang_implementation::controlValue() = 0;
    } else if (value == rang::control::autoColor) {
        rang_implementation::controlValue() = 1;
    } else if (value == rang::control::forceColor) {
        rang_implementation::controlValue() = 2;
    }
    return os;
}
}  // namespace rang

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */
