#pragma once

    template <class T1> class containerAllocator;

    template <> class containerAllocator<void> {
    public:
        typedef void * pointer;
        typedef const void* const_pointer;
        typedef void value_type;
        template <class U1> struct rebind { typedef containerAllocator<U1> other; };
    };


    template <class T1> class containerAllocator {
    public:

        typedef T1 value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T1* pointer;
        typedef const T1* const_pointer;
        typedef T1& reference;
        typedef const T1& const_reference;

        template <class U> struct rebind {
            typedef containerAllocator<U> other;
        };

        containerAllocator() {}
        containerAllocator(const containerAllocator& other ) {}

        template <class U> containerAllocator(const containerAllocator<U> &) {}

        template <class U> containerAllocator& operator=(const containerAllocator<U> &) {}

        ~containerAllocator() {}

        pointer address(reference r) const { return &r; };
        const_pointer address(const_reference r) const { return &r; };

        pointer allocate(size_type n, containerAllocator<void>::const_pointer hint = 0 ) {
            job::pCurrentHeap = job::pNextHeap;
            job::pNextHeap = (BYTE *)job::pCurrentHeap + n * sizeof(T1);
            return reinterpret_cast<pointer>(job::pCurrentHeap);
        }

        void deallocate(pointer p, size_type n) { }

        size_type max_size() const {
            return job::currentlyAllocatedHeap - ((BYTE *)job::pNextHeap - (BYTE *)job::pHeap);
        }

        void construct(pointer ptr, const T1& val) {
            ::new (reinterpret_cast<void*>(ptr)) T1(val);
        }

        void destroy(pointer p) {
            p -> T1::~T1();
        }

    private:

    };


