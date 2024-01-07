#ifndef JUTIL_SHARED_PTR_HPP
#define JUTIL_SHARED_PTR_HPP

namespace jutil {
    template <typename T>
    class SharedPtr {
        public:
        
            SharedPtr() : _ptr(nullptr), _refCount(0) {}
            SharedPtr(T *ptr) : _ptr(ptr), _refCount(1) {}
            SharedPtr(const SharedPtr<T> &other) : _ptr(other._ptr), _refCount(other._refCount) {
                ++_refCount;
            }

            ~SharedPtr() {
                if (--_refCount == 0) {
                    delete _ptr;
                }
            }

            T *operator->() {
                return _ptr;
            }

            T &operator*() {
                return *_ptr;
            }

        private:
            T *_ptr;
            size_t _refCount;          
    };
}

#endif // JUTIL_SHARED_PTR_HPP