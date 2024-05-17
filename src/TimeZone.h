#ifndef __TIMEZONE_H__
#define __TIMEZONE_H__

#include "types.h"

class TimeZone
{
public:
	TimeZone() = default;

	TimeZone(tzid id) : id(id) {}

	const static std::string KMASK;
	
	tzid id;
	int16_t timeZone;
	int32_t timeShift;
	clsdate	dateFrom;//	˜˜	˜˜˜˜ - ˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜˜˜˜ / ˜˜˜˜(UTC)	int64
	clsdate	dateTo;//	˜˜	˜˜˜˜ - ˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜˜˜˜ / ˜˜˜˜(UTC)	int64
	//std::string description;

	static std::string key(tzid id) {
		return std::format("{}{{{}}}", KMASK, id);
	}

	tzid key() {
		return id;
	}

	/*tzid rdkey(const std::string& rdkey) {
		if (rdkey.starts_with(KMASK)) {
			rdcast(rdkey.substr(KMASK.size()), id);
			return id;
		}
		else {
			LOG_ERROR << "Invalid timeZone identifier: " << rdkey;
			throw std::invalid_argument("Invalid timeZone identifier");
		}
	}*/

	void FromRd(const hmap& m) {
		RDFLD(m, id);
		RDFLD(m, timeZone);
		RDFLD(m, timeShift);
		RDFLD(m, dateFrom);
		RDFLD(m, dateTo);
	}
	//void ToRd(hmap& map) const;
};

/*
class ActivationCode {
public:
	uint32_t id;
	uint32_t activationExtCode;

	ActivationCode() = default;

	const static std::string KMASK;

	static std::string key(uint32_t id) {
		return std::format("{}:{}", KMASK, id);
	}

	uint32_t key() {
		return id;
	}

	void FromRd(const hmap& m) {
		RDFLD(m, activationExtCode);
	}

	uint32_t rdkey(const std::string& rdkey) {
		if (rdkey.starts_with(KMASK)) {
			rdcast(rdkey.substr(KMASK.size()), id);
		}
		else {
			LOG_ERROR << "Invalid activationCode identifier: " << rdkey;
			throw std::invalid_argument("Invalid activationCode identifier");
		}
	}
};

class ActCodeCache : public AbsCache<ActivationCode, uint32_t> {
	ActCodeCache() : AbsCache(ActivationCode::KMASK) {}

	friend class Caches;
};
*/

#endif // !__TIMEZONE_H__