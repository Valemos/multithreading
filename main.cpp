
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
#include "concurrent_console.hpp"


#define ARRAY_SIZE 10000
#define MAX_PART_SIZE 200


std::vector<int> filterByMedian(ThreadPool *pool,  std::vector<int> *array_ptr){

    int median_index = 0;
    auto future_median = pool->addTask(std::bind(array_operation::findMedian, array_ptr, &median_index));
    future_median.get();

    int median_element = (*array_ptr)[median_index];

    std::function<bool(int)> isLessMedian = [median_element] (int element) {
        return element < median_element;
    };

    return array_operation::filterParallel(pool, array_ptr, isLessMedian, MAX_PART_SIZE);
}


std::vector<int> filterIfEven(ThreadPool *pool,  std::vector<int> *array_ptr){
    auto mod2filter = [](int elem) {return elem % 2 == 0;};
    return array_operation::filterParallel(pool, array_ptr, mod2filter, MAX_PART_SIZE);
}


int main() {
    ThreadPool pool(2);

    std::vector<int> array_1(ARRAY_SIZE, 0), array_2(ARRAY_SIZE, 0), array_3(ARRAY_SIZE, 0);


    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_1));
    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_2));
    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_3));

    pool.waitGroupFinished();
    
    auto future_filter_1 = std::async(std::bind(filterByMedian, &pool, &array_1));
    auto future_filter_2 = std::async(std::bind(filterIfEven, &pool, &array_2));

    auto filtered_1 = future_filter_1.get();
    auto filtered_2 = future_filter_2.get();

    pool.addTaskToGroup(std::bind(array_operation::sort, &filtered_1));
    pool.addTaskToGroup(std::bind(array_operation::sort, &filtered_2));
    pool.addTaskToGroup(std::bind(array_operation::sort, &array_3));

    pool.waitGroupFinished();

    auto filtered_merged = array_operation::merge(filtered_1, filtered_2);
    auto result = array_operation::merge(filtered_merged, array_3);

    pool.stopAll();
    console::print("finished");
    return 0;
}
