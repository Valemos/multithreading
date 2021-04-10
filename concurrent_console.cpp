#include "concurrent_console.hpp"

#include <iostream>


std::mutex console::m_output;

void console::print(std::string value) {
    std::lock_guard<std::mutex> lock(m_output);
    std::cout << value << std::endl;
}

template<typename T>
void console::printListElement(T value){
    std::lock_guard<std::mutex> lock(m_output);
    std::cout << value << ", ";
}
