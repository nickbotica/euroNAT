#pragma once

#include "StdAfx.h"
#include "WebGrab.h"
#include <EuroScopePlugIn.h>

using namespace EuroScopePlugIn;

#define FLCOUNT 20
#define WPCOUNT 20
#define MAXNATS 26

enum Direction{
	NONE,
	EAST,
	WEST
};

struct NATWaypoint {
public:
	CString Name;
	CPosition Position;
};

struct NAT {
public:
	int WPCount;
	char Letter;
	Direction Dir;
	int FlightLevels[FLCOUNT];
	NATWaypoint Waypoints[WPCOUNT];
	int TMI;
	bool Concorde;
};

class NATData
{
private:
	CWinThread* workerThread;

public: static struct NATWorkerCont{
		NAT * m_pNats;
		int * m_pNatCount;
	} NATWorkerData;

protected:

	NAT * m_nats;
	int * m_natcount;

	static void AddConcordTracks( NATWorkerCont* dta );

	static UINT FetchDataWorker( LPVOID pvar );

public:
	NATData(void);
	~NATData(void);

	static NATData * LastInstance;

	static bool IsDataInit(){
		return *NATWorkerData.m_pNatCount > 0;
	}

	void GetTrackPtrs( NAT * pNats, int * pCount );

	void Refresh(void);
};

