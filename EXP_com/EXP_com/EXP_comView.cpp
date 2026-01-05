#include "pch.h"
#include "framework.h"
#include "EXP_com.h"
#include "EXP_comDoc.h"
#include "EXP_comView.h"
#include "CurveBezier.h"
#include "CurveBSpline.h"
#include "ShapeLine.h"
#include "ShapeRect.h"
#include "ShapePolygon.h"
#include "ShapeEllipse.h"
#include "ShapeFill.h"
#include "InputDialog.h"
#include "TransformMatrix.h"
#include "Resource.h"
#include "ClipAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CEXPcomView, CView)

BEGIN_MESSAGE_MAP(CEXPcomView, CView)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()

    // 曲线
    ON_COMMAND(MID_BEZIER, &CEXPcomView::OnCurveBezier)
    ON_COMMAND(MID_BSAMPLE, &CEXPcomView::OnCurveBSpline)
    ON_UPDATE_COMMAND_UI(MID_BEZIER, &CEXPcomView::OnUpdateCurveBezier)
    ON_UPDATE_COMMAND_UI(MID_BSAMPLE, &CEXPcomView::OnUpdateCurveBSpline)

    // 线段
    ON_COMMAND(MID_DDAL, &CEXPcomView::OnLineDDA)
    ON_COMMAND(MID_MLINE, &CEXPcomView::OnLineMidpoint)
    ON_COMMAND(MID_BLINE, &CEXPcomView::OnLineBresenham)
    ON_UPDATE_COMMAND_UI(MID_DDAL, &CEXPcomView::OnUpdateLineDDA)
    ON_UPDATE_COMMAND_UI(MID_MLINE, &CEXPcomView::OnUpdateLineMidpoint)
    ON_UPDATE_COMMAND_UI(MID_BLINE, &CEXPcomView::OnUpdateLineBresenham)

    // 图形
    ON_COMMAND(MID_RECTANGLE, &CEXPcomView::OnShapeRectangle)
    ON_COMMAND(MID_POLYGON, &CEXPcomView::OnShapePolygon)
    ON_COMMAND(MID_ELLIPSE, &CEXPcomView::OnShapeEllipse)
    ON_COMMAND(MID_CIRCLE, &CEXPcomView::OnShapeCircle)
    ON_UPDATE_COMMAND_UI(MID_RECTANGLE, &CEXPcomView::OnUpdateShapeRectangle)
    ON_UPDATE_COMMAND_UI(MID_POLYGON, &CEXPcomView::OnUpdateShapePolygon)
    ON_UPDATE_COMMAND_UI(MID_ELLIPSE, &CEXPcomView::OnUpdateShapeEllipse)
    ON_UPDATE_COMMAND_UI(MID_CIRCLE, &CEXPcomView::OnUpdateShapeCircle)

    // 填充
    ON_COMMAND(MID_SEED, &CEXPcomView::OnFillSeed)
    ON_COMMAND(MID_SCAN, &CEXPcomView::OnFillScanLine)
    ON_COMMAND(MID_FILLCOLOR, &CEXPcomView::OnFillColor)
    ON_UPDATE_COMMAND_UI(MID_SEED, &CEXPcomView::OnUpdateFillSeed)
    ON_UPDATE_COMMAND_UI(MID_SCAN, &CEXPcomView::OnUpdateFillScanLine)

    // 画笔
    ON_COMMAND(MID_PCOLOR, &CEXPcomView::OnPenColor)
    ON_COMMAND(MID_PTHICKNESS, &CEXPcomView::OnPenThickness)

    // 模式
    ON_COMMAND(MID_MODE_bindable, &CEXPcomView::OnModeDrawing)
    ON_COMMAND(MID_MODE_SELECT, &CEXPcomView::OnModeSelect)
    ON_UPDATE_COMMAND_UI(MID_MODE_bindable, &CEXPcomView::OnUpdateModeDrawing)
    ON_UPDATE_COMMAND_UI(MID_MODE_SELECT, &CEXPcomView::OnUpdateModeSelect)

    // 变换
    ON_COMMAND(MID_TRANS_TRANSLATE, &CEXPcomView::OnTransTranslate)
    ON_COMMAND(MID_TRANS_ROTATE, &CEXPcomView::OnTransRotate)
    ON_COMMAND(MID_TRANS_SCALE, &CEXPcomView::OnTransScale)
    ON_COMMAND(MID_TRANS_SYMMETRY, &CEXPcomView::OnTransSymmetry)
    ON_COMMAND(MID_TRANS_SHEAR, &CEXPcomView::OnTransShear)
    ON_COMMAND(MID_TRANS_DELETE, &CEXPcomView::OnTransDelete)

    // 裁剪菜单
    ON_COMMAND(MID_CLIP_LINE, &CEXPcomView::OnClipLine)
    ON_COMMAND(MID_CLIP_POLYGON, &CEXPcomView::OnClipPolygon)
    ON_COMMAND(MID_CLIP_WINDOW, &CEXPcomView::OnClipWindow)
    ON_COMMAND(MID_CLIP_EXECUTE, &CEXPcomView::OnClipExecute)
    ON_COMMAND(MID_CLIP_CLEAR, &CEXPcomView::OnClipClear)
    ON_UPDATE_COMMAND_UI(MID_CLIP_LINE, &CEXPcomView::OnUpdateClipLine)
    ON_UPDATE_COMMAND_UI(MID_CLIP_POLYGON, &CEXPcomView::OnUpdateClipPolygon)
    ON_UPDATE_COMMAND_UI(MID_CLIP_EXECUTE, &CEXPcomView::OnUpdateClipExecute)
    ON_UPDATE_COMMAND_UI(MID_CLIP_CLEAR, &CEXPcomView::OnUpdateClipClear)

    // 编辑菜单
    ON_COMMAND(MID_UNDO, &CEXPcomView::OnEditUndo)
    ON_COMMAND(MID_REDO, &CEXPcomView::OnEditRedo)
    ON_COMMAND(MID_CLEAR_ALL, &CEXPcomView::OnEditClearAll)
    ON_UPDATE_COMMAND_UI(MID_UNDO, &CEXPcomView::OnUpdateEditUndo)
    ON_UPDATE_COMMAND_UI(MID_REDO, &CEXPcomView::OnUpdateEditRedo)
