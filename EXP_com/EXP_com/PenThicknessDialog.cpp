#include "pch.h"
#include "PenThicknessDialog.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPenThicknessDialog::CPenThicknessDialog(int initialWidth, CWnd* pParent)
    : CDialog()
    , m_nSelectedWidth(initialWidth)
{
}

BEGIN_MESSAGE_MAP(CPenThicknessDialog, CDialog)
    ON_WM_HSCROLL()
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CPenThicknessDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BOOL CPenThicknessDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_titleFont.CreatePointFont(95, _T("Microsoft YaHei"));

    SetWindowText(_T("选择画笔粗细"));
    MoveWindow(0, 0, 380, 210);
    CenterWindow();

    m_txtLabel.Create(_T("画笔粗细"), WS_CHILD | WS_VISIBLE, CRect(20, 18, 200, 42), this);
    m_txtLabel.SetFont(&m_titleFont);

    m_txtValue.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_RIGHT, CRect(220, 18, 340, 42), this);
    m_txtValue.SetFont(&m_titleFont);

    m_slider.Create(WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS, CRect(20, 60, 340, 90), this, 3001);
    m_slider.SetRange(1, 12);
    m_slider.SetTicFreq(1);
    m_nSelectedWidth = std::clamp(m_nSelectedWidth, 1, 12);
    m_slider.SetPos(m_nSelectedWidth);

    m_btnOK.Create(_T("确定"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, CRect(90, 165, 170, 190), this, IDOK);
    m_btnOK.SetFont(&m_titleFont);

    m_btnCancel.Create(_T("取消"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(190, 165, 270, 190), this, IDCANCEL);
    m_btnCancel.SetFont(&m_titleFont);

    m_rcPreview = CRect(20, 110, 340, 160);

    UpdateValueText();
    UpdatePreview();

    return TRUE;
}

void CPenThicknessDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

    if (pScrollBar == (CScrollBar*)&m_slider)
    {
        m_nSelectedWidth = m_slider.GetPos();
        UpdateValueText();
        UpdatePreview();
    }
}

void CPenThicknessDialog::OnPaint()
{
    CPaintDC dc(this);
    CRect rcPreview = m_rcPreview;

    // 背景
    dc.FillSolidRect(rcPreview, RGB(248, 250, 252));
    dc.DrawEdge(rcPreview, EDGE_SUNKEN, BF_RECT);

    // 预览线条
    int centerY = (rcPreview.top + rcPreview.bottom) / 2;
    CPen pen(PS_SOLID, m_nSelectedWidth, RGB(64, 128, 255));
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.MoveTo(rcPreview.left + 15, centerY);
    dc.LineTo(rcPreview.right - 15, centerY);
    dc.SelectObject(pOldPen);
}

void CPenThicknessDialog::UpdateValueText()
{
    CString text;
    text.Format(_T("%d px"), m_nSelectedWidth);
    m_txtValue.SetWindowText(text);
}

void CPenThicknessDialog::UpdatePreview()
{
    InvalidateRect(m_rcPreview, FALSE);
}

bool ShowPenThicknessDialog(CWnd* pParent, int currentWidth, int& selectedWidth)
{
    struct
    {
        DLGTEMPLATE tmpl;
        WORD menu, wndClass, title;
    } dlgData = { 0 };

    dlgData.tmpl.style = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER;
    dlgData.tmpl.cx = 200;
    dlgData.tmpl.cy = 115;

    CPenThicknessDialog dlg(currentWidth, pParent);
    dlg.InitModalIndirect(&dlgData.tmpl, pParent);

    if (dlg.DoModal() == IDOK)
    {
        selectedWidth = dlg.GetSelectedWidth();
        return true;
    }
    return false;
}
