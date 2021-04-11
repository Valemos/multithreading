#ifndef VECTOR_SLICE_H
#define VECTOR_SLICE_H

#include <vector>
#include <memory>

struct VectorSlice{
    std::vector<int>* array_ptr;
    size_t start{0}; 
    size_t end{0};
};

#endif /* VECTOR_SLICE_H */
