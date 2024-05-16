
#include "CounterServiceComponent.h"
#include "Cache.h"
#include "Counter.h"
#include "CounterTemplate.h"

#include <userver/yaml_config/merge_schemas.hpp>

#include <userver/storages/redis/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

template <class R, class C, class T>
void populate_counter(R const& request, C& counter, T& tmp);

template <class Req, class Template, class ResInfo>
bool is_invalid_request(Req const& request, Template const& tmp, ResInfo& resultInfo );

template <class Req, class Counter, class ResInfo>
bool is_invalid_counter(Req const& request, Counter const& tmp, ResInfo& resultInfo );

cls_gen::CreateCounterResponse create_counter(   cls_gen::CreateCounterRequest&& request_
                                                ,userver::storages::redis::ClientPtr& redis_client_
                                                ,userver::storages::redis::CommandControl redis_cc_
                                                ,cls_core::CounterTempateCache& cache_ )
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
            return reply_;
    }
    
    /*Algo 2 redis access*/
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - set <ping Hi>";
    std::string res = redis_client_->Ping(0, "Hi", redis_cc_).Get();
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check is redis online - get <"<< res <<"> - redis is online";
    if ("Hi" != res ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DatabaseError );
            LOG_WARNING() << "CreateCounter no Redis connection";
            return reply_;
    }

 /*Algo 3. check is clientRequestId unique */
    std::string clReqIdFul ("stLock:");
    std::string clReqId (std::to_string (request_.requestinfo().clientrequestid()) );
    clReqIdFul += clReqId;
    auto ret = redis_client_->Exists( clReqIdFul, redis_cc_).Get();
    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check if cliendReqiestId <" << clReqIdFul << "> is unique <" << !ret <<  ">"; 
    if ( 1 == ret){
            LOG_WARNING() << "CreateCounter got duplicate request with clientRequestId <" << clReqId << ">\n";
            std::cout << "CreateCounter got duplicate request with clientRequestId <" << clReqId << ">\n";
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DuplicateRequest );
            return reply_;
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
             return reply_;
    }

    LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Check domainCode is exists, domainCode=<" << request_.requestinfo().systemdomaincode() << ">";
    if ( request_.requestinfo().systemdomaincode() == cls_gen::DomainCode::Undef ){
            reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
            reply_.set_resultdetail ( cls_gen::RequestDetailStatus::DomainCodeNotFound );

            LOG_WARNING() << "CreateCounter wrong arguments, count less or equal than zero";
            return reply_;
    }
        /*Определяем какой шаблон использовать*/              
        
        auto tkey = CounterTemplate::crkey(request_.input().templateid(), request_.input().attrs().profileid(), 0);
        CounterTemplate ctemplate;
        if ( !cache_.find_template( tkey, ctemplate ) ) {
                LOG_WARNING() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << " Template " << CounterTemplate::key(request_.input().templateid(), request_.input().attrs().profileid(), 0) << " not found, exiting";
                reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
                reply_.set_resultdetail ( cls_gen::RequestDetailStatus::TemplateIdNotFound );
                return reply_;
        }    


        std::string key = "counter_id_seq"; //Eval counter ID
        /*Populate counter data*/
        size_t idx=0, good_req=0;
        for ( idx = 0; idx < request_.input().count(); ++idx ){
                auto resultInfo = reply_.add_resultinfo();
                if( is_invalid_request (request_, ctemplate, resultInfo) ) continue;

                auto id = co_await redis_ -> command<long long >("incr", key);
                Counter counter( id );
                LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" <<"new counter have redis_key: <" << counter.key() << ">";
                LOG_INFO() << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" <<"new counter have id: <" << id << ">";

                populate_counter(request_, counter, ctemplate );
                if( is_invalid_counter(request_, counter, resultInfo ) ) continue;

                hmap mm;
                counter.ToRd( mm );
                for (auto xmm: mm){
                LOG_TRACE << "CounterData " << xmm.first << " : " << xmm.second;
                }
                auto val = co_await redis_ -> hset(counter.key(), mm.begin(), mm.end() );
                LOG_TRACE << "HSET return " << val;
                /*Update classificator attr if it set*/
                for (int i =0; i<request_.input().attrs().classattrs_size(); ++i ){
                auto one = request_.input().attrs().classattrs( i );
                std::string _account("counterClassifier:{");
                _account += one.type();
                _account += ":";
                _account += one.value();
                _account += "}";
                co_await redis_ -> sadd ( _account, std::to_string( id ) );
                LOG_INFO << "Update counterClassifier: <" << _account << ">  add id: <" << id << ">";                
                }


                resultInfo->mutable_complinfo()->set_id( id );
                resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestSuccess );
                resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::Undefined );
                ++good_req;

        }
        if (0 != good_req and good_req  == idx ) reply_.set_result ( cls_gen::RequestStatus::RequestSuccess );
        else if (0 != idx and 0 == good_req ) reply_.set_result ( cls_gen::RequestStatus::RequestFailed );
        else reply_.set_result ( cls_gen::RequestStatus::RequestSuccessPartial );
        reply_.set_resultdetail ( cls_gen::RequestDetailStatus::Undefined );
        status_ = Status::FINISH;
        responder_.Finish(reply_, grpc::Status::OK, this);         
        LOG_INFO << "clientRequestId<" << request_.requestinfo().clientrequestid() << ">" << "Handle CreateCounter Done";

        

    return reply_;
}




