#pragma once
#include "Shape.h"

// 线段绘制算法类型
enum class ELineAlgorithm
{
    DDA,
    Midpoint,
    Bresenham
};

// 线段类
class CShapeLine : public CShape
{
public:
    CShapeLine(ELineAlgorithm algorithm = ELineAlgorithm::DDA);
    virtual ~CShapeLine();

    // 重写基类方法
    virtual EShapeType GetType() const override;
    virtual void Draw(CDC* pDC) override;
    virtual CShape* Clone() const override;
    virtual BOOL OnRButtonDown(CPoint point) override;

    // 设置算法类型
    void SetAlgorithm(ELineAlgorithm alg) { m_algorithm = alg; }
    ELineAlgorithm GetAlgorithm() const { return m_algorithm; }

private:
    // 三种画线算法
    void DrawDDA(CDC* pDC, CPoint p1, CPoint p2);
    void DrawMidpoint(CDC* pDC, CPoint p1, CPoint p2);
    void DrawBresenham(CDC* pDC, CPoint p1, CPoint p2);

    // 绘制单个像素（考虑线宽）
    void DrawPixel(CDC* pDC, int x, int y);

private:
    ELineAlgorithm m_algorithm;
};