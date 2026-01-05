#include "pch.h"
#include "ShapePolygon.h"

CShapePolygon::CShapePolygon()
{
}

CShapePolygon::~CShapePolygon()
{
}

BOOL CShapePolygon::OnRButtonDown(CPoint point)
{
    // 多边形至少需要3个点
    if (GetControlPointCount() >= 3)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CShapePolygon::Draw(CDC* pDC)
{
    int nPts = GetControlPointCount();
    if (nPts < 3) return;

    // 创建画笔和画刷
    CPen* pPen = m_style.CreatePen();
    CPen* pOldPen = pDC->SelectObject(pPen);
    CBrush* pBrush = m_style.CreateBrush();
    CBrush* pOldBrush = pDC->SelectObject(pBrush);

    // 转换为POINT数组
    std::vector<POINT> pts(nPts);
    for (int i = 0; i < nPts; ++i)
    {
        pts[i].x = m_vecCtrlPts[i].x;
        pts[i].y = m_vecCtrlPts[i].y;
    }

    // 绘制多边形
    pDC->Polygon(pts.data(), nPts);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    delete pBrush;
    delete pPen;
}

void CShapePolygon::DrawPreview(CDC* pDC)
{
    int nPts = GetControlPointCount();

    // 创建画笔
    CPen* pPen = m_style.CreatePen();
    CPen* pOldPen = pDC->SelectObject(pPen);

    // 绘制已有的边
    if (nPts >= 1)
    {
        pDC->MoveTo(m_vecCtrlPts[0]);
        for (int i = 1; i < nPts; ++i)
        {
            pDC->LineTo(m_vecCtrlPts[i]);
        }

        // 橡皮筋效果：连接到当前鼠标位置
        if (m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
        {
            pDC->LineTo(m_ptCurrent);
            // 虚线连接回起点，提示闭合
            CPen penDash(PS_DOT, 1, m_style.GetLineColor());
            pDC->SelectObject(&penDash);
            pDC->LineTo(m_vecCtrlPts[0]);
        }
    }

    pDC->SelectObject(pOldPen);
    delete pPen;

    // 绘制控制点
    DrawControlPoints(pDC);
}

CShape* CShapePolygon::Clone() const
{
    CShapePolygon* pClone = new CShapePolygon();
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    return pClone;
}