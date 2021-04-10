
/*
Завдання:
26) Створити 3 масиви (або колекції) з випадковими числами. 
У першому масиві - залишити елементи які менші медіани масиву, 
в другому залишити елементи кратні 2.
Третій масив не змінювати. 
Відсортувати масиви за спаданням і злити в один відсортований масив.
*/


#include <vector>
#include <iostream>
#include <memory>
#include "thread_pool/ThreadPool.hpp"
#include "array_operations.hpp"

#define ARRAY_SIZE 10
#define MAX_PART_SIZE 1


std::vector<int> filterArrayParallel(std::shared_ptr<ThreadPool> pool, 
                                    std::shared_ptr< std::vector<int> > array_ptr,
                                    std::function<bool(int)> condition)
{
    std::vector< std::vector<int> > filtered_parts(array_ptr->size() / MAX_PART_SIZE);

    auto array_filter_tasks = pool->createTaskGroup();
    for (size_t part_start = 0; part_start < array_ptr->size(); part_start += MAX_PART_SIZE){
        auto part_end = std::min(part_start + MAX_PART_SIZE - 1, array_ptr->size() - 1);

        VectorSlice slice {array_ptr, part_start, part_end};

        std::vector<int> filter_result;
        auto task = std::bind(array_operation::filter, slice, condition, &filter_result);
        array_filter_tasks.addTask(task);
        filtered_parts.emplace_back(std::move(filter_result));
    }
    array_filter_tasks.waitTasksFinished();

    size_t total_size = 0;
    for (auto& part : filtered_parts){
        total_size += part.size();
    }
    std::vector<int> filtered;
    filtered.reserve(total_size);

    for (auto& part : filtered_parts){
        if (!part.empty()){
            filtered.insert(filtered.end(), part.begin(), part.end());
        }
    }

    return filtered;
}


void transformMedianArray(std::shared_ptr<ThreadPool> pool, std::shared_ptr< std::vector<int> > array_ptr){

    int median_index = 0;
    auto future_median = pool->addTask(std::bind(array_operation::findMedian, array_ptr, &median_index));
    future_median.get();

    int median_element = (*array_ptr)[median_index];
    std::vector<int> filtered_array;

    std::function<bool(int)> isLessMedian = [median_element] (int element) {
        return element < median_element;
    };

    auto result = filterArrayParallel(pool, array_ptr, isLessMedian);
}


int main() {
    ThreadPool main_pool(2);
    std::shared_ptr<ThreadPool> main_pool_ptr(&main_pool);

    std::vector<int> array_1(ARRAY_SIZE, 0), array_2(ARRAY_SIZE, 0), array_3(ARRAY_SIZE, 0);
    std::shared_ptr< std::vector<int> > array_ptr_1(&array_1), array_ptr_2(&array_2), array_ptr_3(&array_3);

    // generate arrays
    auto task_group = main_pool.createTaskGroup();
    task_group.addTask(std::bind(array_operation::initializeRandom, array_ptr_1));
    task_group.addTask(std::bind(array_operation::initializeRandom, array_ptr_2));
    task_group.addTask(std::bind(array_operation::initializeRandom, array_ptr_3));

    task_group.waitTasksFinished();
    
    // task_group.addTask(std::bind(transformMedianArray, main_pool_ptr, array_ptr_1));
    
    auto testFilter = [](int elem) {return elem % 2 == 0;};
    std::shared_ptr<std::vector<int>> result_ptr(new std::vector<int>());

    auto filterWrapper = [main_pool_ptr, array_ptr_2, testFilter, result_ptr]()
    {
        auto filtered = filterArrayParallel(main_pool_ptr, array_ptr_2, testFilter);
        result_ptr->insert(result_ptr->begin(), filtered.begin(), filtered.end()); 
    };

    main_pool.addTask(filterWrapper).get();
    array_operation::print(*result_ptr);

    // array_operation::print(array_1);
    // array_operation::print(array_2);
    // array_operation::print(array_3);

    main_pool.stopAll();
    return 0;
}
