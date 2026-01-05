#include "pch.h"
#include "ShapeLine.h"
#include <cmath>

CShapeLine::CShapeLine(ELineAlgorithm algorithm)
    : m_algorithm(algorithm)
{
}

CShapeLine::~CShapeLine()
{
}

EShapeType CShapeLine::GetType() const
{
    switch (m_algorithm)
    {
    case ELineAlgorithm::DDA:       return EShapeType::LineDDA;
    case ELineAlgorithm::Midpoint:  return EShapeType::LineMidpoint;
    case ELineAlgorithm::Bresenham: return EShapeType::LineBresenham;
    default:                        return EShapeType::LineDDA;
    }
}

BOOL CShapeLine::OnRButtonDown(CPoint point)
{
    // 线段只需要2个点
    if (GetControlPointCount() >= 2)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CShapeLine::DrawPixel(CDC* pDC, int x, int y)
{
    int width = m_style.GetLineWidth();
    COLORREF color = m_style.GetLineColor();

    if (width <= 1)
    {
        pDC->SetPixel(x, y, color);
    }
    else
    {
        // 绘制粗线（以像素为中心的方块）
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

// DDA算法
void CShapeLine::DrawDDA(CDC* pDC, CPoint p1, CPoint p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    int steps = max(abs(dx), abs(dy));
    if (steps == 0)
    {
        DrawPixel(pDC, p1.x, p1.y);
        return;
    }

    double xInc = (double)dx / steps;
    double yInc = (double)dy / steps;

    double x = p1.x;
    double y = p1.y;

    for (int i = 0; i <= steps; ++i)
    {
        DrawPixel(pDC, (int)round(x), (int)round(y));
        x += xInc;
        y += yInc;
    }
}

// 中点画线算法
void CShapeLine::DrawMidpoint(CDC* pDC, CPoint p1, CPoint p2)
{
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int x = x1, y = y1;

    if (dx >= dy)
    {
        // 斜率绝对值 <= 1
        int d = dy * 2 - dx;  // 初始判别式
        int dE = dy * 2;       // 选E时的增量
        int dNE = (dy - dx) * 2;  // 选NE时的增量

        DrawPixel(pDC, x, y);
        while (x != x2)
        {
            if (d < 0)
            {
                d += dE;
            }
            else
            {
                d += dNE;
                y += sy;
            }
            x += sx;
            DrawPixel(pDC, x, y);
        }
    }
    else
    {
        // 斜率绝对值 > 1，交换x和y的角色
        int d = dx * 2 - dy;
        int dE = dx * 2;
        int dNE = (dx - dy) * 2;

        DrawPixel(pDC, x, y);
        while (y != y2)
        {
            if (d < 0)
            {
                d += dE;
            }
            else
            {
                d += dNE;
                x += sx;
            }
            y += sy;
            DrawPixel(pDC, x, y);
        }
    }
}

// Bresenham算法
void CShapeLine::DrawBresenham(CDC* pDC, CPoint p1, CPoint p2)
{
    int x1 = p1.x, y1 = p1.y;
    int x2 = p2.x, y2 = p2.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;
    int x = x1, y = y1;

    while (true)
    {
        DrawPixel(pDC, x, y);

        if (x == x2 && y == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void CShapeLine::Draw(CDC* pDC)
{
    // 获取起点和终点
    CPoint p1, p2;

    if (GetControlPointCount() >= 2)
    {
        p1 = m_vecCtrlPts[0];
        p2 = m_vecCtrlPts[1];
    }
    else if (GetControlPointCount() == 1 && m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
    {
        // 橡皮筋效果
        p1 = m_vecCtrlPts[0];
        p2 = m_ptCurrent;
    }
    else
    {
        return;
    }

    // 根据算法类型选择绘制方法
    switch (m_algorithm)
    {
    case ELineAlgorithm::DDA:
        DrawDDA(pDC, p1, p2);
        break;
    case ELineAlgorithm::Midpoint:
        DrawMidpoint(pDC, p1, p2);
        break;
    case ELineAlgorithm::Bresenham:
        DrawBresenham(pDC, p1, p2);
        break;
    }
}

CShape* CShapeLine::Clone() const
{
    CShapeLine* pClone = new CShapeLine(m_algorithm);
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    return pClone;
}