template <class R, class C, class T>
void populate_counter(R const& request, C& counter, T& tmp){
    if ( request.input().attrs().has_activationdate() )
        counter.dateFrom = request.input().attrs().activationdate();
    else
        counter.dateFrom = time(0);


    counter.isPeriodic = false; 
    counter.dateTo = tmp.dateTo;
    if ( cls_gen::RecalculationPeriodType::NonPeriodic == tmp.periodType ){
        if ( request.input().attrs().has_deactivationdate() )
            counter.dateTo = request.input().attrs().deactivationdate();
    }
    else{
        counter.isPeriodic = true;   
        if ( request.input().attrs().has_deactivationdate() )
            counter.dateTo = request.input().attrs().deactivationdate( ); //MVP1 limitation
    }
    /*Fix counter Date*/
    if ( request.input().attrs().has_deactivationdate() and request.input().attrs().deactivationdate() == END_OF_TIMES_DATE and tmp.dateTo < counter.dateTo )
            counter.dateTo = tmp.dateTo;
    if ( request.input().attrs().has_deactivationdate() and request.input().attrs().deactivationdate() < counter.dateTo.ct() )
            counter.dateTo = request.input().attrs().deactivationdate();
    /*Fix dome*/

    counter.value = tmp.baseInitialValue;
    counter.initialValue = 0;
    if (counter.isPeriodic){
        counter.initialValue = tmp.baseInitialValue;
    }
    if (request.input().has_initialvalue()){
        long value = request.input().initialvalue().integer();
        int scale = request.input().initialvalue().scale();
        int template_scale = static_cast<int>(tmp.scale);
        counter.value = value * std::pow(10, template_scale-scale); 
        if (counter.isPeriodic){
            counter.initialValue = counter.value;
        }
    }
    counter.totalReservedAmount = 0;

    counter.modifyTS = time(0);
    counter.metaVersion = 0;


    std::vector<ClassificationAttr> class_attrs;
    for (int i =0; i<request.input().attrs().classattrs_size(); ++i ){
        auto one = request.input().attrs().classattrs( i );
        class_attrs.emplace_back( one.type(), one.value() );
    }
    counter.classificationAttrs = std::move( class_attrs ); 


    counter.domainCode = request.requestinfo().systemdomaincode();


    counter.templateVersion = tmp.templateVersion;
    counter.templateId = tmp.templateId;  
    counter.profileId = tmp.profileId;    
    counter.activationCode = request.input().attrs().activationcode().empty() ? "1" : request.input().attrs().activationcode();    //defect https://jira.mts.ru/browse/TPSUC-4525
    counter.passedPeriodCounter = 0;

    counter.startOfLifeDate = counter.isPeriodic ? request.input().attrs().activationdate() : 0; 
    counter.endOfLifeDate = counter.isPeriodic ? request.input().attrs().deactivationdate() : 0; 

    counter.nextDateFrom = 0;   
    counter.timeZone = request.input().attrs().timeshift(); /*Часовой пояс счетчика (timeZoneDict.id) - запросить из редиса вне этой фуункции*/

    counter.billingPeriod = 1; /*Биллинговый период счетчика (billingPeriodDict.id)*/
    counter.factors = "";
    counter.crmFactors = "";
    counter.flags = notSet;


    counter.occurOnReserve =tmp.occurOnReserve; /*Список связанных actionRule (actionRules.id), которые проверяются при выполнении резервации */
    counter.occurOnCommit = tmp.occurOnCommit; /*Список связанных actionRule (actionRules.id), которые проверяются при подтверждении резервации*/
    counter.occurOnEOS = tmp.occurOnEOS; /*Список связанных actionRule (actionRules.id), которые проверяются при закрытии сессии*/
    counter.occurOnGet = tmp.occurOnGet; /*Список связанных actionRule (actionRules.id), которые проверяются при обращении на чтение*/
    counter.occurOnLifeCycle = tmp.occurOnLifeCycle; /*Список связанных actionRule (actionRules.id), которые проверяются при создании, удалении, реинициализации*/
    counter.movedValue = 0;
    counter.IRPAamount = 0;   /*Значение коэффициента пересчета при смене ИРПА*/
    //counter.valueHistory = 0; //"value1, totalReservedAmount1, dateTo1, realModifyTime1; value2, totalReservedAmount2 dateTo2, realModifyTime2; … valueN, totalReservedAmountN, dateToN, realModifyTimeN"; /*История предыдущих периодов счетчиков (глубина задается в настройках CLS):*/

    
    std::vector<ExtAttr> extAttrs; 
    for (int i =0; i<request.input().attrs().extattrs_size(); ++i ){
        auto one = request.input().attrs().extattrs( i );
        //ExtAttr s { counter.dateFrom, counter.dateTo, one.name(), one.value() };
        ExtAttr s { counter.dateFrom, END_OF_TIMES_DATE, one.name(), one.value() };
        extAttrs.push_back( s );
    }
    counter.extAttrs = std::move( extAttrs );    /*Структура для хранения данных по внешним атрибутам: */

    //counter.autoRefill = "";//"dateFrom, dateTo, value, priority"; /*Структура для хранения данных по автопродлениям: */
    //counter.LTR = "";//"dateFrom, dateTo, value, priority, context";/*Структура для хранения данных long term reservations:*/

}

