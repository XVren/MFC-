#include "pch.h"
#include "ShapeStyle.h"

CShapeStyle::CShapeStyle()
    : m_colorLine(RGB(0, 0, 0))
    , m_nLineWidth(1)
    , m_nLineStyle(PS_SOLID)
    , m_colorFill(RGB(255, 0, 0))
    , m_bFilled(false)
{
}

CShapeStyle::CShapeStyle(const CShapeStyle& other)
    : m_colorLine(other.m_colorLine)
    , m_nLineWidth(other.m_nLineWidth)
    , m_nLineStyle(other.m_nLineStyle)
    , m_colorFill(other.m_colorFill)
    , m_bFilled(other.m_bFilled)
{
}

CShapeStyle& CShapeStyle::operator=(const CShapeStyle& other)
{
    if (this != &other)
    {
        m_colorLine = other.m_colorLine;
        m_nLineWidth = other.m_nLineWidth;
        m_nLineStyle = other.m_nLineStyle;
        m_colorFill = other.m_colorFill;
        m_bFilled = other.m_bFilled;
    }
    return *this;
}

CShapeStyle::~CShapeStyle()
{
}

CPen* CShapeStyle::CreatePen() const
{
    CPen* pPen = new CPen();

    if (m_nLineWidth > 1 && m_nLineStyle != PS_SOLID)
    {
        // 粗虚线需要使用扩展画笔
        LOGBRUSH lb;
        lb.lbStyle = BS_SOLID;
        lb.lbColor = m_colorLine;
        lb.lbHatch = 0;

        pPen->CreatePen(PS_GEOMETRIC | m_nLineStyle, m_nLineWidth, &lb);
    }
    else
    {
        pPen->CreatePen(m_nLineStyle, m_nLineWidth, m_colorLine);
    }

    return pPen;
}

CBrush* CShapeStyle::CreateBrush() const
{
    CBrush* pBrush = new CBrush();

    if (m_bFilled)
    {
        pBrush->CreateSolidBrush(m_colorFill);
    }
    else
    {
        pBrush->CreateStockObject(NULL_BRUSH);
    }

    return pBrush;
}

BOOL CShapeStyle::ChooseLineColor(CWnd* pParent)
{
    CColorDialog dlg(m_colorLine, CC_FULLOPEN, pParent);
    if (dlg.DoModal() == IDOK)
    {
        m_colorLine = dlg.GetColor();
        return TRUE;
    }
    return FALSE;
}

BOOL CShapeStyle::ChooseFillColor(CWnd* pParent)
{
    CColorDialog dlg(m_colorFill, CC_FULLOPEN, pParent);
    if (dlg.DoModal() == IDOK)
    {
        m_colorFill = dlg.GetColor();
        return TRUE;
    }
    return FALSE;
}