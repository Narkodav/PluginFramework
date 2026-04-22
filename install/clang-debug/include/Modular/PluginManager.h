#pragma once
#include "Modular/Plugin.h"

#include <vector>
#include <memory>
#include <filesystem>

namespace Modular {
    class PluginManager {
    private:
        std::vector<Plugin> m_plugins;
        Modular_OnLoadHostApi* m_hostOnLoad = nullptr;

    public:

        bool loadPlugin(std::filesystem::path path) {
            Plugin plugin;
            if (!plugin.load(path, m_hostOnLoad)) return false;

            m_plugins.push_back(std::move(plugin));
            return true;
        }

        void shutdownAll() {
            m_plugins.clear();
        }

        void setHost(Modular_OnLoadHostApi* hostOnLoad) {
            m_hostOnLoad = hostOnLoad;
        }
    };
}