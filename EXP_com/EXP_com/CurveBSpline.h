#pragma once
#include "Shape.h"

// B样条曲线类
class CCurveBSpline : public CShape
{
public:
    CCurveBSpline(int degree = 3);  // 默认3次B样条
    virtual ~CCurveBSpline();

    // 重写基类方法
    virtual EShapeType GetType() const override { return EShapeType::BSpline; }
    virtual void Draw(CDC* pDC) override;
    virtual BOOL OnRButtonDown(CPoint point) override;
    virtual CShape* Clone() const override;

    // B样条阶数
    void SetDegree(int degree) { m_nDegree = degree; }
    int GetDegree() const { return m_nDegree; }

private:
    // de Boor 算法计算曲线上的点
    CPoint DeBoor(double t, const std::vector<CPoint>& ctrlPts, const std::vector<double>& knots) const;

private:
    int m_nDegree;                  // B样条次数（degree）
    std::vector<double> m_vecKnots; // 节点向量
};