#include "array_operations.hpp"

#include <iostream>
#include <random>
#include <chrono>

#include "concurrent_console.hpp"

void array_operation::findMedian(std::shared_ptr<std::vector<int>> array, int* median) {
    *median = 42;
    // todo write quick select
}

void array_operation::filter(VectorSlice slice, std::function<bool(int)> condition, std::vector<int>* result){
    auto array = *slice.array_ptr;
    for (int i = slice.start; i <= slice.end; i++){
        int element = array[i];
        if (condition(element)){
            result->emplace_back(element);
        }
    }

    console::print("filtered " + std::to_string(slice.start) + "-" + std::to_string(slice.end));
}

std::vector<int> array_operation::merge(std::vector<int> first, std::vector<int> second){
    // todo: write merge function similar to merge sort
    return first;
}

void array_operation::initializeRandom(std::shared_ptr< std::vector<int> > array){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution;

    for (auto& elem : *array){
        elem = distribution(generator);
    }
    console::print("generated");
}

void array_operation::print(const std::vector<int>& array){
    for (auto& elem : array){
        console::printListElement<int>(elem);
    }
    std::cout << std::endl;
}
