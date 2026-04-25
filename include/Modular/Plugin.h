#pragma once
#include "Modular/Library.h"
#include "Modular/Interfaces.h"

namespace Modular {
    class Plugin {
    public:
        using CreateFn = Modular_PluginInterface* (*)();
        using DestroyFn = void (*)(Modular_PluginInterface*);
    private:
        Library m_lib;
        PluginInterface m_instance = nullptr;
        CreateFn m_create = nullptr;
        DestroyFn m_destroy = nullptr;
        
    public:

        Plugin() = default;
        explicit Plugin(std::string_view path, Modular_OnLoadHostInterface* host) { load(path, host); };
        explicit Plugin(std::string_view path, OnLoadHost& host) { load(path, host); };

        ~Plugin() { unload(); }

        Plugin(const Plugin&) = delete;
        Plugin& operator=(const Plugin&) = delete;

        Plugin(Plugin&& other) noexcept {
                m_lib = std::move(other.m_lib);
                m_instance = other.m_instance;
                m_create = other.m_create;
                m_destroy = other.m_destroy;

                other.m_instance = nullptr;
                other.m_create = nullptr;
                other.m_destroy = nullptr;
        }

        Plugin& operator=(Plugin&& other) noexcept {
            if (this != &other) {
                unload();
                m_lib = std::move(other.m_lib);
                m_instance = other.m_instance;
                m_create = other.m_create;
                m_destroy = other.m_destroy;

                other.m_instance = nullptr;
                other.m_create = nullptr;
                other.m_destroy = nullptr;
            }
            return *this;
        }

        bool load(std::filesystem::path path, Modular_OnLoadHostInterface* host) {
            if (!m_lib.load(path)) return false;

            m_create = m_lib.getSymbol<CreateFn>("Modular_createPlugin");
            m_destroy = m_lib.getSymbol<DestroyFn>("Modular_destroyPlugin");

            if (!m_create || !m_destroy) return false;

            m_instance = m_create();

            if(m_instance.getApi() == nullptr) return false;

            m_instance.onLoad(host);
            
            return true;
        }

        bool load(std::string_view path, OnLoadHost& host) {
            return load(path, host.getApi());
        }

        void unload() {
            if (m_instance.getApi()) {
                m_instance.onUnload();
                m_destroy(m_instance.getApi());
                m_instance = nullptr;
            }
            m_lib.unload();
        }

        PluginInterface& get() { return m_instance; }
        const PluginInterface& get() const { return m_instance; }

        bool isLoaded() const { return m_instance.getApi(); }
    };
}