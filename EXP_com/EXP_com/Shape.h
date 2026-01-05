#pragma once
#include <afxwin.h>
#include <vector>
#include "ShapeStyle.h"

// 前向声明（避免循环包含）
class CTransformMatrix;

// 图形类型枚举
enum class EShapeType
{
    None = 0,
    // 线段
    LineDDA,
    LineMidpoint,
    LineBresenham,
    // 基本图形
    Rectangle,
    Polygon,
    Ellipse,
    Circle,
    // 曲线
    Bezier,
    BSpline,
    // 填充模式
    FillSeed,
    FillScanLine,
};

// 图形基类
class CShape
{
public:
    CShape();
    virtual ~CShape();

    // === 纯虚函数 ===
    virtual EShapeType GetType() const = 0;
    virtual void Draw(CDC* pDC) = 0;
    virtual CShape* Clone() const = 0;

    // === 交互 ===
    virtual void OnLButtonDown(CPoint point);
    virtual void OnMouseMove(CPoint point);
    virtual BOOL OnRButtonDown(CPoint point);
    virtual void DrawPreview(CDC* pDC);

    // === 选择与变换 ===
    virtual bool HitTest(CPoint point, int tolerance = 5);
    virtual bool HitTestRect(CRect rect);
    virtual CRect GetBoundingBox();
    virtual void Transform(double matrix[3][3]);  // 使用数组而非类
    virtual CPoint GetCenter();

    // === 控制点管理 ===
    void AddControlPoint(CPoint pt);
    void ClearControlPoints();
    void SetControlPoint(int index, CPoint pt);
    CPoint GetControlPoint(int index) const;
    int GetControlPointCount() const { return (int)m_vecCtrlPts.size(); }
    const std::vector<CPoint>& GetControlPoints() const { return m_vecCtrlPts; }

    // === 状态 ===
    BOOL IsFinished() const { return m_bFinished; }
    void SetFinished(BOOL bFinished) { m_bFinished = bFinished; }

    BOOL IsSelected() const { return m_bSelected; }
    void SetSelected(BOOL bSelected) { m_bSelected = bSelected; }

    // === 样式 ===
    void SetStyle(const CShapeStyle& style) { m_style = style; }
    CShapeStyle& GetStyle() { return m_style; }
    const CShapeStyle& GetStyle() const { return m_style; }

    // === 绘制选择状态（改为public） ===
    void DrawSelectionHandles(CDC* pDC);

protected:
    void DrawControlPoints(CDC* pDC);

protected:
    std::vector<CPoint> m_vecCtrlPts;
    CPoint m_ptCurrent;
    CShapeStyle m_style;
    BOOL m_bFinished;
    BOOL m_bSelected;
};