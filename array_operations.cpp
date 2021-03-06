#include "array_operations.hpp"

#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include "concurrent_console.hpp"


size_t partition(std::vector<int>& arr, size_t left, size_t right)
{
    int x = arr[right];
    size_t i = left;
    for (size_t j = left; j <= right - 1; j++) {
        if (arr[j] <= x) {
            std::swap(arr[i], arr[j]);
            i++;
        }
    }
    std::swap(arr[i], arr[right]);
    return i;
}

// quickselect algorithm
int kthSmallest(std::vector<int>& array, size_t left, size_t right, size_t search_position)
{
    if (search_position > 0 && search_position <= right - left + 1) {
 
        size_t partition_center = partition(array, left, right);
 
        if (partition_center - left == search_position - 1)
            return array[partition_center];
 
        // recur for left subarray
        if (partition_center - left > search_position - 1)
            return kthSmallest(array, left, partition_center - 1, search_position);
 
        // recur for right subarray
        return kthSmallest(array, partition_center + 1, right, search_position - partition_center + left - 1);
    }
    
    return INT_MAX;
}

void array_operation::findMedian(std::vector<int> *array, int* median) {
    size_t median_index = array->size() % 2 != 0 ? array->size() / 2 : array->size() / 2 + 1;
    *median = kthSmallest(std::vector<int>(*array), 0, array->size() - 1, median_index);
}

void array_operation::filter(VectorSlice slice, std::function<bool(int)> condition, std::vector<int>* result){
    auto array = *slice.array_ptr;
    
    result->reserve(slice.end - slice.start);

    for (size_t i = slice.start; i <= slice.end; i++){
        int element = array[i];
        if (condition(element)){
            result->emplace_back(element);
        }
    }

    // console::print("filtered " + std::to_string(slice.start) + "-" + std::to_string(slice.end));
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

void array_operation::initializeRandom(std::vector<int> *array){
    unsigned seed = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution;

    for (auto& elem : *array){
        elem = distribution(generator);
    }
    // console::print("generated");
}

void array_operation::print(const std::vector<int>& array){
    for (auto& elem : array){
        console::printListElement<int>(elem);
    }
    std::cout << std::endl;
}

std::vector<int> array_operation::filterParallel(ThreadPool *pool, 
                                                std::vector<int> *array_ptr,
                                                std::function<bool(int)> condition, 
                                                size_t part_size)
{
    std::vector< std::vector<int>* > filter_results;
    std::vector< std::future<void> > filter_futures;

    // split array in parts and collect results in another vector
    for (size_t part_start = 0; part_start < array_ptr->size(); part_start += part_size){
        auto part_end = std::min(part_start + part_size - 1, array_ptr->size() - 1);

        VectorSlice slice {array_ptr, part_start, part_end};


        filter_results.emplace_back(new std::vector<int>());

        auto filter_future = pool->addTask(std::bind(array_operation::filter, slice, condition, filter_results.back()));
        filter_futures.emplace_back(std::move(filter_future));
    }


    // merge all filtered parts into one array
    size_t total_size = 0;
    for (size_t i = 0; i < filter_futures.size(); i++) {
        filter_futures[i].get();
        total_size += filter_results[i]->size();
    }

    std::vector<int> filtered;
    filtered.reserve(total_size);
    
    for (auto& part : filter_results){
        if (!part->empty()){
            filtered.insert(filtered.end(), part->begin(), part->end());
        }
        delete part;
    }

    return filtered;
}

void array_operation::sort(std::vector<int> *array){
    std::sort(array->begin(), array->end());
}