END_MESSAGE_MAP()

// ==================== 构造/析构 ====================

CEXPcomView::CEXPcomView() noexcept
    : m_eCurrentTool(EShapeType::None)
    , m_eMouseMode(EMouseMode::Drawing)
    , m_bSelecting(FALSE)
    , m_bHasClipWindow(FALSE)
    , m_bDrawingClipWindow(FALSE)
    , m_eClipType(EClipType::None)
{
}

CEXPcomView::~CEXPcomView()
{
}

BOOL CEXPcomView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// ==================== 防止闪烁 ====================

BOOL CEXPcomView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

// ==================== 双缓冲绘制 ====================

void CEXPcomView::OnDraw(CDC* pDC)
{
    CEXPcomDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc) return;

    CRect rcClient;
    GetClientRect(&rcClient);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    memDC.FillSolidRect(&rcClient, RGB(255, 255, 255));

    // 绘制所有图形
    for (const auto& pShape : pDoc->GetShapes())
    {
        pShape->Draw(&memDC);
        if (pShape->IsSelected())
        {
            pShape->DrawSelectionHandles(&memDC);
        }
    }

    // 绘制当前图形
    if (m_pCurrentShape && !m_pCurrentShape->IsFinished())
    {
        m_pCurrentShape->DrawPreview(&memDC);
    }

    // 绘制框选矩形
    if (m_bSelecting)
    {
        DrawSelectionRect(&memDC);
    }

    // 绘制裁剪窗口
    DrawClipWindow(&memDC);

    pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
    memBitmap.DeleteObject();
    memDC.DeleteDC();
}

