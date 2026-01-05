#pragma once
#include <afxwin.h>
#include <vector>

// 裁剪算法类
class CClipAlgorithm
{
public:
    // ==================== Cohen-Sutherland 线段裁剪 ====================
    // 返回值：true=线段可见（部分或全部），false=线段完全不可见
    static bool CohenSutherlandClip(
        double x1, double y1, double x2, double y2,
        double xmin, double ymin, double xmax, double ymax,
        double& nx1, double& ny1, double& nx2, double& ny2);

    // ==================== Liang-Barsky 线段裁剪 ====================
    static bool LiangBarskyClip(
        double x1, double y1, double x2, double y2,
        double xmin, double ymin, double xmax, double ymax,
        double& nx1, double& ny1, double& nx2, double& ny2);

    // ==================== Sutherland-Hodgman 多边形裁剪 ====================
    // 输入：多边形顶点列表，裁剪窗口
    // 输出：裁剪后的多边形顶点列表
    static std::vector<CPoint> SutherlandHodgmanClip(
        const std::vector<CPoint>& polygon,
        const CRect& clipRect);

private:
    // Cohen-Sutherland 区域编码
    enum OutCode
    {
        INSIDE = 0,     // 0000
        LEFT = 1,       // 0001
        RIGHT = 2,      // 0010
        BOTTOM = 4,     // 0100
        TOP = 8         // 1000
    };

    static int ComputeOutCode(double x, double y,
        double xmin, double ymin, double xmax, double ymax);

    // Sutherland-Hodgman 辅助函数
    static std::vector<CPoint> ClipPolygonByEdge(
        const std::vector<CPoint>& polygon,
        const CPoint& edgeStart, const CPoint& edgeEnd);

    static bool IsInside(const CPoint& p, const CPoint& edgeStart, const CPoint& edgeEnd);
    static CPoint ComputeIntersection(const CPoint& p1, const CPoint& p2,
        const CPoint& edgeStart, const CPoint& edgeEnd);
};