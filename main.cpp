#include "GarbageCollectorReferenceCount.h"
#include <iostream>

int main() {
    auto& gc = GarbageCollectorReferenceCount::getInstance();
    gc.setMemorySize(108);

    auto obj2 = gc.New<int>();
    *obj2 = 1001;

    {
        auto obj = gc.New<int>();
        *obj = 100;

        auto obj1 = gc.Copy(obj);

        auto obj_c = obj;

        std::cout << *obj << std::endl;
        std::cout << *obj_c << std::endl;
        std::cout << *obj1 << std::endl;
        std::cout << *obj2 << std::endl;
    }

    gc.compact();

    {
        auto obj = gc.New<std::string>();
        *obj = "test";
        std::cout << *obj << std::endl;
    }

    std::cout << *obj2 << std::endl;

    return 0;
}