void CEXPcomView::DrawSelectionRect(CDC* pDC)
{
    CPen pen(PS_DOT, 1, RGB(0, 0, 255));
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    CRect rect(m_ptSelectStart, m_ptSelectEnd);
    rect.NormalizeRect();
    pDC->Rectangle(&rect);

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

void CEXPcomView::DrawClipWindow(CDC* pDC)
{
    CRect rc;

    if (m_bDrawingClipWindow)
    {
        // 正在绘制裁剪窗口
        rc = CRect(m_ptClipStart, m_ptClipEnd);
        rc.NormalizeRect();
    }
    else if (m_bHasClipWindow)
    {
        // 已设置裁剪窗口
        rc = m_rcClipWindow;
    }
    else
    {
        return;
    }

    // 绘制裁剪窗口（红色虚线矩形）
    CPen pen(PS_DASH, 2, RGB(255, 0, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    pDC->Rectangle(&rc);

    // 绘制四角标记
    const int markSize = 6;
    CBrush brushRed(RGB(255, 0, 0));
    pDC->SelectObject(&brushRed);

    pDC->Rectangle(rc.left - markSize, rc.top - markSize,
        rc.left + markSize, rc.top + markSize);
    pDC->Rectangle(rc.right - markSize, rc.top - markSize,
        rc.right + markSize, rc.top + markSize);
    pDC->Rectangle(rc.left - markSize, rc.bottom - markSize,
        rc.left + markSize, rc.bottom + markSize);
    pDC->Rectangle(rc.right - markSize, rc.bottom - markSize,
        rc.right + markSize, rc.bottom + markSize);

    // 显示"裁剪窗口"文字
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(255, 0, 0));
    pDC->TextOut(rc.left, rc.top - 20, _T("裁剪窗口"));

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

// ==================== 工具创建 ====================

CShape* CEXPcomView::CreateShape(EShapeType type)
{
    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = nullptr;

    switch (type)
    {
        // 线段
    case EShapeType::LineDDA:
        pShape = new CShapeLine(ELineAlgorithm::DDA);
        break;
    case EShapeType::LineMidpoint:
        pShape = new CShapeLine(ELineAlgorithm::Midpoint);
        break;
    case EShapeType::LineBresenham:
        pShape = new CShapeLine(ELineAlgorithm::Bresenham);
        break;

        // 基本图形
    case EShapeType::Rectangle:
        pShape = new CShapeRect();
        break;
    case EShapeType::Polygon:
        pShape = new CShapePolygon();
        break;
    case EShapeType::Ellipse:
        pShape = new CShapeEllipse(false);
        break;
    case EShapeType::Circle:
        pShape = new CShapeEllipse(true);
        break;

        // 曲线
    case EShapeType::Bezier:
        pShape = new CCurveBezier();
        break;
    case EShapeType::BSpline:
        pShape = new CCurveBSpline();
        break;

        // 填充
    case EShapeType::FillSeed:
    {
        CShapeFill* pFill = new CShapeFill(true);
        pFill->SetFillColor(pDoc->GetCurrentStyle().GetFillColor());
        pFill->SetBorderColor(RGB(0, 0, 0));
        pShape = pFill;
    }
    break;
    case EShapeType::FillScanLine:
    {
        CShapeFill* pFill = new CShapeFill(false);
        pFill->SetFillColor(pDoc->GetCurrentStyle().GetFillColor());
        pShape = pFill;
    }
    break;

    default:
        break;
    }

    if (pShape)
    {
        pShape->SetStyle(pDoc->GetCurrentStyle());
    }

    return pShape;
}

// ==================== 鼠标事件 ====================

void CEXPcomView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_eMouseMode == EMouseMode::ClipWindow)
    {
        // 裁剪窗口绘制模式
        m_bDrawingClipWindow = TRUE;
        m_ptClipStart = point;
        m_ptClipEnd = point;
        SetCapture();
    }
    else if (m_eMouseMode == EMouseMode::Select)
    {
        m_bSelecting = TRUE;
        m_ptSelectStart = point;
        m_ptSelectEnd = point;
        SetCapture();
    }
    else
    {
        if (m_pCurrentShape && !m_pCurrentShape->IsFinished())
        {
            m_pCurrentShape->OnLButtonDown(point);
            Invalidate(FALSE);
        }
    }

    CView::OnLButtonDown(nFlags, point);
}

void CEXPcomView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDrawingClipWindow)
    {
        ReleaseCapture();
        m_ptClipEnd = point;

        m_rcClipWindow = CRect(m_ptClipStart, m_ptClipEnd);
        m_rcClipWindow.NormalizeRect();

        if (m_rcClipWindow.Width() > 10 && m_rcClipWindow.Height() > 10)
        {
            m_bHasClipWindow = TRUE;
        }

        m_bDrawingClipWindow = FALSE;
        m_eMouseMode = EMouseMode::Select;
        Invalidate(FALSE);
    }
    else if (m_bSelecting)
    {
        ReleaseCapture();
        m_ptSelectEnd = point;

        CEXPcomDoc* pDoc = GetDocument();
        CRect selectRect(m_ptSelectStart, m_ptSelectEnd);
        pDoc->SelectShapeByRect(selectRect);

        m_bSelecting = FALSE;
        Invalidate(FALSE);
    }

    CView::OnLButtonUp(nFlags, point);
}

