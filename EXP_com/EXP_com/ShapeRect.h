#pragma once
#include "Shape.h"

// æÿ–Œ¿‡
class CShapeRect : public CShape
{
public:
    CShapeRect();
    virtual ~CShapeRect();

    virtual EShapeType GetType() const override { return EShapeType::Rectangle; }
    virtual void Draw(CDC* pDC) override;
    virtual CShape* Clone() const override;
    virtual BOOL OnRButtonDown(CPoint point) override;
};