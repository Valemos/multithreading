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

std::vector<int> array_operation::merge(const std::vector<int>& first, const std::vector<int>& second)
{
    std::vector<int> result;
    result.reserve(first.size() + second.size());

    int i1 = 0, i2 = 0;
    while (i1 < first.size() && i2 < second.size()) {
        if (first[i1] <= second[i2]) {
            result.emplace_back(first[i1++]);
        } else {
            result.emplace_back(second[i2++]);
        }
    }

    // copy remaining elements of bigger subarray
    if (i1 < first.size()){
        result.insert(result.end(), first.begin() + i1, first.end());
    }else if (i2 < second.size()) {
        result.insert(result.end(), second.begin() + i2, second.end());
    }

    return result;
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
