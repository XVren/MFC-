#pragma once
#include <vector>
#include <memory>
#include <deque>
#include "Shape.h"
#include "ShapeStyle.h"

class CEXPcomDoc : public CDocument
{
protected:
    CEXPcomDoc() noexcept;
    DECLARE_DYNCREATE(CEXPcomDoc)

public:
    // 图形管理
    void AddShape(CShape* pShape);
    const std::vector<std::unique_ptr<CShape>>& GetShapes() const { return m_vecShapes; }
    std::vector<std::unique_ptr<CShape>>& GetShapes() { return m_vecShapes; }
    void ClearAllShapes();
    int GetShapeCount() const { return (int)m_vecShapes.size(); }

    // 选择管理
    void ClearSelection();
    void SelectShapeByRect(CRect rect);
    void SelectShapeAt(CPoint point);
    CShape* GetSelectedShape();
    int GetSelectedIndex() const { return m_nSelectedIndex; }
    void DeleteSelected();

    // 变换
    void TransformSelected(double matrix[3][3]);

    // 样式
    CShapeStyle& GetCurrentStyle() { return m_currentStyle; }
    const CShapeStyle& GetCurrentStyle() const { return m_currentStyle; }

    // ===== 撤销/重做 =====
    void SaveUndoState();   // 保存当前状态到撤销栈
    void Undo();
    void Redo();
    bool CanUndo() const { return !m_undoStack.empty(); }
    bool CanRedo() const { return !m_redoStack.empty(); }

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual void DeleteContents();

public:
    virtual ~CEXPcomDoc();

protected:
    std::vector<std::unique_ptr<CShape>> m_vecShapes;
    CShapeStyle m_currentStyle;
    int m_nSelectedIndex;

    // ===== 撤销/重做栈 =====
    static const int MAX_UNDO_LEVELS = 20;  // 最大撤销层数
    std::deque<std::vector<std::unique_ptr<CShape>>> m_undoStack;
    std::deque<std::vector<std::unique_ptr<CShape>>> m_redoStack;

    // 克隆当前图形列表
    std::vector<std::unique_ptr<CShape>> CloneShapes() const;
    void RestoreShapes(std::vector<std::unique_ptr<CShape>>& shapes);

    DECLARE_MESSAGE_MAP()
};