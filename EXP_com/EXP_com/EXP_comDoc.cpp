#include "pch.h"
#include "framework.h"
#include "EXP_com.h"
#include "EXP_comDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CEXPcomDoc, CDocument)

BEGIN_MESSAGE_MAP(CEXPcomDoc, CDocument)
END_MESSAGE_MAP()

CEXPcomDoc::CEXPcomDoc() noexcept
    : m_nSelectedIndex(-1)
{
}

CEXPcomDoc::~CEXPcomDoc()
{
}

BOOL CEXPcomDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    m_vecShapes.clear();
    m_nSelectedIndex = -1;
    m_undoStack.clear();
    m_redoStack.clear();

    return TRUE;
}

void CEXPcomDoc::DeleteContents()
{
    m_vecShapes.clear();
    m_nSelectedIndex = -1;
    m_undoStack.clear();
    m_redoStack.clear();
    CDocument::DeleteContents();
}

void CEXPcomDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
    }
    else
    {
    }
}

// ==================== 图形管理 ====================

void CEXPcomDoc::AddShape(CShape* pShape)
{
    if (pShape)
    {
        SaveUndoState();  // 保存撤销状态
        m_vecShapes.push_back(std::unique_ptr<CShape>(pShape->Clone()));
        SetModifiedFlag(TRUE);
    }
}

void CEXPcomDoc::ClearAllShapes()
{
    SaveUndoState();  // 保存撤销状态
    m_vecShapes.clear();
    m_nSelectedIndex = -1;
    SetModifiedFlag(TRUE);
    UpdateAllViews(NULL);
}

// ==================== 选择管理 ====================

void CEXPcomDoc::ClearSelection()
{
    if (m_nSelectedIndex >= 0 && m_nSelectedIndex < (int)m_vecShapes.size())
    {
        m_vecShapes[m_nSelectedIndex]->SetSelected(FALSE);
    }
    m_nSelectedIndex = -1;
}

void CEXPcomDoc::SelectShapeByRect(CRect rect)
{
    ClearSelection();
    rect.NormalizeRect();

    for (int i = (int)m_vecShapes.size() - 1; i >= 0; --i)
    {
        if (m_vecShapes[i]->HitTestRect(rect))
        {
            m_nSelectedIndex = i;
            m_vecShapes[i]->SetSelected(TRUE);
            return;
        }
    }
}

void CEXPcomDoc::SelectShapeAt(CPoint point)
{
    ClearSelection();

    for (int i = (int)m_vecShapes.size() - 1; i >= 0; --i)
    {
        if (m_vecShapes[i]->HitTest(point))
        {
            m_nSelectedIndex = i;
            m_vecShapes[i]->SetSelected(TRUE);
            return;
        }
    }
}

CShape* CEXPcomDoc::GetSelectedShape()
{
    if (m_nSelectedIndex >= 0 && m_nSelectedIndex < (int)m_vecShapes.size())
    {
        return m_vecShapes[m_nSelectedIndex].get();
    }
    return nullptr;
}

void CEXPcomDoc::DeleteSelected()
{
    if (m_nSelectedIndex >= 0 && m_nSelectedIndex < (int)m_vecShapes.size())
    {
        SaveUndoState();  // 保存撤销状态
        m_vecShapes.erase(m_vecShapes.begin() + m_nSelectedIndex);
        m_nSelectedIndex = -1;
        SetModifiedFlag(TRUE);
    }
}

void CEXPcomDoc::TransformSelected(double matrix[3][3])
{
    CShape* pShape = GetSelectedShape();
    if (pShape)
    {
        SaveUndoState();  // 保存撤销状态
        pShape->Transform(matrix);
        SetModifiedFlag(TRUE);
    }
}

// ==================== 撤销/重做 ====================

std::vector<std::unique_ptr<CShape>> CEXPcomDoc::CloneShapes() const
{
    std::vector<std::unique_ptr<CShape>> cloned;
    for (const auto& pShape : m_vecShapes)
    {
        cloned.push_back(std::unique_ptr<CShape>(pShape->Clone()));
    }
    return cloned;
}

void CEXPcomDoc::RestoreShapes(std::vector<std::unique_ptr<CShape>>& shapes)
{
    m_vecShapes = std::move(shapes);
    m_nSelectedIndex = -1;
}

void CEXPcomDoc::SaveUndoState()
{
    // 克隆当前状态并压入撤销栈
    m_undoStack.push_back(CloneShapes());

    // 限制撤销栈大小
    while (m_undoStack.size() > MAX_UNDO_LEVELS)
    {
        m_undoStack.pop_front();
    }

    // 执行新操作时清空重做栈
    m_redoStack.clear();
}

void CEXPcomDoc::Undo()
{
    if (m_undoStack.empty())
        return;

    // 保存当前状态到重做栈
    m_redoStack.push_back(CloneShapes());

    // 恢复撤销栈顶状态
    RestoreShapes(m_undoStack.back());
    m_undoStack.pop_back();

    SetModifiedFlag(TRUE);
    UpdateAllViews(NULL);
}

void CEXPcomDoc::Redo()
{
    if (m_redoStack.empty())
        return;

    // 保存当前状态到撤销栈
    m_undoStack.push_back(CloneShapes());

    // 恢复重做栈顶状态
    RestoreShapes(m_redoStack.back());
    m_redoStack.pop_back();

    SetModifiedFlag(TRUE);
    UpdateAllViews(NULL);
}