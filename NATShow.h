#include <EuroScopePlugIn.h>

#define SETTING_EASTBOUND	"ShowEastbound"
#define SETTING_WESTBOUND	"ShowWestbound"
#define SETTING_LETTERS		"ShowLetters"
#define SETTING_LINES		"ShowLines"
#define SETTING_DOTTEDLINES "ShowDottedLines"
#define SETTING_SHORTNAMES	"ShowShortNames"
#define SETTING_ANTIALIASED "AntialiasedLines"
#define SETTING_CONCORDE	"ConcordeTracks"
#define SETTING_EASTBOUND_COLOR "EastboundColor"
#define SETTING_WESTBOUND_COLOR "WestboundColor"
#define SETTING_CONCORDE_COLOR	"ConcordeColor"

class NATShow {
private:

public:
	static bool Eastbound;
	static bool Westbound;
	static bool Letters;
	static bool Lines;
	static bool DottedLines;
	static bool StaticInfo;
	static bool Loading;
	static bool ShortWPNames;
	static bool OutOfDate;
	static bool AntialiasedLine;
	static bool ConcordTracks;
	static COLORREF EastboundColor;
	static COLORREF WestboundColor;
	static COLORREF ConcordeColor;

	static void Save( EuroScopePlugIn::CPlugIn * pPlugin );
	static void Load( EuroScopePlugIn::CPlugIn * pPlugin );
	static void Defaults();
};