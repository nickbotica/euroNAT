#include "StdAfx.h"
#include "euroNatPlugin.h"
#include "NATShow.h"
#include "resource.h"

// TODO: Update version number for new releases (format x.x.x)
const double this_version_number = 1.3;
// INT_MAX if not in beta
const unsigned int this_beta_version = 1;
const char * version_string = "1.3b1";

const CString pluginversion_url = "https://raw.githubusercontent.com/nickbotica/euroNAT/master/pluginversion.txt";

euroNatPlugin::euroNatPlugin(void) : 
	EuroScopePlugIn::CPlugIn( 
		EuroScopePlugIn::COMPATIBILITY_CODE, 
		"euroNAT",
		version_string,
		"Nick Botica (999991)",
		"NYARTCC ES euroNAT"
	)
{
	this->m_nats = NATData::NATWorkerData.m_pNats;
	this->m_natcount = NATData::NATWorkerData.m_pNatCount;

	CheckVersion();

	NATShow::Load( this );

	this->natData.SetPlugin( this );
	this->natData.Refresh();
}

euroNatPlugin::~euroNatPlugin(void)
{
	NATShow::Save( this );
}

bool euroNatPlugin::OnCompileCommand( const char * sCommandLine ){
	CString cmd(sCommandLine);
	if( cmd.Left(11) == CString(".natrefresh") ) {
		this->natData.Refresh();
	} else if( cmd.Left(9) == CString(".natshow ") ){
		if( cmd.Mid( 9, 3 ) == "gui" ){
			this->m_guiopts.DestroyWindow();
			this->m_guiopts.Create( IDD_DIALOG_OPTIONS );
			this->m_guiopts.ShowWindow( 1 );
		} else if( cmd.Mid( 9, 4 ) == "east" ){
			NATShow::Eastbound = true;
			NATShow::Westbound = false;
		} else if (cmd.Mid(9, 4) == "west") {
			NATShow::Eastbound = false;
			NATShow::Westbound = true;
		} else if (cmd.Mid(9, 4) == "both") {
			NATShow::Eastbound = true;
			NATShow::Westbound = true;
		} else if( cmd.Mid( 9, 3 ) == "all" ){
			NATShow::Eastbound = true;
			NATShow::Westbound = true;
			NATShow::ConcordTracks = true;
		} else if( cmd.Mid( 9, 4 ) == "none" ){
			NATShow::Eastbound = false;
			NATShow::Westbound = false;
			NATShow::ConcordTracks = false;
		} else if (cmd.Mid(9, 4) == "conc") {
			NATShow::ConcordTracks = !NATShow::ConcordTracks;
		} else if (cmd.Mid(9, 8) == "concorde") {
			NATShow::ConcordTracks = !NATShow::ConcordTracks;
		}
	} else if( cmd.Left(11) == ".natoptions" ){
		this->m_guiopts.DestroyWindow();
		this->m_guiopts.Create( IDD_DIALOG_OPTIONS );
		this->m_guiopts.ShowWindow( 1 );
	} else if( cmd.Left(8) == ".natopt " ){
		int nsep = cmd.Find(' ', 8);
		CString opt( cmd.Mid( 8, nsep - 8 ) );
		CString val( cmd.Right( cmd.GetLength() - nsep - 1 ) );
		if( opt == "linestyle" ){
			if( val == "dotted" ){
				NATShow::DottedLines = true;
				NATShow::Lines = true;
			} else if( val == "solid" ){
				NATShow::DottedLines = false;
				NATShow::Lines = true;
			} else if( val == "none" ){
				NATShow::Lines = false;
			}
		} else if( opt == "infostyle" ){
			if( val == "static" ){
				NATShow::StaticInfo = true;
			} else if( val == "popup" ){
				NATShow::StaticInfo = false;
			}
		} else if( opt == "letters" ){
			if( val == "show" ){
				NATShow::Letters = true;
			} else if( val == "hide" ){
				NATShow::Letters = false;
			}
		}
	} else {
		return false;
	}
	return true;
}

inline CRadarScreen * euroNatPlugin::OnRadarScreenCreated(
	const char * sDisplayName, 
	bool NeedRadarContent, 
	bool GeoReferenced, 
	bool CanBeSaved, 
	bool CanBeCreated
)
{
	int i = this->m_RadarScreenList.Add( new euroNatRadarScreen( this->m_nats, this->m_natcount ) );

	return this->m_RadarScreenList[i];
}

void euroNatPlugin::CheckVersion(void) {

	CWebGrab grab;
	CString response;

	bool downloaded = grab.GetFile(pluginversion_url, response);

	if (!downloaded) {
		CString message;
		message.Format("Couldn't reach %s, to check for a newer version.", pluginversion_url);
		DisplayUserMessage("euroNAT", "Info", message, true, true, false, false, false);
		return;
	}

	std::string res((LPCTSTR) response);

	if (res.find("404") != std::string::npos) {
		CString message;
		message.Format("Received '404: Not Found' at %s, while checking for a newer version.", pluginversion_url);
		DisplayUserMessage("euroNAT", "Info", message, true, false, false, false, false);
		return;
	}

	double new_version_number = std::stod(res);

	// check for a beta version
	unsigned int new_beta_version = 0;
	int beta_cursor = response.Find("beta");
	if (beta_cursor >= 0) {
		beta_cursor += 4;
		std::string new_beta_version_str = res.substr(beta_cursor);
		new_beta_version = std::stod(new_beta_version_str);
	}


	if (this_version_number < new_version_number) {
		// if it's in beta
		if (new_beta_version != INT_MAX) {
			CString message;
			message.Format("There is a new beta (v%g beta %d) avaliable at github.com/nickbotica/euroNAT/releases.", new_version_number, new_beta_version);
			DisplayUserMessage("euroNAT", "Info", message, true, true, false, false, false);
		}
		else {
			CString message;
			message.Format("There is a new version (v%g) avaliable at github.com/nickbotica/euroNAT/releases.", new_version_number);
			DisplayUserMessage("euroNAT", "Info", message, true, true, false, true, false);
		}

	} else if (this_version_number == new_version_number) {
		if (this_beta_version < new_beta_version) {
			CString message;
			message.Format("There is a new beta (v%g beta %d) avaliable at github.com/nickbotica/euroNAT/releases.", new_version_number, new_beta_version);
			DisplayUserMessage("euroNAT", "Info", message, true, true, false, false, false);

		} else if (this_beta_version < INT_MAX && new_beta_version == 0) {
			CString message;
			message.Format("There is a new version (v%g) avaliable at github.com/nickbotica/euroNAT/releases.", new_version_number);
			DisplayUserMessage("euroNAT", "Info", message, true, true, false, true, false);
		}
	
	}

	grab.Close();

}

