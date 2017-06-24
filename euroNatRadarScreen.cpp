#include "StdAfx.h"
#include "euroNatRadarScreen.h"
#include "NATShow.h"
#include <math.h>

#include "wuline.h"

#define WPWIDTH 4

euroNatRadarScreen::euroNatRadarScreen( NAT * pNat, int * pCount )
{
	this->m_pNat = pNat;
	this->m_pNatCount = pCount;
	
	this->m_pFont = new CFont();
	this->m_pFontBold = new CFont();
	this->m_pFont->CreateFontA(
		13, 7, 
		0, 0, 
		FW_NORMAL, 
		0, 0, 0, 
		ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"EuroScope"
		);
	this->m_pFontBold->CreateFontA(
		13, 7, 
		0, 0, 
		FW_BOLD, 
		0, 0, 0, 
		ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"EuroScope"
		);

	this->m_showMouseover = -1;
}

euroNatRadarScreen::~euroNatRadarScreen(void)
{
	delete this->m_pFont;
	delete this->m_pFontBold;
}

inline void euroNatRadarScreen::OnRefresh( HDC hDC, int Phase ){
	DWORD curTick = GetTickCount();

	RECT radarArea = this->GetRadarArea();
	
	if( Phase == REFRESH_PHASE_BEFORE_TAGS ){
		this->m_showMouseover = -1;

		POINT cursorPos;
		GetCursorPos( &cursorPos );

		CDC dc;
		dc.Attach( hDC );

		CFont * oldFont = dc.SelectObject( this->m_pFont );

		dc.SetTextColor( RGB(150,150,150) );

		RECT rect; // Temp RECT

		DWORD dottedstyle[6] = {2, 6};
		DWORD solidstyle[2] = {1, 0};
		LOGBRUSH line_east, line_west, line_concorde;
		line_east.lbColor = NATShow::EastboundColor;
		line_west.lbColor = NATShow::WestboundColor;
		line_concorde.lbColor = NATShow::ConcordeColor;
		line_east.lbStyle = line_west.lbStyle = line_concorde.lbStyle = BS_SOLID;
		
		HPEN hpen_east = ExtCreatePen( PS_GEOMETRIC | PS_USERSTYLE, 1, &line_east, 2, (NATShow::DottedLines ? dottedstyle : solidstyle) );
		HPEN hpen_west = ExtCreatePen( PS_GEOMETRIC | PS_USERSTYLE, 1, &line_west, 2, (NATShow::DottedLines ? dottedstyle : solidstyle) );
		HPEN hpen_concorde = ExtCreatePen( PS_GEOMETRIC | PS_USERSTYLE, 1, &line_concorde, 2, (NATShow::DottedLines ? dottedstyle : solidstyle) );

		//CPen * plinepen_east = new CPen( (NATShow::DottedLines ? PS_DOT : PS_SOLID), 0, RGB( 115, 148, 148 ) );
		//CPen * plinepen_west = new CPen( (NATShow::DottedLines ? PS_DOT : PS_SOLID), 0, RGB( 115, 148, 115 ) );
		CPen * pwprectpen = new CPen( PS_SOLID, 0, RGB( 100, 100, 100 ) );
		CBrush * pwprectbrush = new CBrush( RGB(0,0,0) );

		int count = *this->m_pNatCount;
		if( *this->m_pNatCount > 0 ){
			for( int i = 0; i < count; i++ ){
				// Check if show NATShow options
				if( (this->m_pNat[i].Dir == Direction::EAST && !NATShow::Eastbound) || 
					(this->m_pNat[i].Dir == Direction::WEST && !NATShow::Westbound) ||
					(this->m_pNat[i].Concorde && !NATShow::ConcordTracks))
					continue;

				POINT tmpPoint = CRadarScreen::ConvertCoordFromPositionToPixel( this->m_pNat[i].Waypoints[0].Position );
				if( this->m_showMouseover == -1 ){
					this->m_showMouseover = (sqrt((double)(((cursorPos.x-tmpPoint.x)*(cursorPos.x-tmpPoint.x))+((cursorPos.y-tmpPoint.y)*(cursorPos.y - tmpPoint.y)))) < 10) ? i : -1;
				}
				RECT tmpRect;
				LPRECT ptmpRect = &tmpRect;

				bool lastWPunknown = false;

				if( this->m_pNat[i].Waypoints[0].Name != "SKIPME" ){ // Does first WP exist
					// Draw first WP box
					dc.SelectObject( pwprectpen );
					dc.SelectObject( pwprectbrush );
					dc.Rectangle( tmpPoint.x - WPWIDTH, tmpPoint.y - WPWIDTH, tmpPoint.x + WPWIDTH, tmpPoint.y + WPWIDTH );

					// Euroscope Interaction Rectangles (For Mouseovers)
					rect.left = tmpPoint.x - WPWIDTH - 20;
					rect.top = tmpPoint.y - WPWIDTH - 20;
					rect.right = tmpPoint.x + WPWIDTH + 20;
					rect.bottom = tmpPoint.y + WPWIDTH + 20;
					this->AddScreenObject( 0, "", rect, false, "" );

					// Draw first WP name
					tmpRect = CRect( tmpPoint.x - 20, tmpPoint.y + WPWIDTH * 2, tmpPoint.x + 20, (tmpPoint.y + WPWIDTH * 2) + 20 );
					if (NATShow::ShortWPNames) {
						dc.DrawTextEx(this->m_pNat[i].Waypoints[0].ShortName, ptmpRect, DT_CENTER | DT_NOCLIP | DT_VCENTER, NULL);
					} else {
						dc.DrawTextEx(this->m_pNat[i].Waypoints[0].Name, ptmpRect, DT_CENTER | DT_NOCLIP | DT_VCENTER, NULL);
					}
					if( NATShow::Letters ){
						// Draw NAT Letter
						dc.SelectObject( this->m_pFontBold );
						dc.TextOut( tmpPoint.x - WPWIDTH, tmpPoint.y - WPWIDTH * 4, (this->m_pNat[i].Concorde ? "S" : "") + CString(this->m_pNat[i].Letter) );
						dc.SelectObject( this->m_pFont );
					}

					dc.MoveTo( tmpPoint );
				} else {
					lastWPunknown = true;
				}
				for( int wp = 1; wp < this->m_pNat[i].WPCount; wp++ ){
					if( this->m_pNat[i].Waypoints[wp].Name == "SKIPME" ){ // Unknown point or DB error;
						lastWPunknown = true;
						continue;
					}

					tmpPoint = CRadarScreen::ConvertCoordFromPositionToPixel( this->m_pNat[i].Waypoints[wp].Position );
					if( this->m_showMouseover == -1 ){
						this->m_showMouseover = (sqrt((double)(((cursorPos.x-tmpPoint.x)*(cursorPos.x-tmpPoint.x))+((cursorPos.y-tmpPoint.y)*(cursorPos.y - tmpPoint.y)))) < 10) ? i : -1;
					}

					// Draw WP box
					dc.SelectObject( pwprectpen );
					dc.SelectObject( pwprectbrush );
					dc.Rectangle( tmpPoint.x - WPWIDTH, tmpPoint.y - WPWIDTH, tmpPoint.x + WPWIDTH, tmpPoint.y + WPWIDTH );

					// Euroscope Interaction Rectangles (For Mouseovers)
					rect.left = tmpPoint.x - WPWIDTH - 20;
					rect.top = tmpPoint.y - WPWIDTH - 20;
					rect.right = tmpPoint.x + WPWIDTH + 20;
					rect.bottom = tmpPoint.y + WPWIDTH + 20;
					this->AddScreenObject( 0, "", rect, false, "" );

					// Draw WP name
					tmpRect = CRect( tmpPoint.x - 20, tmpPoint.y + WPWIDTH * 2, tmpPoint.x + 20, (tmpPoint.y + WPWIDTH * 2) + 20 );
					if (NATShow::ShortWPNames) {
						dc.DrawTextEx(this->m_pNat[i].Waypoints[wp].ShortName, ptmpRect, DT_CENTER | DT_NOCLIP | DT_VCENTER, NULL);
					} else {
						dc.DrawTextEx(this->m_pNat[i].Waypoints[wp].Name, ptmpRect, DT_CENTER | DT_NOCLIP | DT_VCENTER, NULL);
					}
					if( NATShow::Lines && !lastWPunknown ){
						// Draw line segment
						if( this->m_pNat[i].Concorde )
							dc.SelectObject( hpen_concorde );
						else if( this->m_pNat[i].Dir == Direction::EAST )
							dc.SelectObject( hpen_east );
						else
							dc.SelectObject( hpen_west );

						if( !NATShow::DottedLines && NATShow::AntialiasedLine )
							DrawWuLine( &dc, dc.GetCurrentPosition().x, dc.GetCurrentPosition().y, tmpPoint.x, tmpPoint.y, (this->m_pNat[i].Dir == Direction::EAST ? line_east.lbColor : this->m_pNat[i].Concorde ? line_concorde.lbColor : line_west.lbColor) );
						else
							dc.LineTo( tmpPoint );
					} else
						lastWPunknown = false;
					dc.MoveTo( tmpPoint );
				}
				
			}
		}

		// Show Info Mouseover
		if( this->m_showMouseover >= 0 ){
			dc.SelectObject( pwprectpen );
			dc.SelectObject( pwprectbrush );
			rect.top = cursorPos.y + 15;
			rect.left = cursorPos.x + 15;
			rect.right = rect.left + 170;
			rect.bottom = rect.top + 45;
			dc.Rectangle( rect.left, rect.top, rect.right, rect.bottom );
			CString str;
			if( this->m_pNat[ this->m_showMouseover ].Concorde )
				str.Format("NAT:S%c   Concorde SST", this->m_pNat[ this->m_showMouseover ].Letter);
			else
				str.Format("NAT:%c   TMI:%d   %s", this->m_pNat[ this->m_showMouseover ].Letter, this->m_pNat[ this->m_showMouseover ].TMI, this->m_pNat[ this->m_showMouseover ].Dir == Direction::EAST ? "East" : "West");
			dc.TextOut( rect.left + 4, rect.top + 4, str );
			if( !this->m_pNat[ this->m_showMouseover ].Concorde ){
				dc.TextOut( rect.left + 6, rect.top + 18, "FLs:" );
				for( int i = 0; i < 15; i++ ){
					if( this->m_pNat[ this->m_showMouseover ].FlightLevels[i] == 0 )
						break;
					str.Format("%d", this->m_pNat[ this->m_showMouseover ].FlightLevels[i] );
					dc.TextOut( rect.left + 37 + ((i % 5) * 25), rect.top + 18 + ((i / 5) * 12), str );
				}
			} else {
				dc.TextOut( rect.left + 6, rect.top + 18, "FLs: 500+" );
			}
			this->RequestRefresh();
		}

		// Info Messages
		if( NATShow::Loading )
			dc.TextOut( radarArea.right - 150, radarArea.bottom - 26, "Loading NAT Data..." );
		//if( NATShow::OutOfDate )
		//	dc.TextOut( radarArea.right - 325, radarArea.bottom - 14, "Caution: EuroNAT Waypoint Database Out of Date" );

		// Cleanup
		DeleteObject( hpen_east );
		DeleteObject( hpen_west );
		DeleteObject( hpen_concorde );
		delete pwprectpen;
		delete pwprectbrush;

		dc.SelectObject( oldFont );

		dc.Detach();
	}
}

inline void euroNatRadarScreen::OnOverScreenObject( int ObjectType, const char * sObjectId, POINT Pt, RECT Area ){
	this->RequestRefresh();
}

inline void euroNatRadarScreen::OnAsrContentToBeSaved( void ){
	NATShow::Save( this->GetPlugIn() );
}

inline void euroNatRadarScreen::OnAsrContentLoaded( bool Loaded ){
	if( Loaded )
		NATShow::Load( this->GetPlugIn() );
}