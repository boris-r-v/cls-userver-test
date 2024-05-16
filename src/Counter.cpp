#include <string>

#include "Counter.h"

void Counter::FromRd(const hmap& m) {
	RDFLD(m, id);
	RDFLD(m, dateFrom);
	RDFLD(m, dateTo);
	RDFLD(m, value);
	RDFLD(m, totalReservedAmount);
	RDFLD(m, modifyTS);
	RDFLD(m, metaVersion);
	RDFLDV(m, classificationAttrs);
	RDFLD(m, domainCode);
	RDFLD(m, isPeriodic);
	RDFLD(m, templateId);
	RDFLD(m, templateVersion);
	RDFLD(m, profileId);
	RDFLD(m, templateVersion);
	RDFLD(m, activationCode);
	RDFLD(m, passedPeriodCounter);
	RDFLD(m, startOfLifeDate);
	RDFLD(m, endOfLifeDate);
	RDFLD(m, nextDateFrom);
	RDFLD(m, timeZone);
	RDFLD(m, billingPeriod);

	RDFLD(m, flags);
	RDFLDV(m, occurOnReserve);
	RDFLDV(m, occurOnCommit);
	RDFLDV(m, occurOnEOS);
	RDFLDV(m, occurOnGet);
	RDFLDV(m, occurOnLifeCycle);
	RDFLD(m, initialValue);
	RDFLD(m, movedValue);
	RDFLD(m, IRPAamount);
	
	RDFLDV(m, valueHistory);
	
	//RDFLD(m, numExtAttrs);
	RDFLDV(m, extAttrs);

	//RDFLD(m, numAutoRefills);
	RDFLDV(m, autoRefills);

	//RDFLD(m, numLTR);
	RDFLDV(m, LTR);
}

void Counter::ToRd(hmap& m) const {
	//m.clear();
	TORD(m, id);
	TORD(m, dateFrom);
	TORD(m, dateTo);
	TORD(m, value);
	TORD(m, initialValue);
	TORD(m, templateId);
	TORD(m, templateVersion);

	TORD(m, totalReservedAmount);
	TORD(m, modifyTS);
	TORD(m, metaVersion);
	TORDV(m, classificationAttrs);
	TORD(m, domainCode);
	TORD(m, isPeriodic);
	TORD(m, profileId);
	TORD(m, activationCode);
	TORD(m, passedPeriodCounter);
	TORD(m, startOfLifeDate);
	TORD(m, endOfLifeDate);
	TORD(m, nextDateFrom);
	TORD(m, timeZone);
	TORD(m, billingPeriod);

	TORD(m, flags);
	TORDV(m, occurOnReserve);
	TORDV(m, occurOnCommit);
	TORDV(m, occurOnEOS);
	TORDV(m, occurOnGet);
	TORDV(m, occurOnLifeCycle);
	TORD(m, initialValue);
	TORD(m, movedValue);
	TORD(m, IRPAamount);

	TORDV(m, valueHistory);

	//TORD(m, numExtAttrs);
	TORDV(m, extAttrs);

	//TORD(m, numAutoRefills);
	TORDV(m, autoRefills);

	//TORD(m, numLTR);
	TORDV(m, LTR);
}

