#pragma once
#include "Shape.h"
#include <stack>

// 边表结构（用于扫描线填充）
struct EDGE
{
    int ymax;           // 边的最大y坐标
    int ymin;           // 边的最小y坐标
    double xmin;        // 与ymin对应的x坐标
    double fm;          // 斜率倒数 dx/dy
    EDGE* next;
    EDGE* prev;

    EDGE() : ymax(0), ymin(0), xmin(0), fm(0), next(nullptr), prev(nullptr) {}
};

// 填充类
class CShapeFill : public CShape
{
public:
    CShapeFill(bool bSeedFill = true);
    virtual ~CShapeFill();

    virtual EShapeType GetType() const override;
    virtual void Draw(CDC* pDC) override;
    virtual void DrawPreview(CDC* pDC) override;
    virtual CShape* Clone() const override;
    virtual BOOL OnRButtonDown(CPoint point) override;

    // 设置填充颜色
    void SetFillColor(COLORREF color) { m_fillColor = color; }
    COLORREF GetFillColor() const { return m_fillColor; }

    // 设置边界颜色（种子填充用）
    void SetBorderColor(COLORREF color) { m_borderColor = color; }

    // 执行填充
    void DoFill(CDC* pDC);

private:
    // 种子扫描线填充算法
    void SeedScanLineFill(CDC* pDC, int x, int y, COLORREF borderColor, COLORREF fillColor);

    // 多边形扫描线填充算法
    void PolygonScanLineFill(CDC* pDC, COLORREF fillColor);

    // 边表相关
    EDGE* BuildEdgeTable();
    void SortEdgeTable(EDGE*& pHead);
    void SortAET(EDGE*& pAET);
    void InsertEdge(EDGE*& pHead, EDGE* pNew);

private:
    bool m_bSeedFill;           // true=种子填充, false=扫描线填充
    COLORREF m_fillColor;       // 填充颜色
    COLORREF m_borderColor;     // 边界颜色（种子填充用）
};