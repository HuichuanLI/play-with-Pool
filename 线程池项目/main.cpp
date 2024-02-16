//
// Created by lhc456 on 2024/2/16.
//
#include "threadpool.h"

#include <functional>
#include <thread>
#include <iostream>
#include "threadpool.h"

int main() {
    ThreadPool pool;
    // 开始启动线程池
    pool.start(2);
    std::this_thread::sleep_for(std::chrono::seconds(5));

}