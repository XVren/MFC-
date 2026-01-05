#pragma once
#include <afxwin.h>

// 简单输入对话框
class CInputDlg : public CDialog
{
public:
    CString m_strPrompt;
    CString m_strValue;
    CEdit m_edit;
    CStatic m_static;
    CButton m_btnOK;
    CButton m_btnCancel;
    CFont m_font;

    CInputDlg(LPCTSTR prompt, LPCTSTR defaultVal = _T("0"), CWnd* pParent = NULL);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    double GetValue() { return _ttof(m_strValue); }
    CString GetString() { return m_strValue; }
};

// 对称方式选择对话框
class CSymmetryDlg : public CDialog
{
public:
    int m_nChoice;  // 0=水平轴, 1=垂直轴, 2=中心点
    CButton m_btnX, m_btnY, m_btnO, m_btnCancel;
    CStatic m_static;
    CFont m_font;

    CSymmetryDlg(CWnd* pParent = NULL);
    virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

// 辅助函数
BOOL ShowInputDlg(CWnd* pParent, LPCTSTR prompt, double& value);
int ShowSymmetryDlg(CWnd* pParent);