void CEXPcomView::OnRButtonDown(UINT nFlags, CPoint point)
{
    if (m_eMouseMode == EMouseMode::Drawing)
    {
        if (m_pCurrentShape && !m_pCurrentShape->IsFinished())
        {
            if (m_pCurrentShape->OnRButtonDown(point))
            {
                CEXPcomDoc* pDoc = GetDocument();
                pDoc->AddShape(m_pCurrentShape.get());
                m_pCurrentShape.reset(CreateShape(m_eCurrentTool));
                Invalidate(FALSE);
            }
        }
    }

    CView::OnRButtonDown(nFlags, point);
}

void CEXPcomView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDrawingClipWindow && (nFlags & MK_LBUTTON))
    {
        m_ptClipEnd = point;
        Invalidate(FALSE);
    }
    else if (m_bSelecting && (nFlags & MK_LBUTTON))
    {
        m_ptSelectEnd = point;
        Invalidate(FALSE);
    }
    else if (m_pCurrentShape && !m_pCurrentShape->IsFinished())
    {
        m_pCurrentShape->OnMouseMove(point);
        Invalidate(FALSE);
    }

    CView::OnMouseMove(nFlags, point);
}

// ==================== 裁剪菜单实现 ====================

void CEXPcomView::OnClipLine()
{
    m_eClipType = EClipType::Line;
    AfxMessageBox(_T("已选择线段裁剪模式（Cohen-Sutherland算法）\n请先绘制裁剪窗口，然后选择要裁剪的线段，最后执行裁剪。"));
}

void CEXPcomView::OnClipPolygon()
{
    m_eClipType = EClipType::Polygon;
    AfxMessageBox(_T("已选择多边形裁剪模式（Sutherland-Hodgman算法）\n请先绘制裁剪窗口，然后选择要裁剪的多边形，最后执行裁剪。"));
}

void CEXPcomView::OnClipWindow()
{
    m_eMouseMode = EMouseMode::ClipWindow;
    m_pCurrentShape.reset();
    m_eCurrentTool = EShapeType::None;
    GetDocument()->ClearSelection();

    AfxMessageBox(_T("请拖动鼠标绘制裁剪窗口矩形。"));
}

