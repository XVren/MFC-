#pragma once
#include <memory>
#include "Shape.h"

// 鼠标模式
enum class EMouseMode
{
    Drawing,        // 绑定（绘图）模式
    Select,         // 选择模式
    ClipWindow      // 裁剪窗口绘制模式
};

// 裁剪类型
enum class EClipType
{
    None,
    Line,       // 线段裁剪
    Polygon     // 多边形裁剪
};

class CEXPcomView : public CView
{
protected:
    CEXPcomView() noexcept;
    DECLARE_DYNCREATE(CEXPcomView)

public:
    CEXPcomDoc* GetDocument() const;

public:
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

public:
    virtual ~CEXPcomView();

protected:
    std::unique_ptr<CShape> m_pCurrentShape;
    EShapeType m_eCurrentTool;
    EMouseMode m_eMouseMode;

    BOOL m_bSelecting;
    CPoint m_ptSelectStart;
    CPoint m_ptSelectEnd;

    // ===== 裁剪相关 =====
    CRect m_rcClipWindow;       // 裁剪窗口
    BOOL m_bHasClipWindow;      // 是否已设置裁剪窗口
    BOOL m_bDrawingClipWindow;  // 是否正在绘制裁剪窗口
    CPoint m_ptClipStart;       // 裁剪窗口起点
    CPoint m_ptClipEnd;         // 裁剪窗口终点
    EClipType m_eClipType;      // 当前裁剪类型

    CShape* CreateShape(EShapeType type);
    void DrawSelectionRect(CDC* pDC);
    void DrawClipWindow(CDC* pDC);  // 绘制裁剪窗口

protected:
    DECLARE_MESSAGE_MAP()

public:
    // 鼠标消息
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    // 曲线菜单
    afx_msg void OnCurveBezier();
    afx_msg void OnCurveBSpline();
    afx_msg void OnUpdateCurveBezier(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCurveBSpline(CCmdUI* pCmdUI);

    // 线段菜单
    afx_msg void OnLineDDA();
    afx_msg void OnLineMidpoint();
    afx_msg void OnLineBresenham();
    afx_msg void OnUpdateLineDDA(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLineMidpoint(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLineBresenham(CCmdUI* pCmdUI);

    // 图形菜单
    afx_msg void OnShapeRectangle();
    afx_msg void OnShapePolygon();
    afx_msg void OnShapeEllipse();
    afx_msg void OnShapeCircle();
    afx_msg void OnUpdateShapeRectangle(CCmdUI* pCmdUI);
    afx_msg void OnUpdateShapePolygon(CCmdUI* pCmdUI);
    afx_msg void OnUpdateShapeEllipse(CCmdUI* pCmdUI);
    afx_msg void OnUpdateShapeCircle(CCmdUI* pCmdUI);

    // 填充菜单
    afx_msg void OnFillSeed();
    afx_msg void OnFillScanLine();
    afx_msg void OnFillColor();
    afx_msg void OnUpdateFillSeed(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFillScanLine(CCmdUI* pCmdUI);

    // 画笔菜单
    afx_msg void OnPenColor();
    afx_msg void OnPenThickness();

    // 模式菜单
    afx_msg void OnModeDrawing();
    afx_msg void OnModeSelect();
    afx_msg void OnUpdateModeDrawing(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeSelect(CCmdUI* pCmdUI);

    // 变换菜单
    afx_msg void OnTransTranslate();
    afx_msg void OnTransRotate();
    afx_msg void OnTransScale();
    afx_msg void OnTransSymmetry();
    afx_msg void OnTransShear();
    afx_msg void OnTransDelete();

    // ===== 裁剪菜单 =====
    afx_msg void OnClipLine();
    afx_msg void OnClipPolygon();
    afx_msg void OnClipWindow();
    afx_msg void OnClipExecute();
    afx_msg void OnClipClear();
    afx_msg void OnUpdateClipLine(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipPolygon(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipExecute(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClipClear(CCmdUI* pCmdUI);

    // ===== 编辑菜单 =====
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditClearAll();
    afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
};

#ifndef _DEBUG
inline CEXPcomDoc* CEXPcomView::GetDocument() const
{
    return reinterpret_cast<CEXPcomDoc*>(m_pDocument);
}
#endif