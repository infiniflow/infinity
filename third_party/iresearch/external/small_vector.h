// Licensed under the Unlicense <https://unlicense.org/>
// SPDX-License-Identifier: Unlicense
#pragma once
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>

namespace sbo{

    template<typename T, size_t MaxSize = 8, typename NonReboundT = T>
    struct small_buffer_vector_allocator{
        alignas(alignof(T)) std::byte m_smallBuffer[MaxSize * sizeof(T)];
        std::allocator<T> m_alloc{};
        bool m_smallBufferUsed = false;
        
        using value_type = T;
        //we have to set this three values, as they are responsible for the correct handling of the move assignment operator
        using propagate_on_container_move_assignment = std::false_type;
        using propagate_on_container_swap = std::false_type;
        using is_always_equal = std::false_type;

        constexpr small_buffer_vector_allocator() noexcept = default;
        template<class U>
        constexpr small_buffer_vector_allocator(const small_buffer_vector_allocator<U, MaxSize, NonReboundT>&) noexcept {}

        template <class U>
        struct rebind{
            typedef small_buffer_vector_allocator<U, MaxSize, NonReboundT> other;
        };
        //don't copy the small buffer for the copy/move constructors, as the copying is done through the vector
        constexpr small_buffer_vector_allocator(const small_buffer_vector_allocator& other) noexcept : m_smallBufferUsed(other.m_smallBufferUsed) {}
        constexpr small_buffer_vector_allocator& operator=(const small_buffer_vector_allocator& other) noexcept {  m_smallBufferUsed = other.m_smallBufferUsed; return *this; }
        constexpr small_buffer_vector_allocator(small_buffer_vector_allocator&&) noexcept {}
        constexpr small_buffer_vector_allocator& operator=(const small_buffer_vector_allocator&&) noexcept { return *this; }

        [[nodiscard]] constexpr T* allocate(const size_t n) {
            //when the allocator was rebound we don't want to use the small buffer
            if constexpr (std::is_same_v<T, NonReboundT>) {
                if (n <= MaxSize) {
                    m_smallBufferUsed = true;
                    //as long as we use less memory than the small buffer, we return a pointer to it
                    return reinterpret_cast<T*>(&m_smallBuffer);
                }
            }
            m_smallBufferUsed = false;
            //otherwise use the default allocator
            return m_alloc.allocate(n);
        }
        constexpr void deallocate(void* p, const size_t n) {
          // we don't deallocate anything if the memory was allocated in small buffer
          if (&m_smallBuffer != p) 
              m_alloc.deallocate(static_cast<T*>(p), n);
          m_smallBufferUsed = false;
        }
        //according to the C++ standard when propagate_on_container_move_assignment is set to false, the comparision operators are used 
        //to check if two allocators are equal. When they are not, an element wise move is done instead of just taking over the memory. 
        //For our implementation this means the comparision has to return false, when the small buffer is active
        friend constexpr bool operator==(const small_buffer_vector_allocator& lhs, const small_buffer_vector_allocator& rhs) {
            return !lhs.m_smallBufferUsed && !rhs.m_smallBufferUsed;
        }
        friend constexpr bool operator!=(const small_buffer_vector_allocator& lhs, const small_buffer_vector_allocator& rhs) {
            return !(lhs == rhs);
        }
    };

    template<typename T, size_t N = 8>
    class small_vector : public std::vector<T, small_buffer_vector_allocator<T, N>>{
    public:
        using vectorT = std::vector<T, small_buffer_vector_allocator<T, N>>;
        //default initialize with the small buffer size
        constexpr small_vector() noexcept { vectorT::reserve(N); }
        small_vector(const small_vector&) = default;
        small_vector& operator=(const small_vector&) = default;
        small_vector(small_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
            if (other.size() <= N)
                vectorT::reserve(N);
            vectorT::operator=(std::move(other));
        }
        small_vector& operator=(small_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
            if(other.size() <= N)
                vectorT::reserve(N);
            vectorT::operator=(std::move(other));
            return *this;
        }
        //use the default constructor first to reserve then construct the values
        explicit small_vector(size_t count) : small_vector() { vectorT::resize(count); }
        small_vector(size_t count, const T& value) : small_vector() { vectorT::assign(count, value); }
        template<class InputIt>
        small_vector(InputIt first, InputIt last) : small_vector()   { vectorT::insert(vectorT::begin(), first, last); }
        small_vector(std::initializer_list<T> init) : small_vector() { vectorT::insert(vectorT::begin(), init); }
        friend void swap(small_vector& a, small_vector& b) noexcept {
            using std::swap;
            swap(static_cast<vectorT&>(a), static_cast<vectorT&>(b));
        }
    };
}
