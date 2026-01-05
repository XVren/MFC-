#include "pch.h"
#include "InputDialog.h"

// ==================== CInputDlg ====================

CInputDlg::CInputDlg(LPCTSTR prompt, LPCTSTR defaultVal, CWnd* pParent)
    : CDialog()
{
    m_strPrompt = prompt;
    m_strValue = defaultVal;
}

BOOL CInputDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_font.CreatePointFont(90, _T("宋体"));

    SetWindowText(_T("输入参数"));
    MoveWindow(0, 0, 310, 140);
    CenterWindow();

    m_static.Create(m_strPrompt, WS_CHILD | WS_VISIBLE, CRect(15, 15, 280, 35), this);
    m_static.SetFont(&m_font);

    m_edit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
        CRect(15, 45, 280, 70), this, 1001);
    m_edit.SetFont(&m_font);
    m_edit.SetWindowText(m_strValue);
    m_edit.SetFocus();
    m_edit.SetSel(0, -1);

    m_btnOK.Create(_T("确定"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        CRect(60, 80, 140, 105), this, IDOK);
    m_btnOK.SetFont(&m_font);

    m_btnCancel.Create(_T("取消"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(160, 80, 240, 105), this, IDCANCEL);
    m_btnCancel.SetFont(&m_font);

    return FALSE;
}

void CInputDlg::OnOK()
{
    m_edit.GetWindowText(m_strValue);
    CDialog::OnOK();
}

// ==================== CSymmetryDlg ====================

CSymmetryDlg::CSymmetryDlg(CWnd* pParent)
    : CDialog()
{
    m_nChoice = -1;
}

BOOL CSymmetryDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_font.CreatePointFont(90, _T("宋体"));

    SetWindowText(_T("选择对称方式"));
    MoveWindow(0, 0, 280, 180);
    CenterWindow();

    m_static.Create(_T("请选择对称方式："), WS_CHILD | WS_VISIBLE,
        CRect(15, 15, 260, 35), this);
    m_static.SetFont(&m_font);

    m_btnX.Create(_T("关于水平轴对称"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(30, 45, 240, 70), this, 2001);
    m_btnX.SetFont(&m_font);

    m_btnY.Create(_T("关于垂直轴对称"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(30, 75, 240, 100), this, 2002);
    m_btnY.SetFont(&m_font);

    m_btnO.Create(_T("关于中心点对称"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(30, 105, 240, 130), this, 2003);
    m_btnO.SetFont(&m_font);

    m_btnCancel.Create(_T("取消"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        CRect(100, 140, 180, 165), this, IDCANCEL);
    m_btnCancel.SetFont(&m_font);

    return TRUE;
}

BOOL CSymmetryDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case 2001: m_nChoice = 0; EndDialog(IDOK); return TRUE;
    case 2002: m_nChoice = 1; EndDialog(IDOK); return TRUE;
    case 2003: m_nChoice = 2; EndDialog(IDOK); return TRUE;
    }
    return CDialog::OnCommand(wParam, lParam);
}

// ==================== 辅助函数 ====================

BOOL ShowInputDlg(CWnd* pParent, LPCTSTR prompt, double& value)
{
    struct {
        DLGTEMPLATE tmpl;
        WORD menu, wndClass, title;
    } dlgData = { 0 };

    dlgData.tmpl.style = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER;
    dlgData.tmpl.cx = 155;
    dlgData.tmpl.cy = 70;

    CInputDlg dlg(prompt, _T("0"), pParent);
    dlg.InitModalIndirect(&dlgData.tmpl, pParent);

    if (dlg.DoModal() == IDOK)
    {
        value = dlg.GetValue();
        return TRUE;
    }
    return FALSE;
}

int ShowSymmetryDlg(CWnd* pParent)
{
    struct {
        DLGTEMPLATE tmpl;
        WORD menu, wndClass, title;
    } dlgData = { 0 };

    dlgData.tmpl.style = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER;
    dlgData.tmpl.cx = 140;
    dlgData.tmpl.cy = 90;

    CSymmetryDlg dlg(pParent);
    dlg.InitModalIndirect(&dlgData.tmpl, pParent);

    if (dlg.DoModal() == IDOK)
    {
        return dlg.m_nChoice;
    }
    return -1;
}