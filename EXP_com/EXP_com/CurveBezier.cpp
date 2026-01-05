#include "pch.h"
#include "CurveBezier.h"
#include <cmath>

CCurveBezier::CCurveBezier()
{
}

CCurveBezier::~CCurveBezier()
{
}

void CCurveBezier::Draw(CDC* pDC)
{
    // 获取控制点
    std::vector<CPoint> pts = m_vecCtrlPts;

    // 如果正在绘制中，且鼠标位置有效，临时加入当前鼠标位置
    if (!m_bFinished && m_ptCurrent.x > 0 && m_ptCurrent.y > 0 && !pts.empty())
    {
        pts.push_back(m_ptCurrent);
    }

    // 至少需要2个点
    int nPts = (int)pts.size();
    if (nPts < 2)
        return;

    // 创建画笔
    CPen* pPen = m_style.CreatePen();
    CPen* pOldPen = pDC->SelectObject(pPen);

    // 计算曲线采样点数
    int nSteps = nPts * 50;
    int n = nPts - 1;  // 曲线阶数

    // 绘制贝塞尔曲线
    bool bFirst = true;
    for (int i = 0; i <= nSteps; ++i)
    {
        double t = (double)i / nSteps;

        // 计算曲线上的点
        double x = 0.0, y = 0.0;
        for (int j = 0; j <= n; ++j)
        {
            double basis = Bernstein(j, n, t);
            x += basis * pts[j].x;
            y += basis * pts[j].y;
        }

        CPoint pt((int)round(x), (int)round(y));

        if (bFirst)
        {
            pDC->MoveTo(pt);
            bFirst = false;
        }
        else
        {
            pDC->LineTo(pt);
        }
    }

    // 清理
    pDC->SelectObject(pOldPen);
    delete pPen;
}

CPoint CCurveBezier::CalculatePoint(double t) const
{
    int n = GetControlPointCount() - 1;
    if (n < 0) return CPoint(0, 0);

    double x = 0.0, y = 0.0;
    for (int i = 0; i <= n; ++i)
    {
        double basis = Bernstein(i, n, t);
        x += basis * m_vecCtrlPts[i].x;
        y += basis * m_vecCtrlPts[i].y;
    }

    return CPoint((int)round(x), (int)round(y));
}

double CCurveBezier::Combination(int n, int k)
{
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;

    // 使用乘法公式避免大数溢出
    double result = 1;
    for (int i = 0; i < k; ++i)
    {
        result *= (double)(n - i) / (i + 1);
    }
    return result;
}

double CCurveBezier::Bernstein(int i, int n, double t)
{
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return Combination(n, i) * pow(t, i) * pow(1.0 - t, n - i);
}

CShape* CCurveBezier::Clone() const
{
    CCurveBezier* pClone = new CCurveBezier();
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    return pClone;
}