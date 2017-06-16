#include "StdAfx.h"
#include "NATData.h"
#include "NATShow.h"
#include <regex>
//#include <iterator>
//#include <iostream>
//#include <string>
using namespace std;

NATData::NATWorkerCont NATData::NATWorkerData;
NATData * NATData::LastInstance = NULL;

NATData::NATData(void) {
	this->m_nats = new NAT[MAXNATS];
	this->m_natcount = new int;
	*this->m_natcount = 0;

	NATWorkerData.m_pNats = this->m_nats;
	NATWorkerData.m_pNatCount = this->m_natcount;

	NATData::LastInstance = this;
}


NATData::~NATData(void) {
	delete[] this->m_nats;
	delete this->m_natcount;
}

void NATData::Refresh(void) {
	// Easier than HttpRequest Async
	NATShow::Loading = true;
	this->workerThread = AfxBeginThread(NATData::FetchDataWorker, &NATWorkerData);
}

void NATData::GetTrackPtrs(NAT * pNats, int * pCount) {
	pNats = this->m_nats;
	pCount = this->m_natcount;
}

void Explode(CString szString, CString szToken, CStringArray& result) {
	result.RemoveAll();

	int iCurrPos = 0;
	CString subString;

	while (-1 != (iCurrPos = szString.Find(szToken))) {
		result.Add(szString.Left(iCurrPos));
		szString = szString.Right(szString.GetLength() - iCurrPos - szToken.GetLength());
	}

	if (szString.GetLength() > 0) {
		result.Add(szString);
	}
}

UINT NATData::FetchDataWorker(LPVOID pvar) {
	NATWorkerCont* dta = &NATData::NATWorkerData;

	NATShow::Loading = true;

	CWebGrab grab;
	CString response, track, waypoints, levels, line, tmp;
	CStringArray items;
	int pos1, pos2, pos3, pos4, itemcount, i, NATcnt = 0;

	grab.GetFile("https://pilotweb.nas.faa.gov/common/nat.html", response);

	// CString to string for regex searching
	string res((LPCTSTR) response);

	const regex track_regex("([a-zA-Z]{1}\\s+)([a-zA-Z]{5}\\s+)+((\\d{2,}\\/\\d{2,}\\s+)+)([a-zA-Z]{5}\\s+)+");

	//loop


	auto words_begin = sregex_iterator(res.begin(), res.end(), track_regex);
	auto words_end = sregex_iterator();


	for (sregex_iterator iter = words_begin; iter != words_end; ++iter) {
		smatch match = *iter;
		CString nat = match.str().c_str();

		// Make a NAT


		dta->m_pNats[NATcnt].Concorde = false;
		// Just West for now
		dta->m_pNats[NATcnt].Dir = Direction::WEST;
		dta->m_pNats[NATcnt].Letter = nat[0];


		// Tracks the index for the next waypoint to add.
		int waypoint_index = 0;

		// Scan one NAT string, build and add one NAT.
		for (int cursor = 2; cursor < nat.GetLength(); cursor++) {
			// SPACE
			if (nat[cursor] == ' ') { cursor++; continue; }

			// NAVAID
			if (isalpha(nat[cursor])) {
				CString navaid = nat.Mid(cursor, 5);
				cursor += 5;

				// Add waypoint
				// TODO: Find navaid's positions

				continue;
			}

			// LAT/LON
			if (isdigit(nat[cursor])) {

				// Lat and Long each have at least 2 digits, I've seen up to 4 (e.g 5730 would be 57.30).
				string lat;
				lat = nat.Mid(cursor, 2);
				cursor += 2;

				lat.operator+=('.');
				// If lat has additional decimal numbers.
				while (isdigit(nat[cursor])) {
					lat = lat.operator+=(nat[cursor]);
					cursor++;
				}
				// Eat a slash
				cursor++;

				string lon;
				lon = nat.Mid(cursor, 2);
				cursor += 2;
				lon.operator+=('.');
				// If lon has additional decimal numbers.
				while (isdigit(nat[cursor])) {
					lon = lon.operator+=(nat[cursor]);
					cursor++;
				}
				// String to Double
				double latitude = stod(lat);
				double longitude = stod(lon);
				// Longitudes are West, so negative sign is applied.
				longitude = longitude / -1;

				dta->m_pNats[NATcnt].Waypoints[waypoint_index].Position.m_Latitude = latitude;
				dta->m_pNats[NATcnt].Waypoints[waypoint_index].Position.m_Longitude = longitude;

				// The Longitude name.
				if (NATShow::ShortWPNames) {
					CString wp_name = lon.c_str();
					wp_name.Append("W");
					// Remove the decimal.
					wp_name.Replace(".", "");

					dta->m_pNats[NATcnt].Waypoints[waypoint_index].Name = wp_name;
				} else {
					CString wp_name = lat.c_str();
					wp_name.Append("N");
					wp_name.Append(lon.c_str());
					wp_name.Append("W");
					// Remove the decimals.
					wp_name.Replace(".", "");

					dta->m_pNats[NATcnt].Waypoints[waypoint_index].Name = wp_name;
				}

				// Increment for next waypoint to add
				waypoint_index++;

				continue;
			}


		}
		// Add total number of waypoints
		dta->m_pNats[NATcnt].WPCount = waypoint_index;

		NATcnt += 1;
		// Increment for next NAT to add
		*dta->m_pNatCount = NATcnt;


	}



	//end loop

	*dta->m_pNatCount = NATcnt;

	NATData::AddConcordTracks(dta);

	NATShow::Loading = false;

	return 0;
}



