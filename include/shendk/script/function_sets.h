#pragma once

#include <vector>
#include <stdint.h>
#include <iostream>

static int64_t nullsub(int64_t* a, int64_t* b, int64_t* c, int64_t* d) {
    std::cout << *a << *b << *c << *d << std::endl;
    return 0;
}

static int64_t (*functionSet1[])(int64_t* a, int64_t* b, int64_t* c, int64_t* d) = { nullsub };

static const std::vector<void*> functionSet2;
static const std::vector<void*> functionSet3;
static const std::vector<void*> functionSet4;
static const std::vector<void*> functionSet5;
static const std::vector<void*> functionSet6;


