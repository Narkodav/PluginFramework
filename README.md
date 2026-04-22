# PluginFramework

A C++ framework for loading dynamic plugins and composing modular applications through a service-oriented runtime interface.

## Overview

PluginFramework provides a minimal and ABI-stable foundation for building extensible applications using dynamically loaded plugins. It defines a clear boundary between host and plugins using a C-compatible interface, while allowing flexible internal architecture on both sides.

Plugins act as modular units that register services at load time. These services expose API surfaces that can be queried and used by the host or other plugins.

The framework is designed to be:

* cross-platform (platform-specific loading abstracted)
* ABI-stable across module boundaries
* minimal and non-opinionated about internal architecture
* suitable for engine-style modular systems

---

## Core Concepts

### Plugins

Dynamic libraries that implement a defined interface and are loaded at runtime. Plugins are responsible for registering services during their load phase.

### Services

Named API interfaces exposed by plugins. Services are the primary mechanism for cross-module interaction.

### Host

Owns the service registry and provides APIs for plugins to register and query services.

### Plugin Manager

Handles loading, initialization, and unloading of plugins.

---

## Architecture

Typical flow:

1. Load dynamic library
2. Retrieve plugin interface (`create_plugin`)
3. Call plugin load function
4. Plugin registers services
5. Host and other plugins query services by ID
6. Application uses service APIs
7. Shutdown → unload plugins

---

## Example Usage

A minimal usage pattern:

```cpp
Modular::HostBase host;
Modular::PluginManager manager;

manager.setHost(host.getInterfaceOnLoad());
manager.loadPlugin("test_plugin.dll");

auto* service = host.getService<TestServiceApi>();
service->print(service->self, "Hello world");

manager.shutdownAll();
```

---

## Service Definition

Services are defined through C-compatible API structs:

```cpp
extern "C" {
    struct TestServiceApi {
        void (*print)(void* self, const char* msg);
        void* self;
    };
}
```

Each service provides its identity through `ServiceTraits`:

```cpp
template<>
struct ServiceTraits<TestServiceApi>
    : ServiceTraitBase<"TestService"> {};
```

or:

```cpp
MODULAR_DEFINE_SERVICE(TestServiceApi, "TestService")
```

---

## Threading Model

* Multiple threads may access services concurrently
* Services are responsible for their own thread safety
* Plugin loading/unloading must be externally synchronized
* Shutdown requires all service usage to be stopped before unloading

---

## Build

The project uses CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Examples

Usage examples and test plugins are available in the **`tests/`** directory.

---

## Design Notes

* Uses a C ABI for plugin boundaries to ensure stability across compilers and modules
* Avoids RTTI and C++ object sharing across DLL boundaries
* Service lookup is based on compile-time traits mapped to runtime identifiers
* Plugins are treated as singletons; services act as API surfaces