void NATData::AddConcordTracks(NATWorkerCont* dta) {
	int i = *dta->m_pNatCount;
	*dta->m_pNatCount += 5;

	// SM
	dta->m_pNats[i].Concorde = true;
	dta->m_pNats[i].Dir = Direction::NONE;
	dta->m_pNats[i].Letter = 'M';
	dta->m_pNats[i].Waypoints[0].Name = "15W";
	dta->m_pNats[i].Waypoints[0].Position.m_Latitude = 50.683333;
	dta->m_pNats[i].Waypoints[0].Position.m_Longitude = -15;
	dta->m_pNats[i].Waypoints[1].Name = "20W";
	dta->m_pNats[i].Waypoints[1].Position.m_Latitude = 50.833333;
	dta->m_pNats[i].Waypoints[1].Position.m_Longitude = -20;
	dta->m_pNats[i].Waypoints[2].Name = "30W";
	dta->m_pNats[i].Waypoints[2].Position.m_Latitude = 50.5;
	dta->m_pNats[i].Waypoints[2].Position.m_Longitude = -30;
	dta->m_pNats[i].Waypoints[3].Name = "40W";
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 49.266667;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[4].Name = "50W";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 47.05;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[5].Name = "53W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 46.166667;
	dta->m_pNats[i].Waypoints[5].Position.m_Longitude = -53;
	dta->m_pNats[i].Waypoints[6].Name = "60W";
	dta->m_pNats[i].Waypoints[6].Position.m_Latitude = 44.233333;
	dta->m_pNats[i].Waypoints[6].Position.m_Longitude = -60;
	dta->m_pNats[i].Waypoints[7].Name = "65W";
	dta->m_pNats[i].Waypoints[7].Position.m_Latitude = 42.766667;
	dta->m_pNats[i].Waypoints[7].Position.m_Longitude = -65;
	dta->m_pNats[i].Waypoints[8].Name = "67W";
	dta->m_pNats[i].Waypoints[8].Position.m_Latitude = 42;
	dta->m_pNats[i].Waypoints[8].Position.m_Longitude = -67;
	dta->m_pNats[i].WPCount = 9;
	i++;

	//SN
	dta->m_pNats[i].Concorde = true;
	dta->m_pNats[i].Dir = Direction::NONE;
	dta->m_pNats[i].Letter = 'N';
	dta->m_pNats[i].Waypoints[0].Name = "67W";
	dta->m_pNats[i].Waypoints[0].Position.m_Latitude = 40.416667;
	dta->m_pNats[i].Waypoints[0].Position.m_Longitude = -67;
	dta->m_pNats[i].Waypoints[1].Name = "65W";
	dta->m_pNats[i].Waypoints[1].Position.m_Latitude = 41.666667;
	dta->m_pNats[i].Waypoints[1].Position.m_Longitude = -65;
	dta->m_pNats[i].Waypoints[2].Name = "60W";
	dta->m_pNats[i].Waypoints[2].Position.m_Latitude = 43.116667;
	dta->m_pNats[i].Waypoints[2].Position.m_Longitude = -60;
	dta->m_pNats[i].Waypoints[3].Name = "5230W";
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 45.166667;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -52.5;
	dta->m_pNats[i].Waypoints[4].Name = "50W";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 45.9;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[5].Name = "40W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 48.166667;
	dta->m_pNats[i].Waypoints[5].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[6].Name = "30W";
	dta->m_pNats[i].Waypoints[6].Position.m_Latitude = 49.433333;
	dta->m_pNats[i].Waypoints[6].Position.m_Longitude = -30;
	dta->m_pNats[i].Waypoints[7].Name = "20W";
	dta->m_pNats[i].Waypoints[7].Position.m_Latitude = 49.816667;
	dta->m_pNats[i].Waypoints[7].Position.m_Longitude = -20;
	dta->m_pNats[i].Waypoints[8].Name = "15W";
	dta->m_pNats[i].Waypoints[8].Position.m_Latitude = 49.683333;
	dta->m_pNats[i].Waypoints[8].Position.m_Longitude = -15;
	dta->m_pNats[i].WPCount = 9;
	i++;

	//SL
	dta->m_pNats[i].Concorde = true;
	dta->m_pNats[i].Dir = Direction::NONE;
	dta->m_pNats[i].Letter = 'L';
	dta->m_pNats[i].Waypoints[0].Name = NATShow::ShortWPNames ? "50W" : "57N50W";
	dta->m_pNats[i].Waypoints[0].Position.m_Latitude = 57;
	dta->m_pNats[i].Waypoints[0].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[1].Name = NATShow::ShortWPNames ? "40W" : "57N40W";
	dta->m_pNats[i].Waypoints[1].Position.m_Latitude = 57;
	dta->m_pNats[i].Waypoints[1].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[2].Name = NATShow::ShortWPNames ? "30W" : "56N30W";
	dta->m_pNats[i].Waypoints[2].Position.m_Latitude = 56;
	dta->m_pNats[i].Waypoints[2].Position.m_Longitude = -30;
	dta->m_pNats[i].Waypoints[3].Name = NATShow::ShortWPNames ? "20W" : "54N20W";
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 54;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -20;
	dta->m_pNats[i].Waypoints[4].Name = NATShow::ShortWPNames ? "15W" : "52N15W";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 52;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -15;
	dta->m_pNats[i].WPCount = 5;
	i++;

	//SP
	dta->m_pNats[i].Concorde = true;
	dta->m_pNats[i].Dir = Direction::NONE;
	dta->m_pNats[i].Letter = 'P';
	dta->m_pNats[i].Waypoints[0].Name = "20W";
	dta->m_pNats[i].Waypoints[0].Position.m_Latitude = 46.816667;
	dta->m_pNats[i].Waypoints[0].Position.m_Longitude = -20;
	dta->m_pNats[i].Waypoints[1].Name = "45N";
	dta->m_pNats[i].Waypoints[1].Position.m_Latitude = 45;
	dta->m_pNats[i].Waypoints[1].Position.m_Longitude = -23.883333;
	dta->m_pNats[i].Waypoints[2].Name = "30W";
	dta->m_pNats[i].Waypoints[2].Position.m_Latitude = 41.6;
	dta->m_pNats[i].Waypoints[2].Position.m_Longitude = -30;
	dta->m_pNats[i].Waypoints[3].Name = "40W";
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 34.366667;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[4].Name = "27N";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 27;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -47.783333;
	dta->m_pNats[i].Waypoints[5].Name = "50W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 24.633333;
	dta->m_pNats[i].Waypoints[5].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[6].Name = "18N";
	dta->m_pNats[i].Waypoints[6].Position.m_Latitude = 18;
	dta->m_pNats[i].Waypoints[6].Position.m_Longitude = -55.65;
	dta->m_pNats[i].WPCount = 7;
	i++;

	//SO
	dta->m_pNats[i].Concorde = true;
	dta->m_pNats[i].Dir = Direction::NONE;
	dta->m_pNats[i].Letter = 'O';
	dta->m_pNats[i].Waypoints[0].Name = "15W";
	dta->m_pNats[i].Waypoints[0].Position.m_Latitude = 48.666667;
	dta->m_pNats[i].Waypoints[0].Position.m_Longitude = -15;
	dta->m_pNats[i].Waypoints[1].Name = "20W";
	dta->m_pNats[i].Waypoints[1].Position.m_Latitude = 48.8;
	dta->m_pNats[i].Waypoints[1].Position.m_Longitude = -20;
	dta->m_pNats[i].Waypoints[2].Name = "30W";
	dta->m_pNats[i].Waypoints[2].Position.m_Latitude = 48.366667;
	dta->m_pNats[i].Waypoints[2].Position.m_Longitude = -30;
	dta->m_pNats[i].Waypoints[3].Name = "40W";
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 47.066667;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[4].Name = "50W";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 44.75;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[5].Name = "52W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 44.166667;
	dta->m_pNats[i].Waypoints[5].Position.m_Longitude = -52;
	dta->m_pNats[i].Waypoints[6].Name = "60W";
	dta->m_pNats[i].Waypoints[6].Position.m_Latitude = 42;
	dta->m_pNats[i].Waypoints[6].Position.m_Longitude = -60;
	dta->m_pNats[i].WPCount = 7;



}
