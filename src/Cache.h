#pragma once

#include <unordered_map>
#include <CLS_service.usrv.pb.hpp>

#include <userver/cache/caching_component_base.hpp>
#include <userver/cache/cache_statistics.hpp>

#include <userver/components/component.hpp>
#include <userver/storages/redis/client.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>

#include "CounterTemplate.h"
#include "TimeZone.h"

namespace cls_core{
    class CounterTempateCache: public userver::components::CachingComponentBase<CounterTemplate>{
        public:
            constexpr static const std::string_view kName = "counter-template-cache";
            CounterTempateCache(userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context);
            ~CounterTempateCache();

            CounterTemplate operator[](std::string const& key){return map_[key];}

            bool find_template(std::string const& key, CounterTemplate& cntr ){
                auto fnd = map_.find(key);
                if (fnd == map_.end() ) return false;
                cntr = fnd->second;
                return true;
            }

        private:
            void Update(    userver::cache::UpdateType type,
                            std::chrono::system_clock::time_point const& last_update,
                            std::chrono::system_clock::time_point const& now,
                            userver::cache::UpdateStatisticsScope& stats_scope ) override;

            userver::storages::redis::ClientPtr redis_client_;
            userver::storages::redis::CommandControl redis_cc_;   

            typedef std::unordered_map<std::string, CounterTemplate> map_t;
            map_t map_;

    };
 
 class TimeZoneCache: public userver::components::CachingComponentBase<TimeZone>{
        public:
            constexpr static const std::string_view kName = "time-zone-cache";
            TimeZoneCache(userver::components::ComponentConfig const& config, userver::components::ComponentContext const& context);
            ~TimeZoneCache();

            bool find_zone(std::string const& key, TimeZone& cntr ){
                auto fnd = map_.find(key);
                if (fnd == map_.end() ) return false;
                cntr = fnd->second;
                return true;
            }
            bool find_zone_by_shift( int32_t timeShift, TimeZone& cntr ){
                for ( auto f : map_ ){
                    if ( f.second.timeShift == timeShift ){
                        cntr = f.second;
                        return true;
                    }
                }
                return false;
            }

        private:
            void Update(    userver::cache::UpdateType type,
                            std::chrono::system_clock::time_point const& last_update,
                            std::chrono::system_clock::time_point const& now,
                            userver::cache::UpdateStatisticsScope& stats_scope ) override;

            userver::storages::redis::ClientPtr redis_client_;
            userver::storages::redis::CommandControl redis_cc_;   

            typedef std::unordered_map<std::string, TimeZone> map_t;
            map_t map_;

    };
 

} //cls_core