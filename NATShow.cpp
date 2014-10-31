#include "stdafx.h"
#include "NATShow.h"

bool NATShow::Eastbound		= true;
bool NATShow::Westbound		= true;
bool NATShow::Letters		= true;
bool NATShow::Lines			= true;
bool NATShow::DottedLines	= true;
bool NATShow::StaticInfo	= false;
bool NATShow::Loading		= false;
bool NATShow::ShortWPNames	= false;
bool NATShow::OutOfDate		= false;
bool NATShow::AntialiasedLine = false;
bool NATShow::ConcordTracks = false;
COLORREF NATShow::EastboundColor = RGB(115, 148, 148);
COLORREF NATShow::WestboundColor = RGB(115, 148, 115);
COLORREF NATShow::ConcordeColor = RGB(200, 100, 100);

void NATShow::Defaults(){
	NATShow::Eastbound		= true;
	NATShow::Westbound		= true;
	NATShow::Letters		= true;
	NATShow::Lines			= true;
	NATShow::DottedLines	= true;
	NATShow::StaticInfo		= false;
	NATShow::ShortWPNames	= false;
	NATShow::AntialiasedLine = false;
	NATShow::ConcordTracks	= false;
	NATShow::EastboundColor = RGB(115, 148, 148);
	NATShow::WestboundColor = RGB(115, 148, 115);
	NATShow::ConcordeColor  = RGB(200, 100, 100);
}

void NATShow::Save( EuroScopePlugIn::CPlugIn * pPlugin ){
	CString str;
	pPlugin->SaveDataToSettings( SETTING_EASTBOUND,		"Show Eastbound NATs",		NATShow::Eastbound ? "true" : "false"		);
	pPlugin->SaveDataToSettings( SETTING_WESTBOUND,		"Show Westbound NATs",		NATShow::Westbound ? "true" : "false"		);
	pPlugin->SaveDataToSettings( SETTING_LETTERS,		"Show NAT Letters",			NATShow::Letters ? "true" : "false"			);
	pPlugin->SaveDataToSettings( SETTING_LINES,			"Show NAT Lines",			NATShow::Lines ? "true" : "false"			);
	pPlugin->SaveDataToSettings( SETTING_DOTTEDLINES,	"Show Dotted Lines",		NATShow::DottedLines ? "true" : "false"		);
	pPlugin->SaveDataToSettings( SETTING_SHORTNAMES,	"Show Short WP Names",		NATShow::ShortWPNames ? "true" : "false"	);
	pPlugin->SaveDataToSettings( SETTING_ANTIALIASED,	"Show Antialiased Lines",	NATShow::AntialiasedLine ? "true" : "false" );
	pPlugin->SaveDataToSettings( SETTING_CONCORDE,		"Show Concorde Tracks",		NATShow::ConcordTracks ? "true" : "false"	);
	str.Format("%u", NATShow::EastboundColor);
	pPlugin->SaveDataToSettings( SETTING_EASTBOUND_COLOR, "Eastbound Track Color", str );
	str.Format("%u", NATShow::WestboundColor);
	pPlugin->SaveDataToSettings( SETTING_WESTBOUND_COLOR, "Westbound Track Color", str );
	str.Format("%u", NATShow::ConcordeColor);
	pPlugin->SaveDataToSettings( SETTING_CONCORDE_COLOR, "Concorde Track Color", str );
}

void NATShow::Load( EuroScopePlugIn::CPlugIn * pPlugin ){
	const char * str;
	str = pPlugin->GetDataFromSettings( SETTING_EASTBOUND );
	if( str != NULL )
		NATShow::Eastbound = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_WESTBOUND );
	if( str != NULL )
		NATShow::Westbound = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_LETTERS );
	if( str != NULL )
		NATShow::Letters = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_LINES );
	if( str != NULL )
		NATShow::Lines = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_DOTTEDLINES );
	if( str != NULL )
		NATShow::DottedLines = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_SHORTNAMES );
	if( str != NULL )
		NATShow::ShortWPNames = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_ANTIALIASED );
	if( str != NULL )
		NATShow::AntialiasedLine = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_CONCORDE );
	if( str != NULL )
		NATShow::ConcordTracks = ( str[0] == 't' );
	str = pPlugin->GetDataFromSettings( SETTING_EASTBOUND_COLOR );
	if( str != NULL )
		NATShow::EastboundColor = (unsigned int)atol( str );
	str = pPlugin->GetDataFromSettings( SETTING_WESTBOUND_COLOR );
	if( str != NULL )
		NATShow::WestboundColor = (unsigned int)atol( str );
	str = pPlugin->GetDataFromSettings( SETTING_CONCORDE_COLOR );
	if( str != NULL )
		NATShow::ConcordeColor = (unsigned int)atol( str );
}