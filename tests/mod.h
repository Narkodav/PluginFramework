#pragma once
#define MODULAR_PLUGIN_IMPLEMENTATION

#include "TestService.h"

#include <iostream>

class TestService {
private:
    TestServiceApi m_api;
public:
    TestService() {
        m_api.self = this;
        m_api.print = this->printStatic;
    }

    void print(const char* msg) {
        std::cout << "[TestService] " << msg << std::endl;
    }

    static void printStatic(void* self, const char* msg) {
        static_cast<TestService*>(self)->print(msg);
    }

    TestServiceApi* getApiPointer() {
        return &m_api;
    }

};

class TestPlugin {
private:
    static inline const std::string s_name = "TestPlugin";
    static inline const uint64_t s_version = 1;

    Modular_PluginInterface m_api;
    TestService m_testService;
public:

    TestPlugin() {
        m_api.name = this->nameStatic;
        m_api.onLoad = this->onLoadStatic;
        m_api.onUnload = this->onUnloadStatic;
        m_api.version = this->versionStatic;
        m_api.self = this;
    }

    static const char* nameStatic(void* self) { 
        return getSelf(self)->name();
    }

    const char* name() const { 
        return s_name.c_str();
    }


    static uint64_t versionStatic(void* self) { 
        return getSelf(self)->version();
    }

    uint64_t version() const { 
        return s_version;
    }


    static void onLoadStatic(void* self, Modular_OnLoadHostInterface* host) { 
        return getSelf(self)->onLoad(host);
    }

    void onLoad(Modular_OnLoadHostInterface* host) {
        host->registerService(host->self, Modular::ServiceTraits<TestServiceApi>::s_id, m_testService.getApiPointer());
        std::cout << s_name << " loaded" << std::endl; 
    }

    static void onUnloadStatic(void* self) { 
        return getSelf(self)->onUnload();
    }

    void onUnload() const { std::cout << s_name << " unloaded" << std::endl; }

    Modular_PluginInterface* getApiPointer() { return &m_api; }

private:

    static TestPlugin* getSelf(void* self) { return reinterpret_cast<TestPlugin*>(self); }
};


