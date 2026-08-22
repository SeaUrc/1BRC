#pragma once
#include <vector>

template<typename K, typename V>
class flatmap {
private:
    size_t sz = 0;
    std::optional<std::pair<K, V>>* arr = nullptr;

public:
    // default
    flatmap() = default;

    ~flatmap() {
        if (arr) {
            std::destroy_n(arr, sz);
            ::operator delete(arr);
        }
    }

    // copy constructor
    flatmap(const flatmap& other) : sz(other.sz) {
        if (sz == 0) return;

        arr = static_cast<std::optional<std::pair<K, V>>*>(
            ::operator new(sizeof(std::optional<std::pair<K, V>>) * sz)
        );

        for (size_t i=0; i<sz; ++i) {
            new (&arr[i]) std::optional<std::pair<K, V>>(other.arr[i]);
        }
    }

    flatmap& operator=(const flatmap& other) {
        if (this == &other) return *this;

        flatmap temp(other);

        std::swap(sz, temp.sz);
        std::swap(arr, temp.arr);

        return *this;
    }

    // move constructor
    flatmap(flatmap &&other) noexcept : sz(other.sz), arr(other.arr) {
        other.sz = 0;
        other.arr = nullptr;
    };

    // move assignment
    flatmap& operator=(flatmap &&other) noexcept {
        if (this == &other) return *this;
        std::swap(sz, other.sz);
        std::swap(arr, other.arr);
        return *this;
    }

    void reserve(size_t initsize) {
        sz = initsize;
        arr = static_cast<std::optional<std::pair<K, V>>*>(operator new(sizeof(std::optional<std::pair<K, V>>) * sz));
    }

    // @pre reserved before
    size_t getCapacity(){
        return sz;
    }

    void insert(size_t hashId, K key, V value) {
        size_t d = 0;
        while (arr[(hashId + d*d) % sz].has_value()) {
            d++;
        }
        arr[(hashId + d*d) % sz] = std::make_pair(key, value);
    }

    V& getOrMake(size_t hashId, K key) {
        size_t d =0;
        std::optional<std::pair<K, V>> v;
        while ((v = arr[(hashId + d*d) % sz]).has_value()) {
            if ((*v).first == key) {
                return (*v).second;
            }
        }
        arr[(hashId + d*d) % sz] = std::make_pair(key, V());
        return (*(arr[(hashId + d*d) % sz])).second;
    }

    size_t begin() {
        size_t i=0;
        while (i < sz && !arr[i].has_value()) {
            ++i;
        }
        return i;
    }

    size_t end() {
        return sz;
    }

    size_t next(size_t iter){
        size_t i=iter;
        while (i < sz && !arr[i].has_value()) {
            ++i;
        }
        return i;
    }

    // @pre iter is returned from next() or begin()
    std::pair<K, V> getIter(size_t iter) {
        return *arr[iter];
    }
};