void CEXPcomView::OnClipExecute()
{
    if (!m_bHasClipWindow)
    {
        AfxMessageBox(_T("请先设置裁剪窗口！"));
        return;
    }

    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = pDoc->GetSelectedShape();

    if (!pShape)
    {
        AfxMessageBox(_T("请先选择要裁剪的图形！"));
        return;
    }

    // 获取裁剪窗口边界
    double xmin = m_rcClipWindow.left;
    double ymin = m_rcClipWindow.top;
    double xmax = m_rcClipWindow.right;
    double ymax = m_rcClipWindow.bottom;

    EShapeType shapeType = pShape->GetType();

    // 线段裁剪
    if (shapeType == EShapeType::LineDDA ||
        shapeType == EShapeType::LineMidpoint ||
        shapeType == EShapeType::LineBresenham)
    {
        if (pShape->GetControlPointCount() < 2)
        {
            AfxMessageBox(_T("线段数据无效！"));
            return;
        }

        CPoint p1 = pShape->GetControlPoint(0);
        CPoint p2 = pShape->GetControlPoint(1);

        double nx1, ny1, nx2, ny2;
        bool visible = CClipAlgorithm::CohenSutherlandClip(
            p1.x, p1.y, p2.x, p2.y,
            xmin, ymin, xmax, ymax,
            nx1, ny1, nx2, ny2);

        if (visible)
        {
            // 更新线段端点
            pShape->SetControlPoint(0, CPoint((int)nx1, (int)ny1));
            pShape->SetControlPoint(1, CPoint((int)nx2, (int)ny2));
            AfxMessageBox(_T("线段裁剪完成！"));
        }
        else
        {
            // 线段完全在窗口外，删除
            pDoc->DeleteSelected();
            AfxMessageBox(_T("线段完全在裁剪窗口外，已删除！"));
        }
    }
    // 多边形裁剪
    else if (shapeType == EShapeType::Polygon ||
        shapeType == EShapeType::Rectangle)
    {
        std::vector<CPoint> polygon = pShape->GetControlPoints();

        if (polygon.size() < 3)
        {
            AfxMessageBox(_T("多边形数据无效！"));
            return;
        }

        std::vector<CPoint> clipped = CClipAlgorithm::SutherlandHodgmanClip(
            polygon, m_rcClipWindow);

        if (clipped.size() >= 3)
        {
            // 清除原有控制点，设置新的
            pShape->ClearControlPoints();
            for (const auto& pt : clipped)
            {
                pShape->AddControlPoint(pt);
            }
            pShape->SetFinished(TRUE);
            AfxMessageBox(_T("多边形裁剪完成！"));
        }
        else
        {
            pDoc->DeleteSelected();
            AfxMessageBox(_T("多边形完全在裁剪窗口外，已删除！"));
        }
    }
    else
    {
        AfxMessageBox(_T("当前图形类型不支持裁剪！\n仅支持线段和多边形。"));
        return;
    }

    Invalidate(FALSE);
}

void CEXPcomView::OnClipClear()
{
    m_bHasClipWindow = FALSE;
    m_rcClipWindow.SetRectEmpty();
    m_eClipType = EClipType::None;
    Invalidate(FALSE);
}

// ==================== 模式切换菜单 ====================

void CEXPcomView::OnModeDrawing()
{
    m_eMouseMode = EMouseMode::Drawing;
    CEXPcomDoc* pDoc = GetDocument();
    pDoc->ClearSelection();
    Invalidate(FALSE);
}

void CEXPcomView::OnModeSelect()
{
    m_eMouseMode = EMouseMode::Select;
    m_pCurrentShape.reset();
    m_eCurrentTool = EShapeType::None;
    Invalidate(FALSE);
}

void CEXPcomView::OnUpdateModeDrawing(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eMouseMode == EMouseMode::Drawing);
}

void CEXPcomView::OnUpdateModeSelect(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eMouseMode == EMouseMode::Select);
}

// ==================== 变换菜单 ====================

void CEXPcomView::OnTransTranslate()
{
    CEXPcomDoc* pDoc = GetDocument();
    if (!pDoc->GetSelectedShape())
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    double tx = 0, ty = 0;
    if (!ShowInputDlg(this, _T("请输入X方向平移量："), tx)) return;
    if (!ShowInputDlg(this, _T("请输入Y方向平移量："), ty)) return;

    CTransformMatrix matrix;
    matrix.SetTranslate(tx, ty);
    pDoc->TransformSelected(matrix.m);
    Invalidate(FALSE);
}

void CEXPcomView::OnTransRotate()
{
    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = pDoc->GetSelectedShape();
    if (!pShape)
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    double angle = 0;
    if (!ShowInputDlg(this, _T("请输入旋转角度（度）："), angle)) return;

    CPoint center = pShape->GetCenter();

    CTransformMatrix matrix;
    matrix.SetRotate(angle, center.x, center.y);
    pDoc->TransformSelected(matrix.m);
    Invalidate(FALSE);
}

