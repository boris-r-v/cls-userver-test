//
// Created by vadim on 19.04.24.
//

#include <userver/yaml_config/merge_schemas.hpp>

#include "CounterRPCClient.h"

CounterRPCClient::CounterRPCClient(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : LoggableComponentBase(config, context), // ClientFactory is used to create gRPC clients
      client_factory_(
          context
              .FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
              .GetFactory()),
      // The client needs a fixed endpoint
      client_(client_factory_.MakeClient<cls_gen::CounterRPCClient>(
          "counter-rpc-client", config["endpoint"].As<std::string>())) {}

userver::yaml_config::Schema CounterRPCClient::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<
      userver::ugrpc::server::ServiceComponentBase>(R"(
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

cls_gen::CreateCounterResponse CounterRPCClient::SayHello() {
  cls_gen::CreateCounterRequest request;
  request.mutable_input()->set_count(1);
  request.mutable_input()->set_templateid(1);
  request.mutable_requestinfo()->set_clientrequestid(0);
  request.mutable_requestinfo()->mutable_context()->set_priority(1);
  request.mutable_requestinfo()->mutable_context()->set_traceid("traceid");
  request.mutable_requestinfo()->set_systemdomaincode( cls_gen::DomainCode::Telecom );

  auto context = std::make_unique<grpc::ClientContext>();
  context->set_deadline(
      userver::engine::Deadline::FromDuration(std::chrono::seconds{20}));
  auto stream = client_.CreateCounter(request, std::move(context));
  cls_gen::CreateCounterResponse response = stream.Finish();
  return response;
}
