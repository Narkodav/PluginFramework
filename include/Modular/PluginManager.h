#pragma once
#include "Modular/Plugin.h"

#include <vector>
#include <memory>
#include <filesystem>
#include <iostream>

namespace Modular {
    class PluginManager {
    private:
        std::vector<Plugin> m_plugins;
        Modular_OnLoadHostInterface* m_hostOnLoad = nullptr;

    public:

        bool loadPlugin(std::filesystem::path path) {
            Plugin plugin;
            if (!plugin.load(path, m_hostOnLoad)) return false;

            m_plugins.push_back(std::move(plugin));
            return true;
        }

        void loadPluginsInDirectory(std::filesystem::path dir) {
            for(auto const& dirEntry : std::filesystem::directory_iterator{dir}) {
                auto file = dirEntry.path();
                if(file.extension() == Library::getPlatformExtension()) {
                    if(!loadPlugin(file)) throw std::runtime_error("Plugin failed to load: " + file.string());
                }
            }
        }

        void shutdownAll() {
            m_plugins.clear();
        }

        void setHost(Modular_OnLoadHostInterface* hostOnLoad) {
            m_hostOnLoad = hostOnLoad;
        }
    };
}