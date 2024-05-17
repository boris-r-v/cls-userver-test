#ifndef __COUNTER_H__
#define __COUNTER_H__

#include <CLS.pb.h>
#include "types.h"
const int64_t END_OF_TIMES_DATE(2145916800);

/*template <>
inline void to_rds(std::stringstream& ss, const uint32_t& t) {
	ss << t;
}*/


template <>
inline std::string to_rdstr(const cls_gen::DomainCode& t) {
	return std::to_string(t);
}

template <>
inline void rdcast(const std::string_view& s, cls_gen::DomainCode& t) {
	int i;
	rdcast(s, i);
	t = static_cast<cls_gen::DomainCode>(i);
}


// Counter flags
enum CntFlag {
	notSet = 0,
	isSuspended = 1,
	isForceDeleted = 2,// deleted by DeleteCounter
	isReinitBlocked = 3,// restrict reinit
	isValueTransferBlocked = 4// restrict move
};

template <>
inline void rdcast(const std::string_view& s, CntFlag& t) {
	int i;
	rdcast(s, i);
	t = static_cast<CntFlag>(i);
}

template <>
inline std::string to_rdstr(const CntFlag& t) {
	return std::to_string(t);
}


class ClassificationAttr {
public:
	ClassificationAttr() = default;

	ClassificationAttr(std::string atrName, std::string atrValue) : atrName(atrName), atrValue(atrValue) {}

	std::string atrName, atrValue;

	inline void to_rds(std::stringstream& ss/*, const ClassificationAttr& t*/) const {
		ss << atrName << ',' << atrValue;
	}
};



template <>
inline void rdcast(const std::string_view& s, ClassificationAttr& t) {
	svtok st(s);
	rdcast(st.next(), t.atrName);
	rdcast(st.next(), t.atrValue);
}


class ExtAttr {
public:
	clsdate dateFrom;
	clsdate dateTo;
	std::string key;// uint32_t key;
	std::string value;// uint32_t value;//?
	
	inline void to_rds(std::stringstream& ss/*, const ExtAttr& t*/) const {
		ss << dateFrom << ',' << dateTo << ',' << key << ',' << value;
	}
};


template <>
inline void rdcast(const std::string_view& s, ExtAttr& t) {
	svtok st(s);
	rdcast(st.next(), t.dateFrom);
	rdcast(st.next(), t.dateTo);
	rdcast(st.next(), t.key);
	rdcast(st.next(), t.value);
}

class AutoRefill {
public:
	clsdate dateFrom;
	clsdate dateTo;
	uint16_t priority; // enum?
	cntval_t value;

	AutoRefill() = default;
	AutoRefill(clsdate dateFrom, clsdate dateTo, uint16_t priority, cntval_t value)
		:dateFrom(dateFrom),dateTo(dateTo),priority(priority), value(value) {}

	inline void to_rds(std::stringstream& ss/*, const AutoRefill& t*/) const {
		ss << dateFrom << ',' << dateTo << ',' << priority << ',' << value;
	}
};

template <>
inline void rdcast(const std::string_view& s, AutoRefill& t) {
	svtok st(s);
	rdcast(st.next(), t.dateFrom);
	rdcast(st.next(), t.dateTo);
	rdcast(st.next(), t.priority);
	rdcast(st.next(), t.value);
}

class LTR_t {
public:
	clsdate dateFrom;
	clsdate dateTo;
	uint16_t priority; // enum?
	cntval_t value;
	uint32_t context;// type?

	LTR_t() = default;

	LTR_t(clsdate dateFrom, clsdate dateTo, uint16_t priority, cntval_t value, uint32_t context)
		: dateFrom(dateFrom), dateTo(dateTo), priority(priority), value(), context(context) {}

	inline void to_rds(std::stringstream& ss/*, const LTR_t& t*/) const {
		ss << dateFrom << ',' << dateTo << ',' << priority << ',' << value << ',' << context;
	}
};

template <>
inline void rdcast(const std::string_view& s, LTR_t& t) {
	svtok st(s);
	rdcast(st.next(), t.dateFrom);
	rdcast(st.next(), t.dateTo);
	rdcast(st.next(), t.priority);
	rdcast(st.next(), t.context);
}

