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

namespace cls_core{
/*
    struct CounterTemplate{
        CounterTemplate() = default;
        CounterTemplate( std::unordered_map<std::string, std::string>&& v): data_(v) {}
        
        std::string operator[](std::string const& key){return data_[key];}

        std::unordered_map<std::string, std::string>& map(){ return data_;}

        static std::string make_key(int a, int b, int c){
            std::string ret("counterTemplate:");
            ret.append( std::to_string(a));
            ret.append( ":" );
            ret.append( std::to_string(b));
            ret.append( ":" );
            ret.append( std::to_string(c));
            return ret;
        }

        private:
            std::unordered_map<std::string, std::string> data_;

    };
*/

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
 

} //cls_core