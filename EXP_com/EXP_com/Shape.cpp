#include "pch.h"
#include "Shape.h"

CShape::CShape()
    : m_bFinished(FALSE)
    , m_bSelected(FALSE)
    , m_ptCurrent(-1, -1)
{
}

CShape::~CShape()
{
}

void CShape::OnLButtonDown(CPoint point)
{
    AddControlPoint(point);
}

void CShape::OnMouseMove(CPoint point)
{
    m_ptCurrent = point;
}

BOOL CShape::OnRButtonDown(CPoint point)
{
    if (GetControlPointCount() >= 2)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CShape::DrawPreview(CDC* pDC)
{
    Draw(pDC);
    DrawControlPoints(pDC);
}

void CShape::AddControlPoint(CPoint pt)
{
    m_vecCtrlPts.push_back(pt);
}

void CShape::ClearControlPoints()
{
    m_vecCtrlPts.clear();
    m_bFinished = FALSE;
    m_ptCurrent = CPoint(-1, -1);
}

void CShape::SetControlPoint(int index, CPoint pt)
{
    if (index >= 0 && index < (int)m_vecCtrlPts.size())
    {
        m_vecCtrlPts[index] = pt;
    }
}

CPoint CShape::GetControlPoint(int index) const
{
    if (index >= 0 && index < (int)m_vecCtrlPts.size())
    {
        return m_vecCtrlPts[index];
    }
    return CPoint(0, 0);
}

CRect CShape::GetBoundingBox()
{
    if (m_vecCtrlPts.empty())
        return CRect(0, 0, 0, 0);

    int minX = m_vecCtrlPts[0].x, maxX = m_vecCtrlPts[0].x;
    int minY = m_vecCtrlPts[0].y, maxY = m_vecCtrlPts[0].y;

    for (size_t i = 1; i < m_vecCtrlPts.size(); ++i)
    {
        if (m_vecCtrlPts[i].x < minX) minX = m_vecCtrlPts[i].x;
        if (m_vecCtrlPts[i].x > maxX) maxX = m_vecCtrlPts[i].x;
        if (m_vecCtrlPts[i].y < minY) minY = m_vecCtrlPts[i].y;
        if (m_vecCtrlPts[i].y > maxY) maxY = m_vecCtrlPts[i].y;
    }

    return CRect(minX, minY, maxX, maxY);
}

CPoint CShape::GetCenter()
{
    if (m_vecCtrlPts.empty())
        return CPoint(0, 0);

    double cx = 0, cy = 0;
    for (size_t i = 0; i < m_vecCtrlPts.size(); ++i)
    {
        cx += m_vecCtrlPts[i].x;
        cy += m_vecCtrlPts[i].y;
    }
    cx /= m_vecCtrlPts.size();
    cy /= m_vecCtrlPts.size();

    return CPoint((int)cx, (int)cy);
}

bool CShape::HitTest(CPoint point, int tolerance)
{
    CRect rc = GetBoundingBox();
    rc.InflateRect(tolerance, tolerance);
    return rc.PtInRect(point) != FALSE;
}

bool CShape::HitTestRect(CRect rect)
{
    rect.NormalizeRect();
    CRect bbox = GetBoundingBox();
    CRect intersect;
    return intersect.IntersectRect(&bbox, &rect) != FALSE;
}

// ʹ3x3б任
void CShape::Transform(double matrix[3][3])
{
    for (size_t i = 0; i < m_vecCtrlPts.size(); ++i)
    {
        double x = matrix[0][0] * m_vecCtrlPts[i].x + matrix[0][1] * m_vecCtrlPts[i].y + matrix[0][2];
        double y = matrix[1][0] * m_vecCtrlPts[i].x + matrix[1][1] * m_vecCtrlPts[i].y + matrix[1][2];
        m_vecCtrlPts[i].x = (int)(x + 0.5);
        m_vecCtrlPts[i].y = (int)(y + 0.5);
    }
}

void CShape::DrawControlPoints(CDC* pDC)
{
    if (m_vecCtrlPts.empty())
        return;

    CBrush brushAccent(RGB(0, 158, 142));
    CBrush* pOldBrush = pDC->SelectObject(&brushAccent);
    CPen penFrame(PS_SOLID, 1, RGB(33, 37, 41));
    CPen* pOldPen = pDC->SelectObject(&penFrame);

    const int nSize = 4;
    for (size_t i = 0; i < m_vecCtrlPts.size(); ++i)
    {
        pDC->Rectangle(m_vecCtrlPts[i].x - nSize, m_vecCtrlPts[i].y - nSize,
            m_vecCtrlPts[i].x + nSize, m_vecCtrlPts[i].y + nSize);
    }

    // ƿƶ
    if (m_vecCtrlPts.size() >= 1)
    {
        CPen penGray(PS_DOT, 1, RGB(160, 170, 180));
        pDC->SelectObject(&penGray);
        pDC->MoveTo(m_vecCtrlPts[0]);

        for (size_t i = 1; i < m_vecCtrlPts.size(); ++i)
        {
            pDC->LineTo(m_vecCtrlPts[i]);
        }

        if (!m_bFinished && m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
        {
            pDC->LineTo(m_ptCurrent);
        }
    }

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
}

void CShape::DrawSelectionHandles(CDC* pDC)
{
    if (!m_bSelected)
        return;

    CRect bbox = GetBoundingBox();
    bbox.InflateRect(2, 2);

    CPen penDash(PS_DASH, 1, RGB(64, 128, 255));
    CPen* pOldPen = pDC->SelectObject(&penDash);
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    pDC->Rectangle(&bbox);

    CBrush brushBlue(RGB(80, 140, 255));
    pDC->SelectObject(&brushBlue);
    pDC->SelectObject((CPen*)pDC->SelectStockObject(BLACK_PEN));

    const int hs = 5;
    CPoint handles[8] = {
        CPoint(bbox.left, bbox.top),
        CPoint((bbox.left + bbox.right) / 2, bbox.top),
        CPoint(bbox.right, bbox.top),
        CPoint(bbox.right, (bbox.top + bbox.bottom) / 2),
        CPoint(bbox.right, bbox.bottom),
        CPoint((bbox.left + bbox.right) / 2, bbox.bottom),
        CPoint(bbox.left, bbox.bottom),
        CPoint(bbox.left, (bbox.top + bbox.bottom) / 2)
    };

    for (int i = 0; i < 8; ++i)
    {
        pDC->Rectangle(handles[i].x - hs, handles[i].y - hs,
            handles[i].x + hs, handles[i].y + hs);
    }

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}