void CEXPcomView::OnTransScale()
{
    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = pDoc->GetSelectedShape();
    if (!pShape)
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    double sx = 1, sy = 1;
    if (!ShowInputDlg(this, _T("请输入X方向缩放比例："), sx)) return;
    if (!ShowInputDlg(this, _T("请输入Y方向缩放比例："), sy)) return;

    if (sx == 0) sx = 1;
    if (sy == 0) sy = 1;

    CPoint center = pShape->GetCenter();

    CTransformMatrix matrix;
    matrix.SetScale(sx, sy, center.x, center.y);
    pDoc->TransformSelected(matrix.m);
    Invalidate(FALSE);
}

void CEXPcomView::OnTransSymmetry()
{
    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = pDoc->GetSelectedShape();
    if (!pShape)
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    int choice = ShowSymmetryDlg(this);
    if (choice < 0) return;

    CPoint center = pShape->GetCenter();

    CTransformMatrix matrix;
    switch (choice)
    {
    case 0: matrix.SetSymmetricX(center.y); break;
    case 1: matrix.SetSymmetricY(center.x); break;
    case 2: matrix.SetSymmetricOrigin(center.x, center.y); break;
    default: return;
    }

    pDoc->TransformSelected(matrix.m);
    Invalidate(FALSE);
}

void CEXPcomView::OnTransShear()
{
    CEXPcomDoc* pDoc = GetDocument();
    CShape* pShape = pDoc->GetSelectedShape();
    if (!pShape)
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    double shx = 0, shy = 0;
    if (!ShowInputDlg(this, _T("请输入X方向错切系数："), shx)) return;
    if (!ShowInputDlg(this, _T("请输入Y方向错切系数："), shy)) return;

    CPoint center = pShape->GetCenter();

    CTransformMatrix matrix;
    matrix.SetShear(shx, shy, center.x, center.y);
    pDoc->TransformSelected(matrix.m);
    Invalidate(FALSE);
}

void CEXPcomView::OnTransDelete()
{
    CEXPcomDoc* pDoc = GetDocument();
    if (!pDoc->GetSelectedShape())
    {
        AfxMessageBox(_T("请先框选一个图形！"));
        return;
    }

    if (AfxMessageBox(_T("确定要删除选中的图形吗？"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        pDoc->DeleteSelected();
        Invalidate(FALSE);
    }
}

// ==================== 曲线菜单 ====================

void CEXPcomView::OnCurveBezier()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::Bezier;
    m_pCurrentShape.reset(CreateShape(EShapeType::Bezier));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnCurveBSpline()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::BSpline;
    m_pCurrentShape.reset(CreateShape(EShapeType::BSpline));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnUpdateCurveBezier(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::Bezier);
}

void CEXPcomView::OnUpdateCurveBSpline(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::BSpline);
}

// ==================== 线段菜单 ====================

void CEXPcomView::OnLineDDA()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::LineDDA;
    m_pCurrentShape.reset(CreateShape(EShapeType::LineDDA));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnLineMidpoint()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::LineMidpoint;
    m_pCurrentShape.reset(CreateShape(EShapeType::LineMidpoint));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnLineBresenham()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::LineBresenham;
    m_pCurrentShape.reset(CreateShape(EShapeType::LineBresenham));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnUpdateLineDDA(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::LineDDA);
}

void CEXPcomView::OnUpdateLineMidpoint(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::LineMidpoint);
}

void CEXPcomView::OnUpdateLineBresenham(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::LineBresenham);
}

// ==================== 图形菜单 ====================

void CEXPcomView::OnShapeRectangle()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::Rectangle;
    m_pCurrentShape.reset(CreateShape(EShapeType::Rectangle));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnShapePolygon()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::Polygon;
    m_pCurrentShape.reset(CreateShape(EShapeType::Polygon));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnShapeEllipse()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::Ellipse;
    m_pCurrentShape.reset(CreateShape(EShapeType::Ellipse));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnShapeCircle()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::Circle;
    m_pCurrentShape.reset(CreateShape(EShapeType::Circle));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnUpdateShapeRectangle(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::Rectangle);
}

void CEXPcomView::OnUpdateShapePolygon(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::Polygon);
}

void CEXPcomView::OnUpdateShapeEllipse(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::Ellipse);
}

void CEXPcomView::OnUpdateShapeCircle(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::Circle);
}

// ==================== 填充菜单 ====================

