#include "pch.h"
#include "ClipAlgorithm.h"
#include <algorithm>

// ==================== Cohen-Sutherland 线段裁剪 ====================

int CClipAlgorithm::ComputeOutCode(double x, double y,
    double xmin, double ymin, double xmax, double ymax)
{
    int code = INSIDE;

    if (x < xmin)
        code |= LEFT;
    else if (x > xmax)
        code |= RIGHT;

    if (y < ymin)
        code |= TOP;      // 注意：屏幕坐标y向下增大
    else if (y > ymax)
        code |= BOTTOM;

    return code;
}

bool CClipAlgorithm::CohenSutherlandClip(
    double x1, double y1, double x2, double y2,
    double xmin, double ymin, double xmax, double ymax,
    double& nx1, double& ny1, double& nx2, double& ny2)
{
    int outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    int outcode2 = ComputeOutCode(x2, y2, xmin, ymin, xmax, ymax);

    bool accept = false;

    while (true)
    {
        if (!(outcode1 | outcode2))
        {
            // 两点都在窗口内，完全接受
            accept = true;
            break;
        }
        else if (outcode1 & outcode2)
        {
            // 两点在窗口同一侧外，完全拒绝
            break;
        }
        else
        {
            // 需要裁剪
            double x, y;

            // 选择在窗口外的点
            int outcodeOut = outcode1 ? outcode1 : outcode2;

            // 计算交点
            if (outcodeOut & TOP)
            {
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            }
            else if (outcodeOut & BOTTOM)
            {
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            }
            else if (outcodeOut & RIGHT)
            {
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            }
            else if (outcodeOut & LEFT)
            {
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            // 更新端点
            if (outcodeOut == outcode1)
            {
                x1 = x;
                y1 = y;
                outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            }
            else
            {
                x2 = x;
                y2 = y;
                outcode2 = ComputeOutCode(x2, y2, xmin, ymin, xmax, ymax);
            }
        }
    }

    if (accept)
    {
        nx1 = x1;
        ny1 = y1;
        nx2 = x2;
        ny2 = y2;
        return true;
    }

    return false;
}

// ==================== Liang-Barsky 线段裁剪 ====================

bool CClipAlgorithm::LiangBarskyClip(
    double x1, double y1, double x2, double y2,
    double xmin, double ymin, double xmax, double ymax,
    double& nx1, double& ny1, double& nx2, double& ny2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    double p[4], q[4];
    p[0] = -dx;  q[0] = x1 - xmin;  // 左边界
    p[1] = dx;   q[1] = xmax - x1;  // 右边界
    p[2] = -dy;  q[2] = y1 - ymin;  // 上边界
    p[3] = dy;   q[3] = ymax - y1;  // 下边界

    double u1 = 0.0, u2 = 1.0;

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0)
        {
            // 线段平行于边界
            if (q[i] < 0)
                return false;  // 线段在边界外
        }
        else
        {
            double u = q[i] / p[i];
            if (p[i] < 0)
            {
                // 从外到内
                if (u > u1) u1 = u;
            }
            else
            {
                // 从内到外
                if (u < u2) u2 = u;
            }
        }
    }

    if (u1 > u2)
        return false;  // 线段完全在窗口外

    nx1 = x1 + u1 * dx;
    ny1 = y1 + u1 * dy;
    nx2 = x1 + u2 * dx;
    ny2 = y1 + u2 * dy;

    return true;
}

// ==================== Sutherland-Hodgman 多边形裁剪 ====================

bool CClipAlgorithm::IsInside(const CPoint& p, const CPoint& edgeStart, const CPoint& edgeEnd)
{
    // 使用叉积判断点是否在边的内侧（左侧）
    // (edgeEnd - edgeStart) × (p - edgeStart) >= 0
    return ((edgeEnd.x - edgeStart.x) * (p.y - edgeStart.y) -
        (edgeEnd.y - edgeStart.y) * (p.x - edgeStart.x)) >= 0;
}

CPoint CClipAlgorithm::ComputeIntersection(const CPoint& p1, const CPoint& p2,
    const CPoint& edgeStart, const CPoint& edgeEnd)
{
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = edgeStart.x, y3 = edgeStart.y;
    double x4 = edgeEnd.x, y4 = edgeEnd.y;

    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (fabs(denom) < 1e-10)
    {
        // 平行，返回p1
        return p1;
    }

    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;

    double x = x1 + t * (x2 - x1);
    double y = y1 + t * (y2 - y1);

    return CPoint((int)(x + 0.5), (int)(y + 0.5));
}

std::vector<CPoint> CClipAlgorithm::ClipPolygonByEdge(
    const std::vector<CPoint>& polygon,
    const CPoint& edgeStart, const CPoint& edgeEnd)
{
    std::vector<CPoint> output;

    if (polygon.empty())
        return output;

    CPoint prev = polygon.back();

    for (size_t i = 0; i < polygon.size(); i++)
    {
        CPoint curr = polygon[i];

        bool prevInside = IsInside(prev, edgeStart, edgeEnd);
        bool currInside = IsInside(curr, edgeStart, edgeEnd);

        if (currInside)
        {
            if (!prevInside)
            {
                // 从外到内，添加交点
                output.push_back(ComputeIntersection(prev, curr, edgeStart, edgeEnd));
            }
            // 当前点在内，添加当前点
            output.push_back(curr);
        }
        else if (prevInside)
        {
            // 从内到外，只添加交点
            output.push_back(ComputeIntersection(prev, curr, edgeStart, edgeEnd));
        }

        prev = curr;
    }

    return output;
}

std::vector<CPoint> CClipAlgorithm::SutherlandHodgmanClip(
    const std::vector<CPoint>& polygon,
    const CRect& clipRect)
{
    if (polygon.size() < 3)
        return std::vector<CPoint>();

    std::vector<CPoint> output = polygon;

    // 裁剪窗口的四条边（按顺时针方向）
    // 左边
    CPoint leftStart(clipRect.left, clipRect.bottom);
    CPoint leftEnd(clipRect.left, clipRect.top);
    output = ClipPolygonByEdge(output, leftStart, leftEnd);

    // 上边
    CPoint topStart(clipRect.left, clipRect.top);
    CPoint topEnd(clipRect.right, clipRect.top);
    output = ClipPolygonByEdge(output, topStart, topEnd);

    // 右边
    CPoint rightStart(clipRect.right, clipRect.top);
    CPoint rightEnd(clipRect.right, clipRect.bottom);
    output = ClipPolygonByEdge(output, rightStart, rightEnd);

    // 下边
    CPoint bottomStart(clipRect.right, clipRect.bottom);
    CPoint bottomEnd(clipRect.left, clipRect.bottom);
    output = ClipPolygonByEdge(output, bottomStart, bottomEnd);

    return output;
}