#include "pch.h"
#include "ShapeFill.h"
#include <stack>
#include <algorithm>

CShapeFill::CShapeFill(bool bSeedFill)
    : m_bSeedFill(bSeedFill)
    , m_fillColor(RGB(255, 0, 0))      // 默认红色填充
    , m_borderColor(RGB(0, 0, 0))      // 默认黑色边界
{
}

CShapeFill::~CShapeFill()
{
}

EShapeType CShapeFill::GetType() const
{
    return m_bSeedFill ? EShapeType::FillSeed : EShapeType::FillScanLine;
}

BOOL CShapeFill::OnRButtonDown(CPoint point)
{
    if (m_bSeedFill)
    {
        // 种子填充：只需要1个种子点
        if (GetControlPointCount() >= 1)
        {
            m_bFinished = TRUE;
            return TRUE;
        }
    }
    else
    {
        // 扫描线填充：需要至少3个点（多边形）
        if (GetControlPointCount() >= 3)
        {
            m_bFinished = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

void CShapeFill::DoFill(CDC* pDC)
{
    if (m_bSeedFill)
    {
        if (GetControlPointCount() >= 1)
        {
            CPoint seed = m_vecCtrlPts[0];
            SeedScanLineFill(pDC, seed.x, seed.y, m_borderColor, m_fillColor);
        }
    }
    else
    {
        if (GetControlPointCount() >= 3)
        {
            PolygonScanLineFill(pDC, m_fillColor);
        }
    }
}

void CShapeFill::Draw(CDC* pDC)
{
    if (!m_bFinished) return;

    DoFill(pDC);
}

void CShapeFill::DrawPreview(CDC* pDC)
{
    if (m_bSeedFill)
    {
        // 种子填充预览：显示种子点位置
        if (GetControlPointCount() >= 1)
        {
            CPoint pt = m_vecCtrlPts[0];
            // 绘制十字标记
            CPen pen(PS_SOLID, 2, m_fillColor);
            CPen* pOldPen = pDC->SelectObject(&pen);
            pDC->MoveTo(pt.x - 10, pt.y);
            pDC->LineTo(pt.x + 10, pt.y);
            pDC->MoveTo(pt.x, pt.y - 10);
            pDC->LineTo(pt.x, pt.y + 10);
            pDC->SelectObject(pOldPen);
        }
        else if (m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
        {
            // 显示当前鼠标位置的十字
            CPen pen(PS_DOT, 1, m_fillColor);
            CPen* pOldPen = pDC->SelectObject(&pen);
            pDC->MoveTo(m_ptCurrent.x - 10, m_ptCurrent.y);
            pDC->LineTo(m_ptCurrent.x + 10, m_ptCurrent.y);
            pDC->MoveTo(m_ptCurrent.x, m_ptCurrent.y - 10);
            pDC->LineTo(m_ptCurrent.x, m_ptCurrent.y + 10);
            pDC->SelectObject(pOldPen);
        }
    }
    else
    {
        // 扫描线填充预览：显示多边形轮廓
        int nPts = GetControlPointCount();
        if (nPts >= 1)
        {
            CPen pen(PS_SOLID, 1, m_style.GetLineColor());
            CPen* pOldPen = pDC->SelectObject(&pen);

            pDC->MoveTo(m_vecCtrlPts[0]);
            for (int i = 1; i < nPts; ++i)
            {
                pDC->LineTo(m_vecCtrlPts[i]);
            }

            // 橡皮筋效果
            if (m_ptCurrent.x > 0 && m_ptCurrent.y > 0)
            {
                pDC->LineTo(m_ptCurrent);
                // 虚线连回起点
                CPen penDash(PS_DOT, 1, m_fillColor);
                pDC->SelectObject(&penDash);
                pDC->LineTo(m_vecCtrlPts[0]);
            }

            pDC->SelectObject(pOldPen);
        }

        DrawControlPoints(pDC);
    }
}

// ==================== 种子扫描线填充算法 ====================

void CShapeFill::SeedScanLineFill(CDC* pDC, int x, int y, COLORREF borderColor, COLORREF fillColor)
{
    // 获取起始点的颜色作为"要被替换的颜色"
    COLORREF targetColor = pDC->GetPixel(x, y);

    // 如果起始点已经是填充色，直接返回
    if (targetColor == fillColor)
    {
        return;
    }

    // 如果起始点颜色无效（可能点击在窗口外）
    if (targetColor == CLR_INVALID)
    {
        return;
    }

    std::stack<CPoint> stk;
    stk.push(CPoint(x, y));

    while (!stk.empty())
    {
        CPoint p = stk.top();
        stk.pop();

        int px = p.x;
        int py = p.y;

        // 检查当前点颜色
        COLORREF curColor = pDC->GetPixel(px, py);
        if (curColor != targetColor)
        {
            continue;
        }

        // 填充当前点
        pDC->SetPixel(px, py, fillColor);

        // 向右扫描填充
        int xr = px + 1;
        while (pDC->GetPixel(xr, py) == targetColor)
        {
            pDC->SetPixel(xr, py, fillColor);
            xr++;
        }
        xr--;

        // 向左扫描填充
        int xl = px - 1;
        while (pDC->GetPixel(xl, py) == targetColor)
        {
            pDC->SetPixel(xl, py, fillColor);
            xl--;
        }
        xl++;

        // 检查上下两条扫描线
        for (int dy = -1; dy <= 1; dy += 2)
        {
            int ny = py + dy;
            int nx = xr;

            while (nx >= xl)
            {
                int flag = 0;
                int xid = 0;

                // 查找连通区间
                while (nx >= xl && pDC->GetPixel(nx, ny) == targetColor)
                {
                    if (flag == 0)
                    {
                        flag = 1;
                        xid = nx;
                    }
                    nx--;
                }

                if (flag == 1)
                {
                    stk.push(CPoint(xid, ny));
                }

                // 跳过非目标颜色区域
                while (nx >= xl && pDC->GetPixel(nx, ny) != targetColor)
                {
                    nx--;
                }
            }
        }
    }
}

// ==================== 多边形扫描线填充算法 ====================

EDGE* CShapeFill::BuildEdgeTable()
{
    int nPts = GetControlPointCount();
    if (nPts < 3) return nullptr;

    EDGE* pHead = nullptr;

    for (int i = 0; i < nPts; ++i)
    {
        CPoint p1 = m_vecCtrlPts[i];
        CPoint p2 = m_vecCtrlPts[(i + 1) % nPts];

        // 跳过水平边
        if (p1.y == p2.y) continue;

        EDGE* pEdge = new EDGE();

        // 确保 p1.y < p2.y
        if (p1.y > p2.y)
        {
            std::swap(p1, p2);
        }

        pEdge->ymin = p1.y;
        pEdge->ymax = p2.y;
        pEdge->xmin = (double)p1.x;
        pEdge->fm = (double)(p2.x - p1.x) / (p2.y - p1.y);

        // 插入到边表
        InsertEdge(pHead, pEdge);
    }

    return pHead;
}

void CShapeFill::InsertEdge(EDGE*& pHead, EDGE* pNew)
{
    if (pHead == nullptr)
    {
        pHead = pNew;
        return;
    }

    // 按ymin排序插入
    if (pNew->ymin < pHead->ymin)
    {
        pNew->next = pHead;
        pHead->prev = pNew;
        pHead = pNew;
        return;
    }

    EDGE* p = pHead;
    while (p->next != nullptr && p->next->ymin <= pNew->ymin)
    {
        p = p->next;
    }

    pNew->next = p->next;
    if (p->next != nullptr)
    {
        p->next->prev = pNew;
    }
    p->next = pNew;
    pNew->prev = p;
}

void CShapeFill::SortEdgeTable(EDGE*& pHead)
{
    if (pHead == nullptr || pHead->next == nullptr) return;

    // 简单冒泡排序，按ymin排序
    bool swapped;
    do
    {
        swapped = false;
        EDGE* p = pHead;
        while (p->next != nullptr)
        {
            if (p->ymin > p->next->ymin)
            {
                // 交换数据
                std::swap(p->ymin, p->next->ymin);
                std::swap(p->ymax, p->next->ymax);
                std::swap(p->xmin, p->next->xmin);
                std::swap(p->fm, p->next->fm);
                swapped = true;
            }
            p = p->next;
        }
    } while (swapped);
}

void CShapeFill::SortAET(EDGE*& pAET)
{
    if (pAET == nullptr || pAET->next == nullptr) return;

    // 按xmin排序
    bool swapped;
    do
    {
        swapped = false;
        EDGE* p = pAET;
        while (p->next != nullptr)
        {
            if (p->xmin > p->next->xmin)
            {
                std::swap(p->ymin, p->next->ymin);
                std::swap(p->ymax, p->next->ymax);
                std::swap(p->xmin, p->next->xmin);
                std::swap(p->fm, p->next->fm);
                swapped = true;
            }
            p = p->next;
        }
    } while (swapped);
}

void CShapeFill::PolygonScanLineFill(CDC* pDC, COLORREF fillColor)
{
    EDGE* pET = BuildEdgeTable();
    if (pET == nullptr) return;

    EDGE* pAET = nullptr;
    int y = pET->ymin;

    // 找最大ymax
    int ymax = pET->ymax;
    EDGE* temp = pET;
    while (temp != nullptr)
    {
        if (temp->ymax > ymax) ymax = temp->ymax;
        temp = temp->next;
    }

    // 扫描
    while (y < ymax)
    {
        // 将 ymin == y 的边加入AET
        while (pET != nullptr && pET->ymin == y)
        {
            EDGE* pNew = pET;
            pET = pET->next;
            pNew->next = nullptr;
            pNew->prev = nullptr;

            if (pAET == nullptr)
            {
                pAET = pNew;
            }
            else
            {
                EDGE* pLast = pAET;
                while (pLast->next != nullptr) pLast = pLast->next;
                pLast->next = pNew;
                pNew->prev = pLast;
            }
        }

        if (pAET == nullptr)
        {
            y++;
            continue;
        }

        // 删除 ymax <= y 的边
        EDGE* pCur = pAET;
        while (pCur != nullptr)
        {
            if (pCur->ymax <= y)
            {
                EDGE* pDel = pCur;
                if (pCur->prev == nullptr)
                {
                    pAET = pCur->next;
                }
                else
                {
                    pCur->prev->next = pCur->next;
                }
                if (pCur->next != nullptr)
                {
                    pCur->next->prev = pCur->prev;
                }
                pCur = pCur->next;
                delete pDel;
            }
            else
            {
                pCur = pCur->next;
            }
        }

        if (pAET == nullptr)
        {
            y++;
            continue;
        }

        // 排序AET
        SortAET(pAET);

        // 填充
        EDGE* pFill = pAET;
        while (pFill != nullptr && pFill->next != nullptr)
        {
            int x1 = (int)(pFill->xmin + 0.5);
            int x2 = (int)(pFill->next->xmin + 0.5);

            for (int i = x1; i <= x2; ++i)
            {
                pDC->SetPixel(i, y, fillColor);
            }

            pFill = pFill->next->next;
        }

        // 更新x坐标
        pCur = pAET;
        while (pCur != nullptr)
        {
            pCur->xmin += pCur->fm;
            pCur = pCur->next;
        }

        y++;
    }

    // 释放内存
    while (pAET != nullptr)
    {
        EDGE* pDel = pAET;
        pAET = pAET->next;
        delete pDel;
    }
    while (pET != nullptr)
    {
        EDGE* pDel = pET;
        pET = pET->next;
        delete pDel;
    }
}

CShape* CShapeFill::Clone() const
{
    CShapeFill* pClone = new CShapeFill(m_bSeedFill);
    pClone->m_vecCtrlPts = m_vecCtrlPts;
    pClone->m_style = m_style;
    pClone->m_bFinished = m_bFinished;
    pClone->m_bSelected = m_bSelected;
    pClone->m_ptCurrent = m_ptCurrent;
    pClone->m_fillColor = m_fillColor;
    pClone->m_borderColor = m_borderColor;
    return pClone;
}