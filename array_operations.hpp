#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <vector>
#include <functional>
#include <memory>
#include "VectorSlice.hpp"
#include "thread_pool/ThreadPool.hpp"

namespace array_operation{
    void filter(VectorSlice slice, std::function<bool(int)> condition, std::vector<int>* result);
    std::vector<int> filterParallel(ThreadPool *pool, std::vector<int> *array_ptr, std::function<bool(int)> condition, int split_size);
    
    void findMedian(std::vector<int> *array, int* median);
    void sort(std::vector<int> *array);
    std::vector<int> merge(const std::vector<int>& first, const std::vector<int>& second);

    void initializeRandom(std::vector<int> *array);
    void print(const std::vector<int>& array);
}

#endif /* ARRAY_OPERATIONS_H */
