#pragma once
#include <string>
#include <stdexcept>
#include <string_view>
#include <string>
#include <filesystem>

namespace Modular {

    class Library {
    private:
        void* m_handle = nullptr;

        #ifdef _WIN32
            static inline const std::string s_platformExtension = ".dll";
        #elif __APPLE__
            static inline const std::string s_platformExtension = ".dylib";
        #else
            static inline const std::string s_platformExtension = ".so";
        #endif

    public:
        Library() = default;
        explicit Library(std::filesystem::path path);

        ~Library();

        Library(const Library&) = delete;
        Library& operator=(const Library&) = delete;

        Library(Library&& other) noexcept;
        Library& operator=(Library&& other) noexcept;

        bool load(std::filesystem::path path);
        void unload();

        bool isLoaded() const;

        void* getSymbolRaw(std::string_view name) const;

        template<typename T>
        T getSymbol(std::string_view name) const {
            return reinterpret_cast<T>(getSymbolRaw(name));
        }

        static std::string_view getPlatformExtension() {
            return s_platformExtension;
        }
    };
}