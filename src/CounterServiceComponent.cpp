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
      prefix_(config["service-prefix"].As<std::string>()),
      redis_client_{ context.FindComponent<userver::components::Redis>("redis-database").GetClient("main_db") },
      redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}

{}


void CounterServiceComponent::CreateCounter(    cls_gen::CounterRPCBase::CreateCounterCall& call,
                                                cls_gen::CreateCounterRequest&& request_) 
{
    cls_gen::CreateCounterResponse reply_;

    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Handle CreateCounter << Starting";
    reply_.mutable_responseinfo()->set_clientrequestid( request_.requestinfo().clientrequestid() );
    reply_.mutable_responseinfo()->set_traceid ( request_.requestinfo().context().traceid() );
    reply_.set_result ( cls_gen::RequestStatus::RequestSuccess );
    reply_.set_resultdetail ( cls_gen::RequestDetailStatus::Undefined );

    /*Algo 1.a*/
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check clientRequestId (should be not 0): <" <<  request_.requestinfo().clientrequestid() << ">";
    if  (0 == request_.requestinfo().clientrequestid() ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::ClientRequestIdNotProvided );
            LOG_WARNING() << "CreateCounter got request without clientRequestId";
            call.Finish(reply_);
            return;
    }
    
    /*Algo 2 redis access*/
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - set <ping Hi>";
    std::string res = redis_client_->Ping(0, "Hi", redis_cc_).Get();
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - get <"<< res <<"> - redis is online";
    if ("Hi" != res ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DatabaseError );
            LOG_WARNING() << "CreateCounter no Redis connection";
            call.Finish(reply_);
            return;

    }

 /*Algo 3. check is clientRequestId unique */
    std::string clReqIdFul ("stLock:");
    std::string clReqId (std::to_string (request_.requestinfo().clientrequestid()) );
    clReqIdFul += clReqId;
    auto ret = redis_client_->Exists( clReqIdFul, redis_cc_).Get();
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check if cliendReqiestId <" << clReqIdFul << "> is unique <" << !ret <<  ">"; 
    if ( 1 == ret){
            LOG_WARNING() << "CreateCounter got duplicate request with clientRequestId <" << clReqId << ">\n";
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DuplicateRequest );
            call.Finish(reply_);
            return;
    }
    else{
            std::vector<std::pair<std::string, std::string>> vectr {{"executeDate",std::to_string(time(0))}, {"operationCode", "none"}, {"operationId", "none"}};
            redis_client_-> Hmset(clReqIdFul, vectr, redis_cc_ ).IgnoreResult();
    }


    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check number of counters to create (should be non zero) <" << request_.input().count() << ">";
    if ( 0 >= request_.input().count() ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::InvalidArguments );
            LOG_WARNING() << "CreateCounter wrong arguments, count less or equal than zero";
            call.Finish(reply_);                                    
            return;
    }

    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check domainCode is exists, domainCode=<" << request_.requestinfo().systemdomaincode() << ">";
    if ( request_.requestinfo().systemdomaincode() == cls_gen::DomainCode::Undef ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DomainCodeNotFound );

            LOG_WARNING() << "CreateCounter wrong arguments, count less or equal than zero";
            call.Finish(reply_);            
            return;
    }




    call.Finish(reply_);
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
