#pragma once
#include <iostream>
#include "Modular/Interfaces.h"

extern "C" {
    struct TestServiceApi {
        void (*print)(void* self, const char* msg);
        void* self;
    };
}

MODULAR_DEFINE_SERVICE(TestServiceApi, "TestService")