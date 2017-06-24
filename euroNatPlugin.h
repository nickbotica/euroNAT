#pragma once

#include <EuroScopePlugIn.h>
#include "NATData.h"
#include "euroNatRadarScreen.h"
#include "euroNatOptions.h"
#include <string>

using namespace EuroScopePlugIn;

class euroNatPlugin : public EuroScopePlugIn::CPlugIn
{
protected:
	NATData natData;

	CArray<euroNatRadarScreen *> m_RadarScreenList;
	
	NAT * m_nats;
	int * m_natcount;

	euroNatOptions m_guiopts;

public:
	euroNatPlugin(void);
	void CheckVersion(void);
	virtual ~euroNatPlugin(void);

	inline virtual bool OnCompileCommand( const char * sCommandLine );

	inline virtual CRadarScreen * OnRadarScreenCreated(
		const char * sDisplayName, 
		bool NeedRadarContent, 
		bool GeoReferenced, 
		bool CanBeSaved, 
		bool CanBeCreated
	);

};

