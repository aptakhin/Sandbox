#include <assert.h>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>

unsigned int count[2048] = {0};
constexpr auto COUNT_SIZE = sizeof(count) / sizeof(count[0]);

void radix_pass(const unsigned int n_bits, const unsigned int offset_bits, unsigned int* from, unsigned int* to, const size_t size) {
    if (size <= 1) {
        return;
    }

    auto count_size = 1 << n_bits;
    assert(count_size <= int(COUNT_SIZE) && "Too small count array");
    auto mask = ((1 << n_bits) - 1) << offset_bits;
    memset(count, 0, count_size * sizeof(count[0]));

    for (size_t i = 0; i < size; ++i) {
        auto c = (from[i] & mask) >> offset_bits;
        assert(c < count_size);
        count[c] += 1;
    }
    auto next = count[0];
    count[0] = 0;
    for (size_t i = 1; i < count_size; ++i) {
        auto cur = count[i];
        count[i] = count[i - 1] + next;
        next = cur;
    }

    for (size_t i = 0; i < size; ++i) {
        auto c = (from[i] & mask) >> offset_bits;
        auto s = count[c];
        assert(s < size);
        to[s] = from[i];
        ++count[c];
    }
}

void radix3pass(unsigned int* orig, unsigned int* add, const size_t size) {
    radix_pass(10, 0, orig, add, size);
    radix_pass(11, 10, add, orig, size);
    radix_pass(11, 21, orig, add, size);
}

void radix4pass(unsigned int* orig, unsigned int* add, const size_t size) {
    radix_pass(8, 0, orig, add, size);
    radix_pass(8, 8, add, orig, size);
    radix_pass(8, 16, orig, add, size);
    radix_pass(8, 24, add, orig, size);
}

void test(unsigned int size) {
    typedef unsigned int SortType;

    std::cout << "Size: " << size << "\n";

    std::random_device rd;
    std::uniform_int_distribution<SortType> dist(0, std::numeric_limits<SortType>::max());

    std::vector<SortType> orig;
    orig.resize(size);
    for (size_t i = 0; i < size; ++i) {
        orig[i] = dist(rd);
    }
    std::vector<SortType> second = orig;
    std::vector<SortType> third = orig;
    std::vector<SortType> cpy;
    cpy.resize(orig.size());

    const auto sort1 = std::chrono::high_resolution_clock::now();
    radix3pass(&orig.front(), &cpy.front(), size);

    const auto sort2 = std::chrono::high_resolution_clock::now();
    std::sort(second.begin(), second.end());
    const auto good = cpy == second;

    const auto sort3 = std::chrono::high_resolution_clock::now();
    radix4pass(&third.front(), &cpy.front(), size);
    const auto sort4 = std::chrono::high_resolution_clock::now();

    const auto good2 = third == second;

    std::cout << "Good " << std::boolalpha << good << " " << good2 << "\n";
    const auto radix = std::chrono::duration<double>{sort2 - sort1}.count();
    const auto ssort = std::chrono::duration<double>{sort3 - sort2}.count();
    const auto r4sort = std::chrono::duration<double>{sort4 - sort3}.count();
    std::cout << radix << " s\n";
    std::cout << ssort << " s\n";
    std::cout << r4sort << " s\n";
    std::cout << "Ratio: " << ssort / radix << " " << ssort / r4sort << "\n";
    std::cout << "------------------------------------\n";
}

int main() {
    test(100000u);
    test(1000000u);
    test(10000000u);
    test(100000000u);
    //test(1000000000u);
    return 0;
}