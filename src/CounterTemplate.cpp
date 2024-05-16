#include <string>
#include "CounterTemplate.h"

void CounterTemplate::FromRd(const hmap& m) {
	RDFLD(m, templateId);
	RDFLD(m, profileId);
	RDFLD(m, dateFrom);
	RDFLD(m, dateTo);
	RDFLD(m, templateVersion);
	RDFLD(m, clearMask);
	RDFLD(m, applied);
	RDFLD(m, moveLimitRatio);
	RDFLD(m, moveRatio);
	RDFLD(m, baseInitialValue);
	RDFLD(m, scale);
	RDFLD(m, periodDaysList);
	RDFLD(m, periodDelta);	
	RDFLD(m, periodType);
	RDFLD(m, periodOnInstance);	
	RDFLD(m, extCode);
	RDFLD(m, extName);
	RDFLD(m, extType);
	RDFLD(m, metricUnitCode );

	RDFLD(m, occurOnReserve );
	RDFLD(m, occurOnCommit );
	RDFLD(m, occurOnEOS );
	RDFLD(m, occurOnGet );
	RDFLD(m, occurOnLifeCycle );
	
	RDFLD(m, tmpCounterConfig );
	RDFLD(m, factorClearMasks);


}