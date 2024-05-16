//
// Created by vadim on 18.04.24.
//

#include "CounterServiceComponent.h"

#include <userver/yaml_config/merge_schemas.hpp>

#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>


cls_gen::CreateCounterResponse create_counter(  cls_gen::CreateCounterRequest&& request_,   
                                                userver::storages::redis::ClientPtr& redis_client_,
                                                userver::storages::redis::CommandControl redis_cc_ );

CounterServiceComponent::CounterServiceComponent( userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context)
    :cls_gen::CounterRPCBase::Component::Component(config, context)
    ,redis_client_{ context.FindComponent<userver::components::Redis>("redis-database").GetClient("main_db") }
    ,redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}
    //,tcache_ {context.FindComponent<cls_core::CounterTempateCache>("counter-template-cache").Get()}
    ,tcache_ {context.FindComponent<cls_core::CounterTempateCache>()}

{}


void CounterServiceComponent::CreateCounter(    cls_gen::CounterRPCBase::CreateCounterCall& call,
                                                cls_gen::CreateCounterRequest&& request_ ) 
{
    
    const auto tcache = tcache_.Get();
    const auto& template_map = tcache->map_;
    
    call.Finish( create_counter( std::move(request_), redis_client_, redis_cc_ ) );
}

