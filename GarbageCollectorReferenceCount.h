#ifndef GARBAGECOLLECTOR_GARBAGECOLLECTORREFERENCECOUNT_H
#define GARBAGECOLLECTOR_GARBAGECOLLECTORREFERENCECOUNT_H

#include <memory>
#include <functional>
#include <string>

class GarbageCollectorReferenceCount {
    friend class std::unique_ptr<GarbageCollectorReferenceCount>;

private:
    struct InternalObject {
        size_t *references = nullptr;
        size_t objSize = 0;
        void *ptr = nullptr;
    };

    template<typename T>
    class Object {
        friend class GarbageCollectorReferenceCount;

    private:
        size_t *references = nullptr;
        size_t objSize = 0;
        void *ptr = nullptr;

    public:
        T &operator->() {
            return *(T *) ptr;
        }

        T &operator*() {
            return *(T *) ptr;
        }

        Object() = default;

        Object(const Object& other)
        {
            (*other.references)++;
            references = other.references;
            objSize = other.objSize;
            ptr = other.ptr;
        }

        Object<T>& operator=(const Object<T>& other) {
            (*other.references)++;
            references = other.references;
            objSize = other.objSize;
            ptr = other.ptr;
        }

        ~Object<T>() {
            if (*references == 0) {
                throw std::runtime_error("wrong deletion");
            }
            (*references)--;
        }
    };

public:
    static GarbageCollectorReferenceCount &getInstance();

    void setMemorySize(size_t size);

    template<typename T>
    Object<T> New() {
        if (sizeof(Object<T>) + sizeof(T) + sizeof(size_t) > freeMemoryEnd - freeMemoryStart) {
            throw std::runtime_error("no memory " + std::to_string(sizeof(Object<T>) + sizeof(T) + sizeof(size_t)) + " > " + std::to_string(freeMemoryEnd - freeMemoryStart));
        }
        Object<T> *obj = new(freeMemoryStart) Object<T>();
        freeMemoryStart += sizeof(Object<T>);
        obj->references = new(freeMemoryStart) size_t(0);
        freeMemoryStart += sizeof(size_t);
        obj->ptr = new(freeMemoryStart) T();
        freeMemoryStart += sizeof(T);
        obj->objSize = sizeof(Object<T>) + sizeof(T) + sizeof(size_t);
        return *obj;
    }

    template<typename T>
    Object<T> Copy(Object<T>& other) {
        Object<T> obj = New<T>();
        std::memcpy(obj.ptr, other.ptr, sizeof(T));
        return obj;
    }

    void compact();

private:
    using byte = unsigned char;

    ~GarbageCollectorReferenceCount();

    byte *memory = nullptr;
    byte *freeMemoryStart = nullptr;
    byte *freeMemoryEnd = nullptr;
};

#endif //GARBAGECOLLECTOR_GARBAGECOLLECTORREFERENCECOUNT_H
