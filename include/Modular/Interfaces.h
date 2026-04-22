#pragma once
#include <cstdint>

#if defined(_WIN32)
    #if defined(MODULAR_PLUGIN_IMPLEMENTATION)
        #define MODULAR_PLUGIN_API __declspec(dllexport)
    #else
        #define MODULAR_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define MODULAR_PLUGIN_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
    // Implemented by host
    struct Modular_RuntimeHostApi {
        void* (*getService)(void* self, uint64_t id);
        void (*log)(void* self, const char* message);
        void* self;
    };

    // Can only register services on load
    struct Modular_OnLoadHostApi {
        bool (*registerService)(void* self, uint64_t id, void* ptr);
        void (*log)(void* self, const char* message);
        void* self;
    };

    struct Modular_HostApi {
        void*(*getService)(void* self, uint64_t id);
        bool (*registerService)(void* self, uint64_t id, void* ptr);
        void (*log)(void* self, const char* message);
        void* self;
    };

    struct Modular_PluginApi {
        const char* (*name)(void* self);
        uint64_t (*version)(void* self);
        void (*onLoad)(void* self, Modular_OnLoadHostApi* host);
        void (*onUnload)(void* self);
        void* self;
    };

    // Implemented by plugin
    MODULAR_PLUGIN_API Modular_PluginApi* Modular_createPlugin();
    MODULAR_PLUGIN_API void Modular_destroyPlugin(Modular_PluginApi*);
#ifdef __cplusplus
}
#endif

#define MODULAR_DEFINE_PLUGIN(create_fn, destroy_fn) \
extern "C" { \
    MODULAR_PLUGIN_API Modular_PluginApi* Modular_createPlugin() { return create_fn(); } \
    MODULAR_PLUGIN_API void Modular_destroyPlugin(Modular_PluginApi* p) { destroy_fn(p); } \
}

#ifdef __cplusplus
namespace Modular {
    using OnLoadFn = void (*)(void* self, Modular_OnLoadHostApi* host);
    using OnUnloadFn = void (*)(void* self);
    using NameFn = const char* (*)(void* self);
    using VersionFn = uint64_t (*)(void* self);

    using GetServiceFn = void* (*)(void* self, uint64_t id);
    using RegisterServiceFn = bool (*)(void* self, uint64_t id, void* ptr);
    using LogFn = void (*)(void* self, const char* message);


    class RuntimeHost {
    private:
        Modular_RuntimeHostApi* m_api;
    public:
        RuntimeHost(Modular_RuntimeHostApi* api)
            : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        void* getService(uint64_t id) {
            return m_api->getService(m_api->self, id);
        }

        Modular_RuntimeHostApi* getApi() { return m_api; }
    };

    class OnLoadHost {
    private:
        Modular_OnLoadHostApi* m_api;
    public:
        OnLoadHost(Modular_OnLoadHostApi* api)
            : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        bool registerService(uint64_t id, void* ptr) {
            return m_api->registerService(m_api->self, id, ptr);
        }

        Modular_OnLoadHostApi* getApi() { return m_api; }
    };

    class HostInterface {
    private:
        Modular_HostApi* m_api;
    public:
        HostInterface(Modular_HostApi* api) : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        bool registerService(uint64_t id, void* ptr) {
            return m_api->registerService(m_api->self, id, ptr);
        }

        void* getService(uint64_t id) {
            return m_api->getService(m_api->self, id);
        }

        Modular_HostApi* getApi() { return m_api; }
    };

    class PluginInterface {
    private:
        Modular_PluginApi* m_api;
    public:
        PluginInterface(Modular_PluginApi* api) : m_api(api) {}
        PluginInterface& operator=(Modular_PluginApi* api) { m_api = api; return *this; }

        const char* name() const {
            return m_api->name(m_api->self);
        }

        uint64_t version() const {
            return m_api->version(m_api->self);
        }

        void onLoad(Modular_OnLoadHostApi* host) {
            m_api->onLoad(m_api->self, host);
        }

        void onLoad(OnLoadHost& host) {
            m_api->onLoad(m_api->self, host.getApi());
        }

        void onUnload() {
            m_api->onUnload(m_api->self);
        }

        Modular_PluginApi* getApi() { return m_api; }
        const Modular_PluginApi* getApi() const { return m_api; }
    };

    namespace Detail {
        constexpr uint64_t fnv1a(std::string_view str) {
            uint64_t hash = 1469598103934665603ull;
            for(size_t i = 0; i < str.size(); ++i) {
                hash ^= static_cast<uint8_t>(str[i]);
                hash *= 1099511628211ull;
            }
            return hash;
        }

        template <size_t N>
        struct FixedString {
            char data[N];

            constexpr FixedString(const char (&str)[N]) {
                for (size_t i = 0; i < N; ++i)
                    data[i] = str[i];
            }

            constexpr operator std::string_view() const {
                return {data, N - 1};
            }
        };
    }

    template<typename T>
    struct ServiceTraits;

    template<Detail::FixedString str>
    struct ServiceTraitBase {
        static constexpr std::string_view s_name = str;
        static constexpr uint64_t s_id = Detail::fnv1a(s_name);
    };

    // Example specialisation
    // template<>
    // struct ServiceTraits<TestServiceApi> : ServiceTraitBase<"TestService"> {};

}

#define MODULAR_DEFINE_SERVICE(Type, name)                                      \
namespace Modular {                                                             \
    template<>                                                                  \
    struct ServiceTraits<Type> : ServiceTraitBase<name> {};                     \
}
#endif