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

void Explode(const TCHAR* str, const TCHAR* delimiters, CStringArray& arr)
{
	arr.RemoveAll();

	TCHAR* copy = new TCHAR[_tcslen(str)+sizeof(TCHAR)];
	_tcscpy(copy,str);
	
	TCHAR* param = copy;
	while(true)
	{
		TCHAR* result = _tcstok(param,delimiters);
		if(!result) //no more tokens
		{
			delete copy;
			return;
		}

		arr.Add(result);

		param = NULL;
	}

	delete copy;
}

UINT NATData::FetchDataWorker( LPVOID pvar ){
	NATWorkerCont* dta = &NATData::NATWorkerData;

	NATShow::Loading = true;
	
	CDaoDatabase * pwpdb;
	CDaoQueryDef * pqry;
	CDaoRecordset * precord;

	try{
		// Get dll directory
		TCHAR dllpath[2048];
		GetModuleFileName( GetModuleHandle("euroNAT.dll" ), dllpath, 2048 );
		CString dbfilename( dllpath );
		dbfilename = dbfilename.Left( dbfilename.ReverseFind('\\') + 1 );
		dbfilename += "wp.mdb";

		// Open Database
		pwpdb = new CDaoDatabase();
		pwpdb->Open( dbfilename, false, true );

		pqry = new CDaoQueryDef( pwpdb );
		pqry->Open( "GetWaypointLatLon" );

		precord = new CDaoRecordset( pwpdb );
	}catch(...){
		AfxMessageBox( "euroNAT: Unable to open waypoint database.", MB_OK );
		return 0;
	}


	CString response;
	CWebGrab grab;

	grab.GetFile( "http://nova37.com/nat.php", response, _T("euroNAT") );

	// Clean vars
	for( int i = 0; i < MAXNATS; i++ ){ dta->m_pNats[i].Letter = 0; for( int j = 0; j < FLCOUNT; j++ ){ dta->m_pNats[i].FlightLevels[j] = 0; } }

	// Parse routes
	CStringArray lines;
	CStringArray vals;
	CStringArray rtal;
	Explode( response, "\n", lines );
	int count = atoi(lines[0]);
	int tmi = 0;
	*dta->m_pNatCount = count;
	for( int c = 0,i = 1; c < count; c++, i++ ){
		if( lines[i].Left(1) == "#" ){
			// Look for TMI lines
			tmi = atoi( lines[i].Right(lines[i].GetLength()-1) );
			c--;
			continue;
		}

		Explode( lines[i], "!", vals );

		dta->m_pNats[c].Dir = vals[2] == "NIL" ? Direction::WEST : Direction::EAST;
		dta->m_pNats[c].Letter = vals[0].GetAt(0);
		dta->m_pNats[c].TMI = tmi;

		// Waypoints
		Explode( vals[1], " ", rtal );
		{
			int j = 0;
			for( int rtalc = rtal.GetCount(); j < rtalc; j++ ){
				if( rtal[j][0] >= '0' && rtal[j][0] <= '9' ){ // If WP starts with a number
					int lat = atoi( rtal[j].Left(2) );
					int lon = atoi( rtal[j].Right(2) );
					dta->m_pNats[c].Waypoints[j].Position.m_Latitude = (double) lat;
					dta->m_pNats[c].Waypoints[j].Position.m_Longitude = (double) lon * -1;
					if( NATShow::ShortWPNames )
						dta->m_pNats[c].Waypoints[j].Name.Format( "%dW", lon );
					else
						dta->m_pNats[c].Waypoints[j].Name.Format( "%dN%dW", lat, lon );
				} else {
					// Lookup Lat Lon from DB
					dta->m_pNats[c].Waypoints[j].Name = rtal[j];
					try{
						pqry->SetParamValue( "WaypointName", COleVariant(dta->m_pNats[c].Waypoints[j].Name, VT_BSTRT) );
						if( precord->IsOpen() ){
							precord->Requery();
						} else {
							precord->Open( pqry, 4 );
						}
						COleVariant olevarLat = precord->GetFieldValue( "Latitude" );
						COleVariant olevarLon = precord->GetFieldValue( "Longtitude" );
						dta->m_pNats[c].Waypoints[j].Position.m_Latitude = olevarLat.dblVal;
						dta->m_pNats[c].Waypoints[j].Position.m_Longitude= olevarLon.dblVal;
					} catch( ... ){
						dta->m_pNats[c].Waypoints[j].Name = "SKIPME";
						NATShow::OutOfDate = true;
						// Database error or point doesn't exist!
						//pwpdb->Close();
						//break;
					}
				}
			}
			dta->m_pNats[c].WPCount = j;
		}

		// Flight Levels
		if( dta->m_pNats[c].Dir == Direction::EAST )
			Explode( vals[2], " ", rtal );
		else
			Explode( vals[3], " ", rtal );
		{
			int j = 0;
			for( int rtalc = rtal.GetCount(); j < rtalc; j++ ){
				dta->m_pNats[c].FlightLevels[j] = atoi( rtal[j] );
			}
		}
	}

	NATData::AddConcordTracks( dta );

	pqry->Close();
	precord->Close();
	pwpdb->Close();

	delete pqry;
	delete precord;
	delete pwpdb;

	AfxDaoTerm();
	
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