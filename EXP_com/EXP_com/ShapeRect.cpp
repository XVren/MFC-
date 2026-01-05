#include "pch.h"
#include "ShapeRect.h"

CShapeRect::CShapeRect()
{
}

CShapeRect::~CShapeRect()
{
}

BOOL CShapeRect::OnRButtonDown(CPoint point)
{
    // 矩形需要2个对角点
    if (GetControlPointCount() >= 2)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CShapeRect::Draw(CDC* pDC)
{
    CPoint p1, p2;

    if (GetControlPointCount() >= 2)
    {
        p1 = m_vecCtrlPts[0];
        p2 = m_vecCtrlPts[1];
    }
    else if (GetControlPointCount() == 1 && m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
    {
        p1 = m_vecCtrlPts[0];
        p2 = m_ptCurrent;
    }
    else
    {
        return;
    }

    // 创建画笔和画刷
    CPen* pPen = m_style.CreatePen();
    CPen* pOldPen = pDC->SelectObject(pPen);
    CBrush* pBrush = m_style.CreateBrush();
    CBrush* pOldBrush = pDC->SelectObject(pBrush);

    // 绘制矩形
    pDC->Rectangle(min(p1.x, p2.x), min(p1.y, p2.y),
        max(p1.x, p2.x), max(p1.y, p2.y));

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    delete pBrush;
    delete pPen;
}

CShape* CShapeRect::Clone() const
{
    CShapeRect* pClone = new CShapeRect();
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    return pClone;
}