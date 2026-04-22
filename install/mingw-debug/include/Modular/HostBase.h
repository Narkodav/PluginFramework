#pragma once
#include "Modular/Plugin.h"

#include <string>
#include <unordered_map>

namespace Modular {
    class HostBase {
        struct TransparentHash {
            using is_transparent = void;

            size_t operator()(std::string_view sv) const noexcept {
                return std::hash<std::string_view>{}(sv);
            }

            size_t operator()(const std::string& s) const noexcept {
                return std::hash<std::string_view>{}(s);
            }

            size_t operator()(const char* s) const noexcept {
                return std::hash<std::string_view>{}(s);
            }
        };

        struct TransparentEqual {
            using is_transparent = void;

            bool operator()(std::string_view lhs,
                            std::string_view rhs) const noexcept {
                return lhs == rhs;
            }
        };
    protected:
        std::unordered_map<std::string, void*, TransparentHash, TransparentEqual> m_services;
        Modular_HostApi m_hostApi;
        Modular_OnLoadHostApi m_onLoadHostApi;
        Modular_RuntimeHostApi m_runtimeHostApi;

    public:

        HostBase() {
            m_hostApi.getService = this->getServiceStatic;
            m_hostApi.registerService = this->registerServiceStatic;
            m_hostApi.log = nullptr;
            m_hostApi.self = this;

            m_onLoadHostApi.registerService = this->registerServiceStatic;
            m_onLoadHostApi.log = nullptr;
            m_onLoadHostApi.self = this;

            m_runtimeHostApi.getService = this->getServiceStatic;
            m_runtimeHostApi.log = nullptr;
            m_runtimeHostApi.self = this;
        }

        bool registerService(std::string_view name, void* ptr) {
            auto [_, inserted] = m_services.insert({std::string(name), ptr});
            return inserted;
        }

        static bool registerServiceStatic(void* self, const char* name, void* ptr) {
            return reinterpret_cast<HostBase*>(self)->registerService(name, ptr);
        }

        template<typename T = void>
        T* getService(std::string_view name) {
            auto it = m_services.find(name);
            if(it == m_services.end()) return nullptr;
            return reinterpret_cast<T*>(it->second);
        }

        template<typename T>
        T* getService() {
            auto it = m_services.find(ServiceTraits<T>::s_name);
            if(it == m_services.end()) return nullptr;
            return reinterpret_cast<T*>(it->second);
        }

        static void* getServiceStatic(void* self, const char* name) {
            return reinterpret_cast<HostBase*>(self)->getService<void*>(name);
        }

        Modular_HostApi* getInterface() {
            return &m_hostApi;
        }

        Modular_OnLoadHostApi* getInterfaceOnLoad() {
            return &m_onLoadHostApi;
        }

        Modular_RuntimeHostApi* getInterfaceRuntime() {
            return &m_runtimeHostApi;
        }
    };
}