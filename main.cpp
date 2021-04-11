
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

#define ARRAY_SIZE 20
#define MAX_PART_SIZE 5


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


std::vector<int> filterByMedian(std::shared_ptr<ThreadPool> pool, std::shared_ptr< std::vector<int> > array_ptr){

    int median_index = 0;
    auto future_median = pool->addTask(std::bind(array_operation::findMedian, array_ptr, &median_index));
    future_median.get();

    int median_element = (*array_ptr)[median_index];

    std::function<bool(int)> isLessMedian = [median_element] (int element) {
        return element < median_element;
    };

    return filterArrayParallel(pool, array_ptr, isLessMedian);
}

std::vector<int> filterIfEven(std::shared_ptr<ThreadPool> pool, std::shared_ptr< std::vector<int> > array_ptr){
    auto mod2filter = [](int elem) {return elem % 2 == 0;};
    return filterArrayParallel(pool, array_ptr, mod2filter);
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
    
    auto future_filter_1 = std::async(std::bind(filterByMedian, main_pool_ptr, array_ptr_1));
    auto future_filter_2 = std::async(std::bind(filterIfEven, main_pool_ptr, array_ptr_2));

    auto filtered_1 = std::shared_ptr<std::vector<int>>(&future_filter_1.get());
    auto filtered_2 = std::shared_ptr<std::vector<int>>(&future_filter_2.get());

    task_group.addTask([filtered_1](){std::sort(filtered_1->begin(), filtered_1->end());});
    task_group.addTask([filtered_2](){std::sort(filtered_2->begin(), filtered_2->end());});
    task_group.addTask([array_ptr_3](){std::sort(array_ptr_3->begin(), array_ptr_3->end());});

    task_group.waitTasksFinished();

    auto filtered_merged = array_operation::merge(*filtered_1, *filtered_2);
    auto result = array_operation::merge(filtered_merged, *array_ptr_3);

    array_operation::print(result);

    main_pool.stopAll();
    return 0;
}
