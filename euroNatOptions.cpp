// euroNatOptions.cpp : implementation file
//

#include "stdafx.h"
#include "euroNAT.h"
#include "euroNatOptions.h"
#include "afxdialogex.h"
#include "NATShow.h"
#include "NATData.h"


// euroNatOptions dialog

IMPLEMENT_DYNAMIC(euroNatOptions, CDialogEx)

euroNatOptions::euroNatOptions(CWnd* pParent /*= NULL*/ )
	: CDialogEx(euroNatOptions::IDD, pParent)
{
}

euroNatOptions::~euroNatOptions()
{
}

void euroNatOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHOW_LINES, m_chkShowLines);
	DDX_Control(pDX, IDC_CHECK_SHOW_LETTERS, m_chkShowLetters);
	DDX_Control(pDX, IDC_CHECK_EASTBOUND, m_chkEastbound);
	DDX_Control(pDX, IDC_CHECK_WESTBOUND, m_chkWestbound);
	DDX_Control(pDX, IDC_CHECK_SHORT_NAMES, m_chkShortName);
	DDX_Control(pDX, IDC_CHECK_ANTI_ALIASED, m_chkAntiAliased );
	DDX_Control(pDX, IDC_CHECK_CONCORD, m_chkConcordTracks );

	DDX_Control(pDX, IDC_COMBO_LINESTYLE, m_cbolinestyle );

	DDX_Control(pDX, IDC_BUTTON_GET_NAT_DATA, m_cmdGetNatData);
	DDX_Control(pDX, IDC_BUTTON_DEFAULTS, m_cmdLoadDefaults);

	DDX_Control(pDX, IDC_MFCCOLORBUTTON_EAST, m_colorbtnEastbound);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_WEST, m_colorbtnWestbound);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_CONC, m_colorbtnConcorde);
}


BEGIN_MESSAGE_MAP(euroNatOptions, CDialogEx)
	ON_BN_CLICKED(IDOK, &euroNatOptions::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GET_NAT_DATA, &euroNatOptions::OnBnClickedGetNatData)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, &euroNatOptions::OnBnClickedLoadDefaults)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON_EAST, &euroNatOptions::OnColorPickedEastbound)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON_WEST, &euroNatOptions::OnColorPickedWestbound)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON_CONC, &euroNatOptions::OnColorPickedConcorde)
	ON_BN_CLICKED(IDC_CHECK_SHOW_LINES, &euroNatOptions::OnBnClickedCheckShowLines)
	ON_BN_CLICKED(IDC_CHECK_SHOW_LETTERS, &euroNatOptions::OnBnClickedCheckShowLetters)
	ON_BN_CLICKED(IDC_CHECK_EASTBOUND, &euroNatOptions::OnBnClickedCheckEastbound)
	ON_BN_CLICKED(IDC_CHECK_WESTBOUND, &euroNatOptions::OnBnClickedCheckWestbound)
	ON_BN_CLICKED(IDC_CHECK_SHORT_NAMES, &euroNatOptions::OnBnClickedCheckShortNames)
	ON_BN_CLICKED(IDC_CHECK_ANTI_ALIASED, &euroNatOptions::OnBnClickedCheckAntiAliased)
	ON_BN_CLICKED(IDC_CHECK_CONCORD, &euroNatOptions::OnBnClickedCheckConcordTracks)
	ON_CBN_SELCHANGE(IDC_COMBO_LINESTYLE, &euroNatOptions::OnCbSelChangedLinestyle)
END_MESSAGE_MAP()


// euroNatOptions message handlers


void euroNatOptions::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void euroNatOptions::OnBnClickedGetNatData()
{
	if( !NATShow::Loading )
		NATData::LastInstance->Refresh();
}

void euroNatOptions::OnBnClickedLoadDefaults()
{
	NATShow::Defaults();
	this->LoadValues();
}

void euroNatOptions::OnBnClickedCheckShowLines()
{
	NATShow::Lines = (this->m_chkShowLines.GetCheck() > 0);
}

void euroNatOptions::OnColorPickedEastbound()
{
	NATShow::EastboundColor = this->m_colorbtnEastbound.GetColor();
}

void euroNatOptions::OnColorPickedWestbound()
{
	NATShow::WestboundColor = this->m_colorbtnWestbound.GetColor();
}

void euroNatOptions::OnColorPickedConcorde()
{
	NATShow::ConcordeColor = this->m_colorbtnConcorde.GetColor();
}

void euroNatOptions::OnBnClickedCheckShowLetters()
{
	NATShow::Letters = (this->m_chkShowLetters.GetCheck() > 0);
}


void euroNatOptions::OnBnClickedCheckEastbound()
{
	NATShow::Eastbound = (this->m_chkEastbound.GetCheck() > 0);
}


void euroNatOptions::OnBnClickedCheckWestbound()
{
	NATShow::Westbound = (this->m_chkWestbound.GetCheck() > 0);
}

void euroNatOptions::OnCbSelChangedLinestyle()
{
	NATShow::DottedLines = (this->m_cbolinestyle.GetCurSel() == 0);
	this->m_chkAntiAliased.EnableWindow( this->m_cbolinestyle.GetCurSel() == 1 );
}

void euroNatOptions::OnBnClickedCheckShortNames()
{
	NATShow::ShortWPNames = (this->m_chkShortName.GetCheck() > 0);
}

void euroNatOptions::OnBnClickedCheckAntiAliased()
{
	NATShow::AntialiasedLine = (this->m_chkAntiAliased.GetCheck() > 0);
}

void euroNatOptions::OnBnClickedCheckConcordTracks()
{
	NATShow::ConcordTracks = (this->m_chkConcordTracks.GetCheck() > 0);
}

void euroNatOptions::LoadValues()
{
	this->m_chkShowLines.SetCheck( NATShow::Lines );
	this->m_chkEastbound.SetCheck( NATShow::Eastbound );
	this->m_chkWestbound.SetCheck( NATShow::Westbound );
	this->m_chkShowLetters.SetCheck( NATShow::Letters );
	this->m_chkShortName.SetCheck( NATShow::ShortWPNames );
	this->m_chkAntiAliased.SetCheck( NATShow::AntialiasedLine );
	this->m_chkConcordTracks.SetCheck( NATShow::ConcordTracks );
	this->m_cbolinestyle.SetCurSel( (NATShow::DottedLines ? 0 : 1) );
	this->m_chkAntiAliased.EnableWindow( !NATShow::DottedLines );

	this->m_colorbtnEastbound.EnableOtherButton( "More...", false );
	this->m_colorbtnEastbound.SetColor( NATShow::EastboundColor );
	this->m_colorbtnWestbound.EnableOtherButton( "More...", false );
	this->m_colorbtnWestbound.SetColor( NATShow::WestboundColor );
	this->m_colorbtnConcorde.EnableOtherButton( "More...", false );
	this->m_colorbtnConcorde.SetColor( NATShow::ConcordeColor );
}

BOOL euroNatOptions::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	RECT windowrect;
	this->GetWindowRect( &windowrect );
	this->SetWindowPos( NULL, 300, 200, windowrect.right - windowrect.left, windowrect.bottom - windowrect.top, NULL );

	this->LoadValues();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
