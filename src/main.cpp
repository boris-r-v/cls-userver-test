#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>
#include <userver/ugrpc/server/server_component.hpp>
#include <userver/utils/daemon_run.hpp>

#include "CounterRPCClient.h"
#include "CounterRPCHttpHandler.h"
#include "CounterServiceComponent.h"

int main(int argc, char* argv[]) {
  const auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::ugrpc::client::ClientFactoryComponent>()
          .Append<userver::ugrpc::server::ServerComponent>()
          .Append<CounterServiceComponent>()
          .Append<CounterRPCClient>()
          .Append<CounterRPCHttpHandler>()
      ;
  return userver::utils::DaemonMain(argc, argv, component_list);
}
