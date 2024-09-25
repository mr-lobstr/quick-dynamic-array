#ifndef DYNAMIC_ARRAY_GUARD_H
#define DYNAMIC_ARRAY_GUARD_H

#include <stdexcept>
#include <utility>
#include "my_algorithm.h"

template <typename T>
class DynamicArray
{
public:
    DynamicArray() noexcept = default;

    DynamicArray (DynamicArray&& rhs)
        : data (std::exchange(rhs.data,  Data{}))
    {}

    DynamicArray (DynamicArray const& rhs)
    {
        mem_alloc_and_data_init (rhs.size(), rhs.size());
    }

    DynamicArray (std::size_t count, T const& value = T())
    {
        mem_alloc_and_data_init (count, count);
        for_each (data.begin, data.avail,
            [&] (T* ptr) {
                new(ptr) T {value};
            }        
        );
    }

    DynamicArray& operator= (DynamicArray&& rhs)
    {
        if (this == &rhs)
            return *this;
        
        auto tmp {std::move (rhs)};
        swap (tmp);
        return *this;
    }

    DynamicArray& operator= (DynamicArray const& rhs)
    {
        if (this == &rhs)
            return *this;
        
        auto tmp {rhs};
        swap (tmp);
        return *this;
    }

    ~DynamicArray() noexcept
    {
        resize(0);
        mem_free (data.begin);
    }

    void swap (DynamicArray& rhs) noexcept
    {
        std::swap (data, rhs.data);
    }

    std::size_t size() const noexcept
    {
        return data.avail - data.begin;
    }

    std::size_t capacity() const noexcept
    {
        return data.limit - data.begin;
    }

    bool empty() const noexcept
    {
        return size() == 0;
    }

    void reserve (std::size_t newCapacity)
    {
        realloc_if_capacity_less (newCapacity, newCapacity);
    }

    void resize (std::size_t newSize)
    {
        reserve (newSize);
        
        while (size() != newSize)
        {
            size() < newSize ? push_back (T{})
                             : pop_back();
        }
    }

    T& operator[] (std::size_t index) noexcept
    {
        return data.begin[index];
    }

    T const& operator[] (std::size_t index) const noexcept
    {
        return data.begin[index];
    }

    void push_back (T const& value)
    {
        reserve_before_insert();
        new(data.avail) T {value};
        ++data.avail;
    }

    void pop_back()
    {
        if (empty())
            std::cerr << "!!!!";

        --data.avail;
        data.avail->~T();
    }

    void insert (std::size_t index, T const& value)
    {
        check_out_of_range (index);    
        reserve_before_insert();
        
        auto pos = data.begin + index;
        auto prevPos = pos - 1;
        auto prevAvail = data.avail - 1;

        new(data.avail) T {std::move (*prevAvail)};
        transform (prevAvail, prevPos, data.avail,
            [] (T* ptr, T& value) {
                *ptr = std::move (value);
            }
        );
        new(pos) T {value};

        ++data.avail;
    }

    void erase (std::size_t index)
    {
        check_out_of_range(index);

        auto pos = data.begin + index;
        auto nextPos = pos + 1;

        transform (nextPos, data.avail, pos,
            [] (T* ptr, T& value) {
                *ptr = std::move (value);
            }
        );

        --data.avail;
    }

private:
    void mem_alloc_and_data_init (std::size_t size_, std::size_t capacity_)
    {        
        auto mem = reinterpret_cast<T*> (
            ::operator new (sizeof(T) * capacity_)
        );

        data.begin = mem;
        data.avail = mem + size_;
        data.limit = mem + capacity_;
    }

    void mem_free (T* ptr)
    {
        ::operator delete(ptr);
    }

    void check_out_of_range(std::size_t index)
    {
        if (index >= size()) throw std::out_of_range {
            "выход за пределы динамического массива\n"
        };
    }

    void realloc_if_capacity_less (std::size_t lowerBound, std::size_t newCapacity)
    {
        if (capacity() >= lowerBound)
            return;

        auto old = data;

        mem_alloc_and_data_init (size(), newCapacity);
        transform (old.begin, old.avail, data.begin,
            [] (T* ptr, T& value) {
                new(ptr) T {std::move (value)};
            }
        );
        mem_free (old.begin);
    }

    void reserve_before_insert ()
    {
        realloc_if_capacity_less(size() + 1, size() * 2);
    }

private:
    struct Data
    {
        T* begin = nullptr;
        T* avail = nullptr;
        T* limit = nullptr;
    };

    Data data{};
};

#endif