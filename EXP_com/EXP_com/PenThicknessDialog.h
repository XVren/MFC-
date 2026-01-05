#pragma once
#include <afxwin.h>
#include <afxcmn.h>

class CPenThicknessDialog : public CDialog
{
public:
    CPenThicknessDialog(int initialWidth, CWnd* pParent = nullptr);

    int GetSelectedWidth() const { return m_nSelectedWidth; }

protected:
    virtual BOOL OnInitDialog() override;
    virtual void DoDataExchange(CDataExchange* pDX) override;
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP();

private:
    void UpdatePreview();
    void UpdateValueText();

    int m_nSelectedWidth;
    CSliderCtrl m_slider;
    CStatic m_txtLabel;
    CStatic m_txtValue;
    CButton m_btnOK;
    CButton m_btnCancel;
    CRect m_rcPreview;
    CFont m_titleFont;
};

bool ShowPenThicknessDialog(CWnd* pParent, int currentWidth, int& selectedWidth);
