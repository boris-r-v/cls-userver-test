//
// Created by vadim on 19.04.24.
//

#include <userver/components/component.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>
#include <userver/ugrpc/server/server_component.hpp>
#include <userver/ugrpc/server/service_component_base.hpp>

#include <CLS_client.usrv.pb.hpp>


#ifndef CLS_USERVER_TESTSERVICECOMPONENTCLIENT_H
#define CLS_USERVER_TESTSERVICECOMPONENTCLIENT_H

class CounterRPCClient final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "counter-rpc-client";

  CounterRPCClient(const userver::components::ComponentConfig& config,
                   const userver::components::ComponentContext& context);

  cls_gen::CreateCounterResponse SayHello();

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  userver::ugrpc::client::ClientFactory& client_factory_;
  cls_gen::CounterRPCClient client_;
};

#endif  // CLS_USERVER_TESTSERVICECOMPONENTCLIENT_H
