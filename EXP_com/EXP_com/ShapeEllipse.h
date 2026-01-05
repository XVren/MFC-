#pragma once
#include "Shape.h"

// 椭圆/圆类
class CShapeEllipse : public CShape
{
public:
    CShapeEllipse(bool bCircle = false);
    virtual ~CShapeEllipse();

    virtual EShapeType GetType() const override;
    virtual void Draw(CDC* pDC) override;
    virtual CShape* Clone() const override;
    virtual BOOL OnRButtonDown(CPoint point) override;

    bool IsCircle() const { return m_bCircle; }

private:
    // 中点椭圆算法
    void DrawEllipseMidpoint(CDC* pDC, int cx, int cy, int rx, int ry);
    // 中点圆算法
    void DrawCircleMidpoint(CDC* pDC, int cx, int cy, int r);
    // 绘制椭圆的4个对称点
    void DrawEllipsePoints(CDC* pDC, int cx, int cy, int x, int y);
    // 绘制圆的8个对称点
    void DrawCirclePoints(CDC* pDC, int cx, int cy, int x, int y);
    // 绘制单个像素
    void DrawPixel(CDC* pDC, int x, int y);

private:
    bool m_bCircle;  // true = 圆, false = 椭圆
};