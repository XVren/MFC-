#include "pch.h"
#include "ShapeEllipse.h"
#include <cmath>

CShapeEllipse::CShapeEllipse(bool bCircle)
    : m_bCircle(bCircle)
{
}

CShapeEllipse::~CShapeEllipse()
{
}

EShapeType CShapeEllipse::GetType() const
{
    return m_bCircle ? EShapeType::Circle : EShapeType::Ellipse;
}

BOOL CShapeEllipse::OnRButtonDown(CPoint point)
{
    // 需要2个点（圆心+边界点 或 外接矩形对角点）
    if (GetControlPointCount() >= 2)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CShapeEllipse::DrawPixel(CDC* pDC, int x, int y)
{
    int width = m_style.GetLineWidth();
    COLORREF color = m_style.GetLineColor();

    if (width <= 1)
    {
        pDC->SetPixel(x, y, color);
    }
    else
    {
        int half = width / 2;
        for (int i = -half; i <= half; ++i)
        {
            for (int j = -half; j <= half; ++j)
            {
                pDC->SetPixel(x + i, y + j, color);
            }
        }
    }
}

void CShapeEllipse::DrawCirclePoints(CDC* pDC, int cx, int cy, int x, int y)
{
    // 利用圆的八对称性
    DrawPixel(pDC, cx + x, cy + y);
    DrawPixel(pDC, cx - x, cy + y);
    DrawPixel(pDC, cx + x, cy - y);
    DrawPixel(pDC, cx - x, cy - y);
    DrawPixel(pDC, cx + y, cy + x);
    DrawPixel(pDC, cx - y, cy + x);
    DrawPixel(pDC, cx + y, cy - x);
    DrawPixel(pDC, cx - y, cy - x);
}

void CShapeEllipse::DrawEllipsePoints(CDC* pDC, int cx, int cy, int x, int y)
{
    // 利用椭圆的四对称性
    DrawPixel(pDC, cx + x, cy + y);
    DrawPixel(pDC, cx - x, cy + y);
    DrawPixel(pDC, cx + x, cy - y);
    DrawPixel(pDC, cx - x, cy - y);
}

// 中点圆算法
void CShapeEllipse::DrawCircleMidpoint(CDC* pDC, int cx, int cy, int r)
{
    if (r <= 0) return;

    int x = 0;
    int y = r;
    int d = 1 - r;  // 初始判别式

    DrawCirclePoints(pDC, cx, cy, x, y);

    while (x < y)
    {
        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        DrawCirclePoints(pDC, cx, cy, x, y);
    }
}

// 中点椭圆算法
void CShapeEllipse::DrawEllipseMidpoint(CDC* pDC, int cx, int cy, int rx, int ry)
{
    if (rx <= 0 || ry <= 0) return;

    long long rx2 = (long long)rx * rx;
    long long ry2 = (long long)ry * ry;

    int x = 0;
    int y = ry;

    // 区域1：斜率绝对值 < 1
    long long d1 = ry2 - rx2 * ry + rx2 / 4;

    DrawEllipsePoints(pDC, cx, cy, x, y);

    while (ry2 * x < rx2 * y)
    {
        if (d1 < 0)
        {
            d1 += ry2 * (2 * x + 3);
        }
        else
        {
            d1 += ry2 * (2 * x + 3) + rx2 * (-2 * y + 2);
            y--;
        }
        x++;
        DrawEllipsePoints(pDC, cx, cy, x, y);
    }

    // 区域2：斜率绝对值 >= 1
    long long d2 = ry2 * (x + 1) * (x + 1) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;

    while (y >= 0)
    {
        if (d2 < 0)
        {
            d2 += ry2 * (2 * x + 2) + rx2 * (-2 * y + 3);
            x++;
        }
        else
        {
            d2 += rx2 * (-2 * y + 3);
        }
        y--;
        DrawEllipsePoints(pDC, cx, cy, x, y);
    }
}

void CShapeEllipse::Draw(CDC* pDC)
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

    if (m_bCircle)
    {
        // 圆：p1为圆心，p2为圆上一点
        int cx = p1.x;
        int cy = p1.y;
        int r = (int)sqrt((double)(p2.x - p1.x) * (p2.x - p1.x) +
            (double)(p2.y - p1.y) * (p2.y - p1.y));
        DrawCircleMidpoint(pDC, cx, cy, r);
    }
    else
    {
        // 椭圆：p1和p2为外接矩形对角点
        int cx = (p1.x + p2.x) / 2;
        int cy = (p1.y + p2.y) / 2;
        int rx = abs(p2.x - p1.x) / 2;
        int ry = abs(p2.y - p1.y) / 2;
        DrawEllipseMidpoint(pDC, cx, cy, rx, ry);
    }
}

CShape* CShapeEllipse::Clone() const
{
    CShapeEllipse* pClone = new CShapeEllipse(m_bCircle);
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    return pClone;
}