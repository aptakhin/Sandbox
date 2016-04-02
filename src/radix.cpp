#include <assert.h>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>

unsigned int count[2048] = {0};
constexpr auto COUNT_SIZE = sizeof(count) / sizeof(count[0]);

void radix3pass(unsigned int n_bits, unsigned int offset_bits, unsigned int* from, unsigned int* to, size_t size) {
    if (size <= 1) {
        return;
    }

    auto count_size = 1 << n_bits;

    assert(count_size <= int(COUNT_SIZE) && "Too small count array");
    /*auto mask = (1u << (offset_bits + n_bits + 1)) - 1u;
    if (offset_bits + n_bits == 32) {
        mask = ~0;
    }
    if (offset_bits > 0) {
        mask -= (1u << (offset_bits + 1)) - 1u;
    }*/

    auto mask = 0u;
    for (size_t i = offset_bits; i < offset_bits + n_bits; ++i) {
        mask |= (1 << i);
    }

    for (size_t i = 0; i < count_size; ++i) {
        count[i] = 0;
    }
    for (size_t i = 0; i < size; ++i) {
        auto c = (from[i] & mask) >> offset_bits;
        assert(c < count_size);
        count[c] += 1;
    }
    for (size_t i = 1; i < count_size; ++i) {
        count[i] += count[i - 1];
    }

    for (size_t i = count_size - 1; i >= 1; --i) {
        count[i] = count[i - 1];
    }
    count[0] = 0;

    for (size_t i = 0; i < size; ++i) {
        auto c = (from[i] & mask) >> offset_bits;
        auto s = count[c];
        assert(s < size);
        to[s] = from[i];
        ++count[c];
    }
}

void radix3pass(unsigned int* orig, unsigned int* add, size_t size) {
    radix3pass(10, 0, orig, add, size);
    radix3pass(11, 10, add, orig, size);
    radix3pass(11, 21, orig, add, size);
}

int main() {
    typedef unsigned int SortType;

    std::random_device rd;
    std::uniform_int_distribution<SortType> dist(0, std::numeric_limits<SortType>::max());

    size_t size = 5000000;
    std::vector<SortType> orig;
    orig.resize(size);
    for (size_t i = 0; i < size; ++i) {
        orig[i] = dist(rd);
    }
    std::vector<SortType> second = orig;
    std::vector<SortType> cpy;
    cpy.resize(orig.size());

    auto sort1 = std::chrono::high_resolution_clock::now();
    radix3pass(&orig.front(), &cpy.front(), size);
    auto sort2 = std::chrono::high_resolution_clock::now();
    std::sort(second.begin(), second.end());
    auto sort3 = std::chrono::high_resolution_clock::now();

    auto good = cpy == second;

    std::cout << "Good " << std::boolalpha << good << "\n";
    std::cout << std::chrono::duration<double>{sort2 - sort1}.count() << " s\n";
    std::cout << std::chrono::duration<double>{sort3 - sort2}.count() << " s\n";
    return 0;
}