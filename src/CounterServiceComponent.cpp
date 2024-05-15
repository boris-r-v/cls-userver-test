//
// Created by vadim on 18.04.24.
//

#include "CounterServiceComponent.h"

#include <userver/yaml_config/merge_schemas.hpp>

#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

CounterServiceComponent::CounterServiceComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : cls_gen::CounterRPCBase::Component::Component(config, context),
      prefix_(config["service-prefix"].As<std::string>())
      ,
      redis_client_(
          context.FindComponent<userver::components::Redis>("redis-database")
              .GetClient("main_db")),
      redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}

{}


void CounterServiceComponent::CreateCounter(    cls_gen::CounterRPCBase::CreateCounterCall& call,
                                                cls_gen::CreateCounterRequest&& request_) 
{

    grpc::ServerContext ctx_;

    cls_gen::CreateCounterResponse reply_;

    LOG_INFO << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Handle CreateCounter << Starting";
    reply_.mutable_responseinfo()->set_clientrequestid( request_.requestinfo().clientrequestid() );
    reply_.mutable_responseinfo()->set_traceid ( request_.requestinfo().context().traceid() );
    reply_.set_result ( cls_gen::RequestStatus::UnknownRequestStatus );
    reply_.set_resultdetail ( cls_gen::RequestDetailStatus::Undefined );

    /*Algo 1.a*/
    LOG_TRACE << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check clientRequestId (should be not 0): <" <<  request_.requestinfo().clientrequestid() << ">";
    if  (0 == request_.requestinfo().clientrequestid() ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::ClientRequestIdNotProvided );
            LOG_WARNING << "CreateCounter got request without clientRequestId";
            call.Finish(reply_);
            return;
    }
    
    /*Algo 2 redis access*/
    LOG_TRACE << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - set <ping Hi>";
    std::string res = co_await redis_ -> command<std::string>("ping", "Hi");
    LOG_TRACE << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - get <"<< res <<"> - redis is online";
    if ("Hi" != res ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DatabaseError );
            LOG_WARNING << "CreateCounter no Redis connection";
            call.Finish(reply_);
            return;

    }


}

userver::yaml_config::Schema CounterServiceComponent::GetStaticConfigSchema() {
  return userver::yaml_config::MergeSchemas<
  userver::ugrpc::server::ServiceComponentBase>(R"(
type: object
description: gRPC sample greater service component
additionalProperties: false
properties:
    service-prefix:
        type: string
        description: counter service prefix
)");
}
