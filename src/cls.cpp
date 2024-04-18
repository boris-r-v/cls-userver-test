#include "cls.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

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

#include <cls/greeter_client.usrv.pb.hpp>
#include <cls/greeter_service.usrv.pb.hpp>

namespace cls {

void GreeterServiceComponent::SayHello(
    cls::api::GreeterServiceBase::SayHelloCall& call,
    cls::api::GreetingRequest&& request) {
  // Authentication checking could have gone here. For this example, we trust
  // the world.

  cls::api::GreetingResponse response;
  response.set_greeting(fmt::format("{}, {}!", prefix_, request.name()));

  // Complete the RPC by sending the response. The service should complete
  // each request by calling `Finish` or `FinishWithError`, otherwise the
  // client will receive an Internal Error (500) response.
  call.Finish(response);
}

/// [gRPC sample - client RPC handling]
std::string GreeterClient::SayHello(std::string name) {
  api::GreetingRequest request;
  request.set_name(std::move(name));

  // Deadline must be set manually for each RPC
  auto context = std::make_unique<grpc::ClientContext>();
  context->set_deadline(
      userver::engine::Deadline::FromDuration(std::chrono::seconds{20}));

  // Initiate the RPC. No actual actions have been taken thus far besides
  // preparing to send the request.
  auto stream = client_.SayHello(request, std::move(context));

  // Complete the unary RPC by sending the request and receiving the response.
  // The client should call `Finish` (in case of single response) or `Read`
  // until `false` (in case of response stream), otherwise the RPC will be
  // cancelled.
  api::GreetingResponse response = stream.Finish();

  return std::move(*response.mutable_greeting());
}
/// [gRPC sample - client RPC handling]

userver::yaml_config::Schema GreeterClient::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::components::LoggableComponentBase>(R"(
type: object
description: >
    a user-defined wrapper around api::GreeterServiceClient that provides
    a simplified interface.
additionalProperties: false
properties:
    endpoint:
        type: string
        description: >
            the service endpoint (URI). We talk to our own service,
            which is kind of pointless, but works for an example
)");

}

GreeterClient::GreeterClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context),
      // ClientFactory is used to create gRPC clients
      client_factory_(
          context.FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
              .GetFactory()),
      // The client needs a fixed endpoint
      client_(client_factory_.MakeClient<cls::api::GreeterServiceClient>(
          "greeter", config["endpoint"].As<std::string>())) {

}

userver::yaml_config::Schema GreeterServiceComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<userver::ugrpc::server::ServiceComponentBase>(R"(
type: object
description: gRPC sample greater service component
additionalProperties: false
properties:
    greeting-prefix:
        type: string
        description: greeting prefix
)");
}

namespace {

class Hello final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-hello";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    return cls::SayHelloTo(request.GetArg("name"));
  }
};

} // namespace

std::string SayHelloTo(std::string_view name) {
  if (name.empty()) {
    name = "unknown user";
  }

  return fmt::format("Hello, {}!\n", name);
}

void AppendHello(userver::components::ComponentList &component_list) {
  component_list.Append<Hello>();
}

} // namespace cls
