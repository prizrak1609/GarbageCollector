#include "GarbageCollectorReferenceCount.h"

#include <mutex>

GarbageCollectorReferenceCount::~GarbageCollectorReferenceCount() {
    delete[] memory;
}

GarbageCollectorReferenceCount& GarbageCollectorReferenceCount::getInstance() {
    static std::unique_ptr<GarbageCollectorReferenceCount, std::function<void(GarbageCollectorReferenceCount*)>> instance;
    static std::once_flag flag;
    std::call_once(flag, [&](){
        instance = std::unique_ptr<GarbageCollectorReferenceCount, std::function<void(GarbageCollectorReferenceCount*)>>(new GarbageCollectorReferenceCount(), [](GarbageCollectorReferenceCount* ptr){
            delete ptr;
        });
    });
    return *instance.get();
}

void GarbageCollectorReferenceCount::compact() {
    if (memory == nullptr || *memory == 0) {
        return;
    }
    InternalObject *begin = (InternalObject *) memory;
    while (true) {
        if (freeMemoryStart == memory || freeMemoryStart == (byte*)begin)
        {
            break;
        }
        if (*begin->references == 0)
        {
            size_t sizeToEnd = freeMemoryStart - (byte*)begin - begin->objSize;
            std::memcpy(begin, (byte*)begin + begin->objSize, sizeToEnd);
            freeMemoryStart -= begin->objSize;
            continue;
        }

        begin = (InternalObject*)((byte*)begin + begin->objSize);
        if ((byte*)begin >= freeMemoryStart)
        {
            break;
        }
    }
}

void GarbageCollectorReferenceCount::setMemorySize(size_t size) {
    memory = (byte *) calloc(1, size);
    freeMemoryStart = memory;
    freeMemoryEnd = memory + size;
}