void CEXPcomView::OnFillSeed()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::FillSeed;
    m_pCurrentShape.reset(CreateShape(EShapeType::FillSeed));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnFillScanLine()
{
    m_eMouseMode = EMouseMode::Drawing;
    m_eCurrentTool = EShapeType::FillScanLine;
    m_pCurrentShape.reset(CreateShape(EShapeType::FillScanLine));
    GetDocument()->ClearSelection();
}

void CEXPcomView::OnFillColor()
{
    CEXPcomDoc* pDoc = GetDocument();

    if (pDoc->GetCurrentStyle().ChooseFillColor(this))
    {
        if (m_pCurrentShape)
        {
            CShapeFill* pFill = dynamic_cast<CShapeFill*>(m_pCurrentShape.get());
            if (pFill)
            {
                pFill->SetFillColor(pDoc->GetCurrentStyle().GetFillColor());
            }
            Invalidate(FALSE);
        }
    }
}

void CEXPcomView::OnUpdateFillSeed(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::FillSeed);
}

void CEXPcomView::OnUpdateFillScanLine(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eCurrentTool == EShapeType::FillScanLine);
}

// ==================== 画笔菜单 ====================

void CEXPcomView::OnPenColor()
{
    CEXPcomDoc* pDoc = GetDocument();

    if (pDoc->GetCurrentStyle().ChooseLineColor(this))
    {
        if (m_pCurrentShape)
        {
            m_pCurrentShape->SetStyle(pDoc->GetCurrentStyle());
            Invalidate(FALSE);
        }
    }
}

void CEXPcomView::OnPenThickness()
{
    CEXPcomDoc* pDoc = GetDocument();
    int nCurrentWidth = pDoc->GetCurrentStyle().GetLineWidth();

    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 1 ? MF_CHECKED : 0), 1, _T("1 像素"));
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 2 ? MF_CHECKED : 0), 2, _T("2 像素"));
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 3 ? MF_CHECKED : 0), 3, _T("3 像素"));
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 5 ? MF_CHECKED : 0), 5, _T("5 像素"));
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 8 ? MF_CHECKED : 0), 8, _T("8 像素"));
    menu.AppendMenu(MF_STRING | (nCurrentWidth == 10 ? MF_CHECKED : 0), 10, _T("10 像素"));

    CPoint pt;
    GetCursorPos(&pt);

    int nSelected = menu.TrackPopupMenu(
        TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
        pt.x, pt.y, this);

    if (nSelected > 0)
    {
        pDoc->GetCurrentStyle().SetLineWidth(nSelected);

        if (m_pCurrentShape)
        {
            m_pCurrentShape->SetStyle(pDoc->GetCurrentStyle());
            Invalidate(FALSE);
        }
    }
}

// ==================== 裁剪菜单 ====================

void CEXPcomView::OnUpdateClipLine(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eClipType == EClipType::Line);
}

void CEXPcomView::OnUpdateClipPolygon(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_eClipType == EClipType::Polygon);
}

void CEXPcomView::OnUpdateClipExecute(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bHasClipWindow && GetDocument()->GetSelectedShape() != nullptr);
}

void CEXPcomView::OnUpdateClipClear(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bHasClipWindow);
}

// ==================== 编辑菜单 ====================

void CEXPcomView::OnEditUndo()
{
    GetDocument()->Undo();
    Invalidate(FALSE);
}

void CEXPcomView::OnEditRedo()
{
    GetDocument()->Redo();
    Invalidate(FALSE);
}

void CEXPcomView::OnEditClearAll()
{
    if (AfxMessageBox(_T("确定要清空所有图形吗？"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        GetDocument()->ClearAllShapes();
        m_pCurrentShape.reset();
        m_eCurrentTool = EShapeType::None;
        Invalidate(FALSE);
    }
}

void CEXPcomView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->CanUndo());
}

void CEXPcomView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetDocument()->CanRedo());
}

// ==================== 调试 ====================

#ifdef _DEBUG
void CEXPcomView::AssertValid() const
{
    CView::AssertValid();
}

void CEXPcomView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CEXPcomDoc* CEXPcomView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEXPcomDoc)));
    return (CEXPcomDoc*)m_pDocument;
}
#endif