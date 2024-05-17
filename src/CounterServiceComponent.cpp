//
// Created by vadim on 18.04.24.
//

#include "CounterServiceComponent.h"

#include <userver/yaml_config/merge_schemas.hpp>

#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>


CounterServiceComponent::CounterServiceComponent( userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context)
    :cls_gen::CounterRPCBase::Component::Component(config, context)
    ,redis_client_{ context.FindComponent<userver::components::Redis>("redis-database").GetClient("main_db") }
    ,redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}
    ,tcache_ { context.FindComponent<cls_core::CounterTempateCache>("counter-template-cache") }
    ,zcache_ { context.FindComponent<cls_core::TimeZoneCache>("time-zone-cache") }
{

}


void CounterServiceComponent::CreateCounter(    cls_gen::CounterRPCBase::CreateCounterCall& call,
                                                cls_gen::CreateCounterRequest&& request_ ) 
{
    call.Finish( CreateCounterImpl( std::move(request_) ) );
}



/*
    ,tcache_ {context.FindComponent<cls_core::CounterTempateCache>()}
    const auto tcache = tcache_.Get();
    const auto& template_map = tcache->map_;
*/