#ifndef CONCURRENT_CONSOLE_H
#define CONCURRENT_CONSOLE_H

#include <mutex>
#include <string>

namespace console {
    extern std::mutex m_output;

    void print(std::string value);

    template<typename T>
    extern void printListElement(T value);
}

#include "concurrent_console.cpp"

#endif /* CONCURRENT_CONSOLE_H */
