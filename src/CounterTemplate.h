#ifndef __COUNTER_TEMPLATE_H__
#define __COUNTER_TEMPLATE_H__

#include <cstdint>
#include <format>
#include <functional>
#include <string>

#include <CLS.pb.h>
#include "types.h"


typedef std::tuple<tmplid_t,profileid_t,uint16_t> ctid_t;

class CounterTemplate {
public:
	//void push_back(std::pair<std::string, std::string> p) {}
	tmplid_t templateId; //Unique template id(RD.COUNTER_NUMBER)
	profileid_t profileId; //Profile Id.Switches daily and monthly PPD.
	uint16_t templateVersion; //cycle version number
	clsdate	dateFrom;
	clsdate dateTo;
	std::bitset<512> clearMask; //soc`s bitmask (0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
	bool applied;
	cntval_t moveLimitRatio;//Reinit limit(baseInitialValue + recalc factors), devide by scale
	cntval_t moveRatio;
	cntval_t baseInitialValue;
	Scale scale;

	std::string periodDaysList; // days for reinit
	std::string periodDelta;

	cls_gen::RecalculationPeriodType periodType;
	bool periodOnInstance;// Reinit period is on counter
	std::string extCode; //	Ext template code RD(RD.COUNTER_CODE)
	std::string extName; //	Template name RD(RD.COUNTER_DESCRIPTION)
	cls_gen::CounterSearchFlags extType; //CounterType?
	std::string metricUnitCode;
	std::vector<ruleid_t> occurOnReserve; //csv actionRule(actionRules.id) on reserve
	std::vector<ruleid_t> occurOnCommit; //actionRule(actionRules.id) on commit
	std::vector<ruleid_t> occurOnEOS; //actionRule(actionRules.id) on EndOfSession
	std::vector<ruleid_t> occurOnGet; //actionRule(actionRules.id) on Get
	std::vector<ruleid_t> occurOnLifeCycle; //actionRule(actionRules.id) on create/delete/reinit
	std::string tmpCounterConfig; //??? Alt params of base value and movement rules? //list of{ type, moveLimitRatio, moveRatio, initialValue }
	std::string factorClearMasks; // factors mask { activationCodeId, clearMask } to delete from counter
	//std::string templateFactor;// Factors on template for reinit : {extCode, value, type, position, expirationDate, activationCodeId} *������� ��� ������ ����� ��������� ������ - ��������� �� �������� "������ ���������� �������", � �������� ������� �� ������� ��������(���.������) ������� �����������.
	
	static std::string key(tmplid_t id, profileid_t profileId, uint32_t templateVersion) {
		return std::format("counterTemplate:{{{}}}:{}:{}", id, profileId, templateVersion);
	}
	ctid_t key() {
		return std::make_tuple(templateId, profileId, templateVersion);
	}

	static ctid_t make_key(tmplid_t templateId, profileid_t profileId, uint32_t templateVersion) {
		return std::make_tuple(templateId, profileId, templateVersion);
	}

	/*ctid_t rdkey(const std::string& rdkey) {
		static const std::regex crx("counterTemplate:\\{(\\d+)\\}:(\\d+):(\\d+)");
		std::smatch pieces_match;

		if (std::regex_match(rdkey, pieces_match, crx) && pieces_match.size() == 4)
		{
			rdcast(pieces_match[1].str(), templateId);
			rdcast(pieces_match[2].str(), profileId);
			rdcast(pieces_match[3].str(), templateVersion);
			return key();
		}
		else {
			LOG_ERROR << "Invalid counterTemplate identifier: " << rdkey;
			throw std::invalid_argument("Invalid counterTemplate identifier");
		}
	}*/

	CounterTemplate() = default;

	CounterTemplate(tmplid_t templateId, profileid_t profileId = 0, uint16_t templateVersion = 0) :
		templateId(templateId),
		profileId(profileId),
		templateVersion(templateVersion)
	{}

	void FromRd(const hmap& m);

	//void ToRd(hmap& map) const;
};


struct ct_hash //: public std::unary_function<ctid_t, std::size_t>
{
	std::size_t operator()(const ctid_t& k) const
	{
		return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
	}
};

#endif // !__COUNTER_TEMPLATE_H__