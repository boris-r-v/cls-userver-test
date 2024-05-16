#include "Cache.h"


using namespace cls_core;

CounterTempateCache::CounterTempateCache(userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context)
    :CachingComponentBase<CounterTemplate>{config, context}
    ,redis_client_{ context.FindComponent<userver::components::Redis>("redis-database").GetClient("srv_db") }
    ,redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4}
{
    StartPeriodicUpdates();
}
CounterTempateCache::~CounterTempateCache(){
    StopPeriodicUpdates();
}

void CounterTempateCache::Update(   userver::cache::UpdateType type,
                                    [[maybe_unused]] std::chrono::system_clock::time_point const& last_update,
                                    [[maybe_unused]] std::chrono::system_clock::time_point const& now,
                                    userver::cache::UpdateStatisticsScope& stats_scope ) {

    map_t tmap;
    auto keys = redis_client_->Keys( "counterTemplate:*", 0, redis_cc_).Get();
    for (auto key: keys ){
        std::cout << "KEY: " << key  << std::endl;
        auto redis_template = redis_client_->Hgetall(key, redis_cc_ ).Get();

        CounterTemplate tmp;
        tmp.FromRd( redis_template );
        tmap.insert(std::make_pair( key, tmp ) );
    }

    OnCacheModified();
    stats_scope.Finish(tmap.size());
    tmap.swap(map_);    //FIX ME need something thread safe
    //Emplace ( map_ );
}
