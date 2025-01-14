//
// Created by vadim on 18.04.24.
//
#ifndef CLS_USERVER_COUNTERSERVICECOMPONENT_H
#define CLS_USERVER_COUNTERSERVICECOMPONENT_H

#include <CLS_service.usrv.pb.hpp>
#include "Cache.h"

#include <userver/components/component.hpp>

#include <userver/storages/redis/client.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>


class CounterServiceComponent final: public cls_gen::CounterRPCBase::Component {
  public:
    static constexpr std::string_view kName = "counter-service";

    CounterServiceComponent( userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context);

    void CreateCounter( cls_gen::CounterRPCBase::CreateCounterCall& call, cls_gen::CreateCounterRequest&& request) override;

  private:
    userver::storages::redis::ClientPtr redis_client_;
    userver::storages::redis::CommandControl redis_cc_;
   
    cls_core::CounterTempateCache& tcache_;
    cls_core::TimeZoneCache& zcache_;


    /*Inner Impl*/    
    cls_gen::CreateCounterResponse CreateCounterImpl( cls_gen::CreateCounterRequest&& request_ );

};

#endif  // CLS_USERVER_COUNTERSERVICECOMPONENT_H