class ValueHist {
public:
	cntval_t value;
	cntval_t totalReservedAmount;
	clsdate dateTo;
	clsdate realModifyTime;

	inline void to_rds(std::stringstream& ss) const {
		ss << value << ',' << totalReservedAmount << ',' << dateTo << ',' << realModifyTime;
	}
};

template <>
inline void rdcast(const std::string_view& s, ValueHist& t) {
	svtok st(s);
	rdcast(st.next(), t.value);
	rdcast(st.next(), t.totalReservedAmount);
	rdcast(st.next(), t.dateTo);
	rdcast(st.next(), t.realModifyTime);
}

class Counter
{
public:
	Counter() = default;
	Counter(cntid_t id) : id(id) {}

	cntid_t id;
	clsdate dateFrom;
	clsdate dateTo;
	cntval_t value;
	cntval_t totalReservedAmount; // TRA(total reserved amount)
	clsdate	modifyTS; // counter value last modification timestamp, lastValueModificationDate
	uint16_t metaVersion;//	Counter of changes in thresholds, external attributes, action rules
	std::vector<ClassificationAttr> classificationAttrs; // comma - separated list of{ attrName, attrValue }, e.g. {"personalAccountId", "12345"}, { "terminalDeviceId", "4321" }
	cls_gen::DomainCode	domainCode;
	bool isPeriodic;
	tmplid_t	templateId;//	counterTemplate.id
	int16_t	templateVersion;//	counterTemplate.templateVersion
	profileid_t	profileId; // counterTemplate.profileId. Default : 0 (base), alt: 1.

	std::string activationCode;//	activationCode - string from protobuf
	uint32_t passedPeriodCounter;//	number of re - init periods
	clsdate	startOfLifeDate;//	date of counter first period
	clsdate	endOfLifeDate;//	an individual endOfLifeDate of periodic counter
	clsdate	nextDateFrom;//	Reference date for the counter re - initialization
	tzid timeZone;//timeZoneDict.id
	billprdid_t billingPeriod; //billingPeriodDict.id
	std::string factors;	// BorisR change. comma - separated list of{ factorBitPosition, expirationDate, options }
	std::string	crmFactors;	//BorisR change. comma - separated list of{ crmRatio or crmBonus, scale }, where crmRatio or crmBonus - integer part, scale - position of the decimal point
	CntFlag	flags;
	std::vector<ruleid_t> occurOnReserve; // csv-list actionRules.id
	std::vector<ruleid_t> occurOnCommit; // csv-list actionRules.id
	std::vector<ruleid_t> occurOnEOS; // csv-list  actionRules.id
	std::vector<ruleid_t> occurOnGet; // csv-list  actionRules.id
	std::vector<ruleid_t> occurOnLifeCycle; // csv-list  actionRules.id
	cntval_t initialValue; // Value for the start of last period
	cntval_t movedValue; // Counter's value moved to new period
	cntval_t IRPAamount; //	Integer part of the IRPA coefficient, according TL_Data.scale
	std::vector<ValueHist> valueHistory; //	value1, totalReservedAmount1, dateTo1, realModifyTime1; value2, totalReservedAmount2 dateTo2, realModifyTime2; �; valueN, totalReservedAmountN, dateToN, realModifyTimeN
	//uint32_t numExtAttrs;//	number of external attributes for the counter
	std::vector<ExtAttr> extAttrs;
	//uint32_t numAutoRefills;//	number of tmpCounters with type "Autorefill"
	std::vector<AutoRefill> autoRefills; // autoRefill1{ dateFrom, dateTo, value, priority1 }
	//uint32_t numLTR;//	number of tmpCounters with type "LTR"
	std::vector<LTR_t> LTR; // LTR1{ dateFrom, dateTo, value, priority1, context }
	std::string key() const { return std::format("counter:{{{}}}", id); }


	static std::string key(cntid_t id) { return std::format("counter:{{{}}}", id); }
	void FromRd(const hmap& m);
	void ToRd(hmap& map) const;
	void ToRd(hvector& map) const;
};

#endif // !__COUNTER_H__