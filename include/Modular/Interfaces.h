#pragma once
#include <cstdint>
#include <string_view>

#if defined(_WIN32)
    #if defined(MODULAR_PLUGIN_IMPLEMENTATION)
        #define MODULAR_PLUGIN_API __declspec(dllexport)
    #else
        #define MODULAR_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define MODULAR_PLUGIN_API
#endif

typedef uint64_t Modular_ServiceId;

#ifdef __cplusplus
extern "C" {
#endif
    // Implemented by host
    struct Modular_RuntimeHostInterface {
        void* (*getService)(void* self, Modular_ServiceId id);
        void (*log)(void* self, const char* message);
        void* self;
    };

    // Can only register services on load
    struct Modular_OnLoadHostInterface {
        bool (*registerService)(void* self, Modular_ServiceId id, void* ptr);
        void (*log)(void* self, const char* message);
        void* self;
    };

    struct Modular_HostInterface {
        void*(*getService)(void* self, Modular_ServiceId id);
        bool (*registerService)(void* self, Modular_ServiceId id, void* ptr);
        void (*log)(void* self, const char* message);
        void* self;
    };

    struct Modular_PluginInterface {
        const char* (*name)(void* self);
        uint64_t (*version)(void* self);
        void (*onLoad)(void* self, Modular_OnLoadHostInterface* host);
        void (*onUnload)(void* self);
        void* self;
    };

    // Implemented by plugin
    MODULAR_PLUGIN_API Modular_PluginInterface* Modular_createPlugin();
    MODULAR_PLUGIN_API void Modular_destroyPlugin(Modular_PluginInterface*);
#ifdef __cplusplus
}
#endif

#define MODULAR_DEFINE_PLUGIN(create_fn, destroy_fn) \
extern "C" { \
    MODULAR_PLUGIN_API Modular_PluginInterface* Modular_createPlugin() { return create_fn(); } \
    MODULAR_PLUGIN_API void Modular_destroyPlugin(Modular_PluginInterface* p) { destroy_fn(p); } \
}

#ifdef __cplusplus
namespace Modular {
    using OnLoadFn = void (*)(void* self, Modular_OnLoadHostInterface* host);
    using OnUnloadFn = void (*)(void* self);
    using NameFn = const char* (*)(void* self);
    using VersionFn = uint64_t (*)(void* self);

    using GetServiceFn = void* (*)(void* self, uint64_t id);
    using RegisterServiceFn = bool (*)(void* self, uint64_t id, void* ptr);
    using LogFn = void (*)(void* self, const char* message);

    using ServiceId = Modular_ServiceId;


    class RuntimeHost {
    private:
        Modular_RuntimeHostInterface* m_api;
    public:
        RuntimeHost(Modular_RuntimeHostInterface* api)
            : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        void* getService(ServiceId id) {
            return m_api->getService(m_api->self, id);
        }

        Modular_RuntimeHostInterface* getApi() { return m_api; }
    };

    class OnLoadHost {
    private:
        Modular_OnLoadHostInterface* m_api;
    public:
        OnLoadHost(Modular_OnLoadHostInterface* api)
            : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        bool registerService(ServiceId id, void* ptr) {
            return m_api->registerService(m_api->self, id, ptr);
        }

        Modular_OnLoadHostInterface* getApi() { return m_api; }
    };

    class HostInterface {
    private:
        Modular_HostInterface* m_api;
    public:
        HostInterface(Modular_HostInterface* api) : m_api(api) {}

        void log(const char* msg) {
            m_api->log(m_api->self, msg);
        }

        bool registerService(ServiceId id, void* ptr) {
            return m_api->registerService(m_api->self, id, ptr);
        }

        void* getService(ServiceId id) {
            return m_api->getService(m_api->self, id);
        }

        Modular_HostInterface* getApi() { return m_api; }
    };

    class PluginInterface {
    private:
        Modular_PluginInterface* m_api;
    public:
        PluginInterface(Modular_PluginInterface* api) : m_api(api) {}
        PluginInterface& operator=(Modular_PluginInterface* api) { m_api = api; return *this; }

        const char* name() const {
            return m_api->name(m_api->self);
        }

        uint64_t version() const {
            return m_api->version(m_api->self);
        }

        void onLoad(Modular_OnLoadHostInterface* host) {
            m_api->onLoad(m_api->self, host);
        }

        void onLoad(OnLoadHost& host) {
            m_api->onLoad(m_api->self, host.getApi());
        }

        void onUnload() {
            m_api->onUnload(m_api->self);
        }

        Modular_PluginInterface* getApi() { return m_api; }
        const Modular_PluginInterface* getApi() const { return m_api; }
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

    constexpr ServiceId serviceNameToId(std::string_view name) {
        return Detail::fnv1a(name);
    }

    template<typename T>
    struct ServiceTraits;

    template<Detail::FixedString str>
    struct ServiceTraitBase {
        static constexpr std::string_view s_name = str;
        static constexpr ServiceId s_id = serviceNameToId(s_name);
    };

    // Example specialisation, only valid for unique services, doesnt work for interfaces
    // template<>
    // struct ServiceTraits<TestServiceApi> : ServiceTraitBase<"TestService"> {};

    template<typename Derived>
    struct ModuleBase {
    protected:
        template <auto Method>
        static constexpr auto bind() {
            return [](void* self, auto... args) -> decltype(auto) {
                return (reinterpret_cast<Derived*>(self)->*Method)(args...);
            };
        }
    };

    template<typename Derived, Detail::FixedString nameTem, typename ServiceInterface>
    struct ServiceApiBase : public ModuleBase<Derived> {
    protected:
        static inline const std::string_view s_name = nameTem;
        static inline const Modular_ServiceId s_id = Modular::serviceNameToId(s_name);

        ServiceInterface m_interface;
    public:

        const char* name() { return s_name.data();  }
        Modular_ServiceId id() { return s_id; }
        ServiceInterface* interface() { return &m_interface; }
    };

    template<Detail::FixedString nameTem, typename... ServiceApis>
    struct PluginApiBase : public ModuleBase<PluginApiBase<nameTem, ServiceApis...>> {
    protected:
        static inline const std::string_view s_name = nameTem;

        Modular_PluginInterface m_interface;
        std::tuple<ServiceApis...> m_apis;

    public:

        PluginApiBase() {
            m_interface.name = this->template bind<&PluginApiBase::name>();
            m_interface.version = this->template bind<&PluginApiBase::version>();
            m_interface.onLoad = this->template bind<&PluginApiBase::onLoad>();
            m_interface.onUnload = this->template bind<&PluginApiBase::onUnload>();
            m_interface.self = this;
        };

        const char* name() { return s_name.data(); }

        virtual uint64_t version() = 0;

        virtual void onLoad(Modular_OnLoadHostInterface* host) {
            std::apply([host](auto&&... apis) {
                (host->registerService(host->self, apis.id(), apis.interface()), ...);
            }, m_apis);
            onLoadImpl(host);
        }

        virtual void onUnload() = 0;

        Modular_PluginInterface* interface() { return &m_interface; }

    protected:
        virtual void onLoadImpl(Modular_OnLoadHostInterface* host) = 0;
    };
}

#define MODULAR_DEFINE_SERVICE(Type, name)                                      \
namespace Modular {                                                             \
    template<>                                                                  \
    struct ServiceTraits<Type> : ServiceTraitBase<name> {};                     \
}
#endif