template <class Req, class Template, class ResInfo>
bool is_invalid_request(Req const& request, Template const& tmp, ResInfo& resultInfo ){
    auto attr = request.input().attrs();
    if(attr.activationdate() > attr.deactivationdate()){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::InvalidDeactivationDate );
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << "> Deactivation date cannot be earlier than activation date";
        return true;
    }

    if ( true == attr.classattrsnull() or 0 == attr.classattrs_size() ){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::ClassificationNotProvided );
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << "> Classification Not Provided";
        return true;       
    }
    if (attr.has_activationdate() and attr.activationdate() > tmp.dateTo.ct() ){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::CounterExpired );
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << std::format( "> Classification CounterExpired attr.activationdate() > template.dateTo {} > {}", attr.activationdate(), tmp.dateTo.ct() );
        return true;       
    }

    return false;
}

template <class Req, class Counter, class ResInfo>
bool is_invalid_counter(Req const& request, Counter const& counter, ResInfo& resultInfo ){
    auto attr = request.input().attrs();
    if (counter.dateTo < time(0)){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::CounterExpired );
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << std::format( "> Final calculated/assigned dateTo {} is in the past, less than current time {} ", counter.dateTo.ct(), time(0) );
        return true;
    }

    if (counter.dateTo < attr.activationdate() ){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::CounterExpired );
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << std::format( "> Final calculated/assigned dateTo {} is less than attrs.activationDate {}", counter.dateTo.ct(), attr.activationdate() );
        return true;        
    }

    /*Find TimeZoneId by TimeShift which stored in counter.timeZone defect    https://jira.mts.ru/browse/TPSUC-4526*/
    TimeZone tz;
    if (!Caches::timeZones().getByTimeShift(counter.timeZone, tz)){
        resultInfo->mutable_complinfo()->set_id( -1 );
        resultInfo->mutable_complinfo()->set_result( cls_gen::RequestStatus::RequestFailed );
        resultInfo->mutable_complinfo()->set_resultdetail( cls_gen::RequestDetailStatus::InvalidTimeZone );        
        LOG_ERROR() << "clientRequestId<" << request.requestinfo().clientrequestid() << "> RequestFailed InvalidTimeZone can`t find by timeShift: <" << counter.timeZone <<">";
        return true;
    }
    else{
        const_cast<Counter&>(counter).timeZone = tz.id;
    }
    return false;
}

