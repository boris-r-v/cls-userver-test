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
//      ,redis_client_(
//          context
//              .FindComponent<userver::components::Redis>("redis-database")
//              .GetClient("taxi-tmp")),
//      redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}

{}

enum class Status { CREATE, PROCESS, FINISH };

void CounterServiceComponent::CreateCounter(
    cls_gen::CounterRPCBase::CreateCounterCall& call,
    cls_gen::CreateCounterRequest&& request_) {
  Status status_ = Status::PROCESS;
  grpc::ServerContext ctx_;

  cls_gen::CreateCounterResponse reply_;
  LOG_TRACE() << "Handle CeateBalance << Starting";
  reply_.mutable_reponceinfo()->set_clientrequestid(
      request_.requestinfo().clientrequestid());
  reply_.mutable_reponceinfo()->set_traceid(
      request_.requestinfo().context().traceid());
  reply_.set_result(cls_gen::RequestStatus::UnknownRequestStatus);
  reply_.set_resultdetail(cls_gen::RequestDetailStatus::Undefined);

  /*Algo 1.a*/
  LOG_TRACE() << "Check clientRequestId (should be not 0): <"
              << request_.requestinfo().clientrequestid() << ">";
  if (0 == request_.requestinfo().clientrequestid()) {
    reply_.set_result(cls_gen::RequestStatus::RequestFailed);
    reply_.set_resultdetail(
        cls_gen::RequestDetailStatus::ClientRequestIdNotProvided);
    status_ = Status::FINISH;
    call.Finish(reply_);
  }

  //  if (status_ == Status::PROCESS) {
  //    cls_gen::CreateCounterResponse reply_;
  //    LOG_TRACE() << "Handle CeateBalance << Starting";
  //    reply_.mutable_reponceinfo()->set_clientrequestid(
  //        request_.requestinfo().clientrequestid());
  //    reply_.mutable_reponceinfo()->set_traceid(
  //        request_.requestinfo().context().traceid());
  //    reply_.set_result(cls_gen::RequestStatus::UnknownRequestStatus);
  //    reply_.set_resultdetail(cls_gen::RequestDetailStatus::Undefined);
  //
  //    /*Algo 1.a*/
  //    LOG_TRACE() << "Check clientRequestId (should be not 0): <"
  //              << request_.requestinfo().clientrequestid() << ">";
  //    if (0 == request_.requestinfo().clientrequestid()) {
  //      reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(
  //          cls_gen::RequestDetailStatus::ClientRequestIdNotProvided);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //      LOG_WARNING() << "CreateCounter got request without clientRequestId";
  ////      co_return;
  //    }
  //
  //    /*Algo 1.b.c - come leater*/
  //
  //    /*Algo 2 redis access*/
  //    LOG_TRACE() << "Check is redis online - set <ping Hi>";
  //    std::string res = co_await redis_->command<std::string>("ping", "Hi");
  //    LOG_TRACE() << "Check is redis online - get <" << res
  //              << "> - redis is online";
  //    if ("Hi" != res) {
  //      reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(cls_gen::RequestDetailStatus::DatabaseError);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //      LOG_WARNING() << "CreateCounter no Redis connection";
  ////      co_return;
  //    }
  //    /*Algo 3. проверка уникальности clientRequestId*/
  //    /*                std::string clReqIdFul ("stLock:");
  //                std::string clReqId (std::to_string
  //       (request_.requestinfo().clientrequestid()) ); clReqIdFul += clReqId;
  //       auto ret = co_await redis_ -> command<long long>("EXISTS", clReqIdFul
  //       );
  //                LOG_TRACE << "Check if cliendReqiestId <" << clReqIdFul <<
  //                "> is
  //       unique <" << std::boolalpha << !ret <<  ">"; if ( 1 == ret){
  //                        reply_.set_result (
  //       cls_gen::RequestStatus::RequestFailed ); reply_.set_resultdetail (
  //       cls_gen::RequestDetailStatus::DuplicateRequest ); status_ =
  //       Status::FINISH; responder_.Finish(reply_, grpc::Status::OK, this);
  //                        LOG_WARNING << "CreateCounter got duplicate request
  //                        with
  //       clientRequestId <" << clReqId << ">\n"; co_return;
  //                }
  //                else{
  //                        std::unordered_map<std::string, std::string> map
  //       {{"executeDate",std::to_string(time(0))}, {"operationCode", "none"},
  //       {"operationId", "none"}}; co_await redis_ -> hset(clReqIdFul,
  //       map.begin(), map.end());
  //                }
  //    */
  //    /*Algo 4.b*/
  //    LOG_TRACE() << "Check number of counters to create (should be non zero)
  //    "
  //              << request_.input().count();
  //    if (0 >= request_.input().count()) {
  //      reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(cls_gen::RequestDetailStatus::InvalidArguments);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //      LOG_WARNING()
  //          << "CreateCounter wrong arguments, count less or equal than zero";
  ////      co_return;
  //    }
  //    /*Algo 4.c*/
  //    LOG_TRACE() << "Check domainCode is exists, domainCode="
  //              << request_.input().domaincode();
  //    if (request_.input().domaincode().empty()) {
  //      reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(cls_gen::RequestDetailStatus::DomainCodeNotFound);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //      LOG_WARNING()
  //          << "CreateCounter wrong arguments, count less or equal than zero";
  ////      co_return;
  //    }
  //
  //    /*Оперделяем какой шаблон использовать*/
  //    std::string key("counterTemplate:{");
  //    key += std::to_string(request_.input().templateid());
  //    key += "}:0:0";
  //
  //    LOG_TRACE() << "cls_bl::CreateBalance::Proceed() " << key;
  //    auto counter_template =
  //        co_await redis_->hgetall<std::unordered_map<std::string,
  //        std::string>>(
  //            key);
  //    for (const auto& ele : counter_template)
  //      LOG_TRACE() << " CounterTemplateField: <" << ele.first << ">\t<"
  //                << ele.second << ">";
  //
  //    if (counter_template.empty()) {
  //      LOG_WARNING() << "Template " << key << " not found, exiting";
  //      reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(
  //          cls_gen::RequestDetailStatus::CounterTemplateNotFound);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  ////      co_return;
  //    }
  //
  //    /*Populate counter data*/
  //
  //    try {
  //      for (int i = 0; i < request_.input().count(); ++i) {
  //        std::unordered_map<std::string, std::string> counter;
  //        populate_counter(request_, counter, counter_template);
  //        key = "counter_id_seq";
  //        /*Eval counter ID*/
  //        co_await redis_->command<long long>("incr", key);
  //        std::string id = co_await redis_->command<std::string>("get", key);
  //        std::string counter_id = "counter:{" + id + "}";
  //        LOG_TRACE() << "new counter have redis_key: <" << counter_id << ">";
  //        LOG_TRACE() << "new counter have id: <" << id << ">";
  //
  //        auto val =
  //            co_await redis_->hset(counter_id, counter.begin(),
  //            counter.end());
  //        auto resultInfo = reply_.add_resultinfo();
  //        resultInfo->mutable_complinfo()->set_id(stoi(id));
  //        resultInfo->mutable_complinfo()->set_result(
  //            cls_gen::RequestStatus::RequestSuccess);
  //        resultInfo->mutable_complinfo()->set_resultdetail(
  //            cls_gen::RequestDetailStatus::Undefined);
  //        auto event = resultInfo->add_events();
  //        event->set_counterid(stoi(id));
  //        event->set_expression("Make it amaizing");
  //      }
  //
  //      reply_.set_result(cls_gen::RequestStatus::RequestSuccess);
  //      reply_.set_resultdetail(cls_gen::RequestDetailStatus::Undefined);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //    } catch (std::exception& ex) {
  //      LOG_ERROR() << "Exception thow in populate_counter <" << ex.what() <<
  //      ">"; reply_.set_result(cls_gen::RequestStatus::RequestFailed);
  //      reply_.set_resultdetail(cls_gen::RequestDetailStatus::InvalidArguments);
  //      status_ = Status::FINISH;
  //      responder_.Finish(reply_, grpc::Status::OK, this);
  //    }
  //  }
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
