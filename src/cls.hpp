#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

#include <cls/greeter_client.usrv.pb.hpp>
#include <cls/greeter_service.usrv.pb.hpp>

#include <userver/ugrpc/client/client_factory_component.hpp>
#include <userver/ugrpc/server/server_component.hpp>
#include <userver/ugrpc/server/service_component_base.hpp>
#include <userver/components/component.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace cls {

/// [gRPC sample - client]
// A user-defined wrapper around api::GreeterServiceClient that provides
// a simplified interface.
class GreeterClient final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName = "greeter-client";

  GreeterClient(
      const userver::components::ComponentConfig& config,
      const userver::components::ComponentContext& context);

  std::string SayHello(std::string name);

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  userver::ugrpc::client::ClientFactory& client_factory_;
  cls::api::GreeterServiceClient client_;
};
/// [gRPC sample - client]

/// [gRPC sample - service]
class GreeterServiceComponent final
    : public cls::api::GreeterServiceBase::Component {
 public:
  static constexpr std::string_view kName = "greeter-service";

  GreeterServiceComponent(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context)
      : cls::api::GreeterServiceBase::Component(config, context),
        prefix_(config["greeting-prefix"].As<std::string>()) {}

  void SayHello(SayHelloCall& call, cls::api::GreetingRequest&& request) override;

  static userver::yaml_config::Schema GetStaticConfigSchema();

 private:
  const std::string prefix_;
};
/// [gRPC sample - service]


// Our Python tests use HTTP for all the samples, so we add an HTTP handler,
// through which we test both the client side and the server side.
class GreeterHttpHandler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "greeter-http-handler";

  GreeterHttpHandler(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context)
      : HttpHandlerBase(config, context),
        grpc_greeter_client_(context.FindComponent<GreeterClient>()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    return grpc_greeter_client_.SayHello(request.RequestBody());
  }

 private:
  GreeterClient& grpc_greeter_client_;
};

std::string SayHelloTo(std::string_view name);

} // namespace cls
