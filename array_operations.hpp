#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <vector>
#include <functional>
#include <memory>
#include "VectorSlice.hpp"

namespace array_operation{
    void filter(VectorSlice slice, std::function<bool(int)> condition, std::vector<int>* result);
    void findMedian(std::shared_ptr<std::vector<int>> array, int* median);
    std::vector<int> merge(const std::vector<int>& first, const std::vector<int>& second);
    void initializeRandom(std::shared_ptr<std::vector<int>> array);
    void print(const std::vector<int>& array);
}

#endif /* ARRAY_OPERATIONS_H */
