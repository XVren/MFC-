#pragma once
#include "Shape.h"

// ∂‡±ﬂ–Œ¿‡
class CShapePolygon : public CShape
{
public:
    CShapePolygon();
    virtual ~CShapePolygon();

    virtual EShapeType GetType() const override { return EShapeType::Polygon; }
    virtual void Draw(CDC* pDC) override;
    virtual void DrawPreview(CDC* pDC) override;
    virtual CShape* Clone() const override;
    virtual BOOL OnRButtonDown(CPoint point) override;
};