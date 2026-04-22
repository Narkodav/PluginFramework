#include "Modular/HostBase.h"
#include "Modular/PluginManager.h"

#include "TestService.h"

int main()
{
	Modular::HostBase host;
	Modular::PluginManager manager;
	manager.setHost(host.getInterfaceOnLoad());

	manager.loadPlugin("test_plugin.dll");

	auto* service = host.getService<TestServiceApi>();

	service->print(service->self, "Hello world");

	manager.shutdownAll();

	return 0;
}