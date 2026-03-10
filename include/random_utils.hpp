#pragma once

#include <random>

namespace RandomUtils
{
    inline int rand() {
        thread_local static std::mt19937 generator([]{
            std::random_device rd ;
            return std::mt19937(rd()) ;
        }());

        thread_local static std::uniform_int_distribution<int>
            dis(0, RAND_MAX) ;
        
        return dis(generator) ;
    }
}