
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
#include <chrono>
#include "thread_pool/ThreadPool.hpp"
#include "array_operations.hpp"
#include "concurrent_console.hpp"


#define ARRAY_SIZE 100000
#define MAX_PART_SIZE 200


std::vector<int> filterByMedian(ThreadPool *pool,  std::vector<int> *array_ptr){

    int median_element = 0;
    auto future_median = pool->addTask(std::bind(array_operation::findMedian, array_ptr, &median_element));
    future_median.get();

    // find index of element selected by array_operation::findMedian
    size_t median_index = 0;
    while (median_index < array_ptr->size()){
        if (array_ptr->at(median_index) == median_element){
            break;
        }else{
            median_index++;
        }
    }

    console::print("median index: " + std::to_string(median_index) + " median value: " + std::to_string(median_element));

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

    auto start = std::chrono::high_resolution_clock::now();

    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_1));
    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_2));
    pool.addTaskToGroup(std::bind(array_operation::initializeRandom, &array_3));

    pool.waitGroupFinished();
    
    auto future_filter_1 = std::async(std::bind(filterByMedian, &pool, &array_1));
    auto future_filter_2 = std::async(std::bind(filterIfEven, &pool, &array_2));

    auto filtered_1 = future_filter_1.get();
    auto filtered_2 = future_filter_2.get();

    console::print("filtered 1 and 2 arrays");

    pool.addTaskToGroup(std::bind(array_operation::sort, &filtered_1));
    pool.addTaskToGroup(std::bind(array_operation::sort, &filtered_2));
    pool.addTaskToGroup(std::bind(array_operation::sort, &array_3));

    pool.waitGroupFinished();
    console::print("sorted all arrays");

    pool.stopAll();

    auto filtered_merged = array_operation::merge(filtered_1, filtered_2);
    auto result = array_operation::merge(filtered_merged, array_3);
    console::print("merged arrays");

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f;
    console::print("finished execution in " + std::to_string(elapsed));
    return 0;
}
