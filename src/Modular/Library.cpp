#include "Modular/Library.h"

namespace Modular {
#ifdef _WIN32
#include <windows.h>
    bool Library::load(std::filesystem::path path) {
        m_handle = LoadLibraryA(path.string().data());
        return m_handle != nullptr;
    }

    void Library::unload() {
        if (m_handle) {
            FreeLibrary((HMODULE)m_handle);
            m_handle = nullptr;
        }
    }

    void* Library::getSymbolRaw(std::string_view name) const {
        return (void*)GetProcAddress((HMODULE)m_handle, name.data());
    }
#else
#include <dlfcn.h>

    bool Library::load(std::filesystem::path path) {
        m_handle = dlopen(path.c_str(), RTLD_NOW);
        return m_handle != nullptr;
    }

    void Library::unload() {
        if (m_handle) {
            dlclose(m_handle);
            m_handle = nullptr;
        }
    }

    void* Library::getSymbolRaw(std::string_view name) const {
        return dlsym(m_handle, name.data());
    }
#endif

    Library::Library(std::filesystem::path path) {
        if (!load(path)) {
            throw std::runtime_error("Failed to load library: " + path.string());
        }
    }

    Library::~Library() {
        unload();
    }

    Library::Library(Library&& other) noexcept {
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }

    Library& Library::operator=(Library&& other) noexcept {
        if (this != &other) {
            unload();
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    bool Library::isLoaded() const {
        return m_handle != nullptr;
    }
}