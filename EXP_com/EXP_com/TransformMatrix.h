#pragma once
#include <afxwin.h>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// 3x3Æë´Î×ø±ê±ä»»¾ØÕó
class CTransformMatrix
{
public:
    double m[3][3];

    CTransformMatrix()
    {
        SetIdentity();
    }

    void SetIdentity()
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }

    void SetTranslate(double tx, double ty)
    {
        SetIdentity();
        m[0][2] = tx;
        m[1][2] = ty;
    }

    void SetScale(double sx, double sy, double cx = 0, double cy = 0)
    {
        SetIdentity();
        m[0][0] = sx;
        m[1][1] = sy;
        m[0][2] = cx * (1 - sx);
        m[1][2] = cy * (1 - sy);
    }

    void SetRotate(double angle, double cx = 0, double cy = 0)
    {
        SetIdentity();
        double rad = angle * PI / 180.0;
        double cosA = cos(rad);
        double sinA = sin(rad);

        m[0][0] = cosA;
        m[0][1] = -sinA;
        m[1][0] = sinA;
        m[1][1] = cosA;
        m[0][2] = cx * (1 - cosA) + cy * sinA;
        m[1][2] = cy * (1 - cosA) - cx * sinA;
    }

    void SetSymmetricX(double cy)
    {
        SetIdentity();
        m[1][1] = -1;
        m[1][2] = 2 * cy;
    }

    void SetSymmetricY(double cx)
    {
        SetIdentity();
        m[0][0] = -1;
        m[0][2] = 2 * cx;
    }

    void SetSymmetricOrigin(double cx, double cy)
    {
        SetIdentity();
        m[0][0] = -1;
        m[1][1] = -1;
        m[0][2] = 2 * cx;
        m[1][2] = 2 * cy;
    }

    void SetShear(double shx, double shy, double cx = 0, double cy = 0)
    {
        SetIdentity();
        m[0][1] = shx;
        m[1][0] = shy;
        m[0][2] = -shx * cy;
        m[1][2] = -shy * cx;
    }

    CPoint Transform(const CPoint& pt) const
    {
        double x = m[0][0] * pt.x + m[0][1] * pt.y + m[0][2];
        double y = m[1][0] * pt.x + m[1][1] * pt.y + m[1][2];
        return CPoint((int)(x + 0.5), (int)(y + 0.5));
    }
};