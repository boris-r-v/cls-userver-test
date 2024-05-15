//
// Created by vadim on 19.04.24.
//

#ifndef CLS_USERVER_COUNTERRPCHTTPHANDLER_H
#define CLS_USERVER_COUNTERRPCHTTPHANDLER_H

#include <userver/components/component.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>
#include <userver/ugrpc/server/server_component.hpp>
#include <userver/ugrpc/server/service_component_base.hpp>

#include <CLS_client.usrv.pb.hpp>
#include "CounterRPCClient.h"

class CounterRPCHttpHandler : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "counter-rpc-client-http-handler";

  CounterRPCHttpHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context)
      : HttpHandlerBase(config, context),
        grpc_greeter_client_(context.FindComponent<CounterRPCClient>()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
   // grpc_greeter_client_.SayHello();
    return "";
  }

 private:
  CounterRPCClient & grpc_greeter_client_;
};

#endif  // CLS_USERVER_COUNTERRPCHTTPHANDLER_H
