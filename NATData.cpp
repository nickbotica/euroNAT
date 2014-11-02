#include "StdAfx.h"
#include "NATData.h"
#include "NATShow.h"

NATData::NATWorkerCont NATData::NATWorkerData;
NATData * NATData::LastInstance = NULL;

NATData::NATData(void)
{
	this->m_nats = new NAT[MAXNATS];
	this->m_natcount = new int;
	*this->m_natcount = 0;

	NATWorkerData.m_pNats = this->m_nats;
	NATWorkerData.m_pNatCount = this->m_natcount;

	NATData::LastInstance = this;
}


NATData::~NATData(void)
{
	delete [] this->m_nats;
	delete this->m_natcount;
}

void NATData::Refresh(void){
	// Easier than HttpRequest Async
	NATShow::Loading = true;
	this->workerThread = AfxBeginThread( NATData::FetchDataWorker, &NATWorkerData );
}

void NATData::GetTrackPtrs( NAT * pNats, int * pCount ){
	pNats = this->m_nats;
	pCount = this->m_natcount;
}

void Explode(CString szString, CString szToken, CStringArray& result)
{
	result.RemoveAll();

	int iCurrPos = 0;
	CString subString;

	while (-1 != (iCurrPos = szString.Find(szToken)))
	{
		result.Add(szString.Left(iCurrPos));
		szString = szString.Right(szString.GetLength() - iCurrPos - szToken.GetLength());
	}

	if (szString.GetLength() > 0)
	{
		result.Add(szString);
	}
}

UINT NATData::FetchDataWorker( LPVOID pvar ){
	NATWorkerCont* dta = &NATData::NATWorkerData;

	NATShow::Loading = true;

	CWebGrab grab;
	CString response, track, waypoints, levels, line, tmp;
	CStringArray items;
	int pos1, pos2, pos3, pos4, itemcount, i, NATcnt = 0;

	grab.GetFile("http://vrcapi.metacraft.com/VRC/api/oceanic_tracks.xml", response, _T("euroNAT"));
	
	response.Replace("\n", "");

	pos1 = response.Find("<track type=\"NAT\"");
	while (pos1 > -1){
		pos2 = response.Find("</track>", pos1) + 8;
		track = response.Mid(pos1, pos2 - pos1);

		// Letter
		tmp = track.Mid(track.Find("id=\"") + 4, 1);
		dta->m_pNats[NATcnt].Letter = tmp.GetAt(0);

		// Direction
		tmp = track.Mid(track.Find("direction=\"") + 11, 4);
		dta->m_pNats[NATcnt].Dir = tmp == "WEST" ? Direction::WEST : Direction::EAST;

		// TMI
		tmp = track.Mid(track.Find("tmi=\"") + 5, 3);
		dta->m_pNats[NATcnt].TMI = atoi(tmp);

		// Type
		dta->m_pNats[NATcnt].Concorde = false;

		// Waypoints
		pos3 = track.Find("<waypoints>") + 11;
		pos4 = track.Find("</waypoints>");
		waypoints = track.Mid(pos3, pos4 - pos3); //AfxMessageBox(waypoints, MB_OK);

		waypoints.Replace("<waypoint id=\"", "");
		waypoints.Replace("\" />", " ");
		waypoints.Replace("\" lat=\"", " ");
		waypoints.Replace("\" lon=\"", " ");

		Explode(waypoints, " ", items);
		itemcount = items.GetCount() / 3;

		dta->m_pNats[NATcnt].WPCount = itemcount;

		for (i = 0; i < itemcount; i++){
			dta->m_pNats[NATcnt].Waypoints[i].Position.m_Latitude = atof(items[3 * i + 1]);
			dta->m_pNats[NATcnt].Waypoints[i].Position.m_Longitude = atof(items[3 * i + 2]);
			if (items[3 * i].GetAt(0) >= '0' & items[3 * i].GetAt(0) <= '9'){
				if (NATShow::ShortWPNames)
					dta->m_pNats[NATcnt].Waypoints[i].Name.Format("%sW", items[3 * i].Right(2));
				else
					dta->m_pNats[NATcnt].Waypoints[i].Name.Format("%sN%sW", items[3 * i].Left(2), items[3 * i].Right(2));
			}
			else {
				dta->m_pNats[NATcnt].Waypoints[i].Name = items[3 * i];
			}
			//tmp.Format("%s: %f %f", items[3 * i], atof(items[3 * i + 1]), atof(items[3 * i + 2])); AfxMessageBox(tmp, MB_OK);
		}

		// Flight Levels
		pos3 = track.Find("<levels>") + 8;
		pos4 = track.Find("</levels>");
		levels = track.Mid(pos3, pos4 - pos3); //AfxMessageBox(levels, MB_OK);
		
		levels.Replace("\" direction=\"WEST\"", "");
		levels.Replace("\" direction=\"EAST\"", "");
		levels.Replace("<level fl=\"", "");
		levels.Replace("/>", ""); //AfxMessageBox(levels, MB_OK);
		
		Explode(levels, " ", items);
		itemcount = items.GetCount();
		
		for (i = 0; i < itemcount; i++){
			dta->m_pNats[NATcnt].FlightLevels[i] = atoi(items[i]);
			//tmp.Format("%d", atoi(items[i])); AfxMessageBox(tmp, MB_OK);
		}

		NATcnt += 1;

		pos1 = response.Find("<track type=\"NAT\"", pos1 + 1);
	}

	*dta->m_pNatCount = NATcnt;

	NATData::AddConcordTracks(dta);

	NATShow::Loading = false;

	return 0;
}

void NATData::AddConcordTracks( NATWorkerCont* dta ){
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
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 34.366667 ;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[4].Name = "27N";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 27;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -47.783333;
	dta->m_pNats[i].Waypoints[5].Name = "50W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 24.633333 ;
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
	dta->m_pNats[i].Waypoints[3].Position.m_Latitude = 47.066667 ;
	dta->m_pNats[i].Waypoints[3].Position.m_Longitude = -40;
	dta->m_pNats[i].Waypoints[4].Name = "50W";
	dta->m_pNats[i].Waypoints[4].Position.m_Latitude = 44.75;
	dta->m_pNats[i].Waypoints[4].Position.m_Longitude = -50;
	dta->m_pNats[i].Waypoints[5].Name = "52W";
	dta->m_pNats[i].Waypoints[5].Position.m_Latitude = 44.166667 ;
	dta->m_pNats[i].Waypoints[5].Position.m_Longitude = -52;
	dta->m_pNats[i].Waypoints[6].Name = "60W";
	dta->m_pNats[i].Waypoints[6].Position.m_Latitude = 42;
	dta->m_pNats[i].Waypoints[6].Position.m_Longitude = -60;
	dta->m_pNats[i].WPCount = 7;
}