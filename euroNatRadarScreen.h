#pragma once

#include <EuroScopePlugIn.h>
#include "NATData.h"

using namespace EuroScopePlugIn;

class euroNatRadarScreen :
	public EuroScopePlugIn::CRadarScreen
{
private:
	const NAT * m_pNat;
	const int * m_pNatCount;

	bool m_gotFont;

	CFont * m_pFont;
	CFont * m_pFontBold;
	LOGFONT * m_pLF;

	int m_showMouseover;

public:

	euroNatRadarScreen( NAT * pNat, int * pCount );
	virtual ~euroNatRadarScreen(void);

	inline virtual void OnRefresh( HDC hDC, int Phase );

	inline virtual void OnOverScreenObject( int ObjectType, const char * sObjectId, POINT Pt, RECT Area );

	inline virtual void OnAsrContentToBeSaved( void );
	inline virtual void OnAsrContentLoaded( bool Loaded );

	virtual void OnAsrContentToBeClosed( void ){
		delete this;
	}

};

