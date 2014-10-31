#pragma once


// euroNatOptions dialog

class euroNatOptions : public CDialogEx
{
	DECLARE_DYNAMIC(euroNatOptions)

public:
	euroNatOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~euroNatOptions();

// Dialog Data
	enum { IDD = 3000 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void LoadValues();

	DECLARE_MESSAGE_MAP()
public:
	CButton m_chkShowLetters;
	CButton m_chkEastbound;
	CButton m_chkWestbound;
	CButton m_chkShowLines;
	CButton m_chkShortName;
	CButton m_chkAntiAliased;
	CButton m_chkConcordTracks;

	CMFCColorButton m_colorbtnEastbound;
	CMFCColorButton m_colorbtnWestbound;
	CMFCColorButton m_colorbtnConcorde;

	CComboBox m_cbolinestyle;

	CButton m_cmdGetNatData;
	CButton m_cmdLoadDefaults;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedGetNatData();
	afx_msg void OnBnClickedLoadDefaults();

	afx_msg void OnColorPickedEastbound();
	afx_msg void OnColorPickedWestbound();
	afx_msg void OnColorPickedConcorde();

	afx_msg void OnBnClickedCheckShowLines();
	afx_msg void OnBnClickedCheckShowLetters();
	afx_msg void OnBnClickedCheckEastbound();
	afx_msg void OnBnClickedCheckWestbound();
	afx_msg void OnBnClickedCheckShortNames();
	afx_msg void OnBnClickedCheckAntiAliased();
	afx_msg void OnBnClickedCheckConcordTracks();
	afx_msg void OnCbSelChangedLinestyle();

	virtual BOOL OnInitDialog();
};
