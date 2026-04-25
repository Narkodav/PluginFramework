#include "mod.h"

#include <iostream>

static TestPlugin plugin;

extern "C"
{
    MODULAR_PLUGIN_API Modular_PluginInterface* Modular_createPlugin() {
        std::cout << "Plugin created" << std::endl;
        return plugin.getApiPointer();
    }

    MODULAR_PLUGIN_API void Modular_destroyPlugin(Modular_PluginInterface*) {
        std::cout << "Plugin destroyed" << std::endl;
    }
}