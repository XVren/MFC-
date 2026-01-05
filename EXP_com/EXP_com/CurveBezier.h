#pragma once
#include "Shape.h"

// 贝塞尔曲线类
class CCurveBezier : public CShape
{
public:
    CCurveBezier();
    virtual ~CCurveBezier();

    // 重写基类方法
    virtual EShapeType GetType() const override { return EShapeType::Bezier; }
    virtual void Draw(CDC* pDC) override;
    virtual CShape* Clone() const override;

private:
    // 计算贝塞尔曲线上的点
    CPoint CalculatePoint(double t) const;

    // 计算组合数 C(n, k)
    static double Combination(int n, int k);

    // 计算伯恩斯坦基函数 B_{i,n}(t)
    static double Bernstein(int i, int n, double t);
};