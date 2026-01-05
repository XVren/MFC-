#pragma once
#include <afxwin.h>

class CShapeStyle
{
public:
    CShapeStyle();
    CShapeStyle(const CShapeStyle& other);
    CShapeStyle& operator=(const CShapeStyle& other);
    ~CShapeStyle();

    // 线条属性
    void SetLineColor(COLORREF color) { m_colorLine = color; }
    void SetLineWidth(int width) { m_nLineWidth = width; }
    void SetLineStyle(int style) { m_nLineStyle = style; }

    COLORREF GetLineColor() const { return m_colorLine; }
    int GetLineWidth() const { return m_nLineWidth; }
    int GetLineStyle() const { return m_nLineStyle; }

    // 填充属性
    void SetFillColor(COLORREF color) { m_colorFill = color; }
    void SetFilled(bool bFill) { m_bFilled = bFill; }

    COLORREF GetFillColor() const { return m_colorFill; }
    bool IsFilled() const { return m_bFilled; }

    // 创建GDI对象
    CPen* CreatePen() const;
    CBrush* CreateBrush() const;

    // 颜色选择对话框
    BOOL ChooseLineColor(CWnd* pParent);
    BOOL ChooseFillColor(CWnd* pParent);

private:
    COLORREF m_colorLine;   // 线条颜色
    int m_nLineWidth;       // 线条宽度
    int m_nLineStyle;       // 线条样式

    COLORREF m_colorFill;   // 填充颜色
    bool m_bFilled;         // 是否填充
};