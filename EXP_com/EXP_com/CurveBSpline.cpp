#include "pch.h"
#include "CurveBSpline.h"
#include <cmath>

CCurveBSpline::CCurveBSpline(int degree)
    : m_nDegree(degree)
{
}

CCurveBSpline::~CCurveBSpline()
{
}

BOOL CCurveBSpline::OnRButtonDown(CPoint point)
{
    // B样条至少需要 (degree + 1) 个控制点
    if (GetControlPointCount() >= m_nDegree + 1)
    {
        m_bFinished = TRUE;
        return TRUE;
    }
    return FALSE;
}

// 使用 de Boor 算法计算B样条曲线上的点
CPoint CCurveBSpline::DeBoor(double t, const std::vector<CPoint>& ctrlPts, const std::vector<double>& knots) const
{
    int n = (int)ctrlPts.size() - 1;  // 控制点索引最大值
    int p = m_nDegree;                 // 次数

    // 找到 t 所在的节点区间 [knots[k], knots[k+1])
    int k = p;
    for (int i = p; i <= n; ++i)
    {
        if (t >= knots[i] && t < knots[i + 1])
        {
            k = i;
            break;
        }
    }
    // 处理 t = 1 的特殊情况
    if (fabs(t - 1.0) < 1e-10)
    {
        k = n;
    }

    // de Boor 算法
    // 复制相关控制点
    std::vector<double> dx(p + 1), dy(p + 1);
    for (int i = 0; i <= p; ++i)
    {
        int idx = k - p + i;
        if (idx < 0) idx = 0;
        if (idx > n) idx = n;
        dx[i] = ctrlPts[idx].x;
        dy[i] = ctrlPts[idx].y;
    }

    // 迭代计算
    for (int r = 1; r <= p; ++r)
    {
        for (int i = p; i >= r; --i)
        {
            int idx = k - p + i;
            double denom = knots[idx + p - r + 1] - knots[idx];
            double alpha = 0.0;
            if (fabs(denom) > 1e-10)
            {
                alpha = (t - knots[idx]) / denom;
            }
            dx[i] = (1.0 - alpha) * dx[i - 1] + alpha * dx[i];
            dy[i] = (1.0 - alpha) * dy[i - 1] + alpha * dy[i];
        }
    }

    return CPoint((int)round(dx[p]), (int)round(dy[p]));
}

void CCurveBSpline::Draw(CDC* pDC)
{
    // 获取控制点
    std::vector<CPoint> pts = m_vecCtrlPts;

    // 如果正在绘制中，且鼠标位置有效，临时加入当前鼠标位置
    if (!m_bFinished && m_ptCurrent.x > 0 && m_ptCurrent.y > 0 && !pts.empty())
    {
        pts.push_back(m_ptCurrent);
    }

    // B样条至少需要 degree+1 个控制点
    int nPts = (int)pts.size();
    if (nPts < m_nDegree + 1)
        return;

    // 生成夹紧(Clamped)均匀节点向量
    int p = m_nDegree;
    int n = nPts - 1;  // 控制点索引最大值
    int m = n + p + 1; // 节点向量最大索引

    std::vector<double> knots(m + 1, 0.0);

    // 前 p+1 个节点为 0
    for (int i = 0; i <= p; ++i)
    {
        knots[i] = 0.0;
    }

    // 后 p+1 个节点为 1
    for (int i = m - p; i <= m; ++i)
    {
        knots[i] = 1.0;
    }

    // 中间节点均匀分布
    int numInternal = n - p;  // 内部节点数量
    for (int i = 1; i <= numInternal; ++i)
    {
        knots[p + i] = (double)i / (numInternal + 1);
    }

    // 创建画笔
    CPen* pPen = m_style.CreatePen();
    CPen* pOldPen = pDC->SelectObject(pPen);

    // 计算采样点数
    int nSteps = nPts * 50;

    // 绘制B样条曲线
    bool bFirst = true;
    for (int i = 0; i <= nSteps; ++i)
    {
        double t = (double)i / nSteps;

        // 确保 t 在 [0, 1] 范围内
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;

        CPoint pt = DeBoor(t, pts, knots);

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

CShape* CCurveBSpline::Clone() const
{
    CCurveBSpline* pClone = new CCurveBSpline(m_nDegree);
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    pClone->m_vecKnots = m_vecKnots;
    return pClone;
}