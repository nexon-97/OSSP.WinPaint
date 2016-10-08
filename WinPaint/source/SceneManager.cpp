#include "stdafx.h"
#include "SceneManager.h"
#include "AppContext.h"

using namespace paint;

/////////////////////////////////////////////////////

SceneManager::SceneManager()
{
	
}

/////////////////////////////////////////////////////

SceneManager::~SceneManager()
{

}

/////////////////////////////////////////////////////

void SceneManager::NotifyCreationFinished()
{
	auto context = AppContext::GetInstance();
	context->GetRenderer()->Refresh();

	m_undoBuffer.clear();
	UpdateHistoryButtons();
}

/////////////////////////////////////////////////////

void SceneManager::NotifyCreationStateChanged()
{
	auto context = AppContext::GetInstance();
	auto currentShape = context->GetCurrentTool()->GetShape();

	auto shapesCount = m_shapes.size();
	if ((shapesCount == 0) || (currentShape != m_shapes[shapesCount - 1]))
	{
		m_shapes.push_back(currentShape);
	}

	AppContext::GetInstance()->GetRenderer()->Refresh();
	UpdateHistoryButtons();
}

/////////////////////////////////////////////////////

void SceneManager::UndoAction()
{
	auto shapesCount = m_shapes.size();
	if (shapesCount > 0)
	{
		auto lastShape = m_shapes[shapesCount - 1];
		m_shapes.pop_back();

		// Add to undo queue
		m_undoBuffer.push_back(lastShape);

		AppContext::GetInstance()->GetRenderer()->Refresh();
	}

	UpdateHistoryButtons();
}

/////////////////////////////////////////////////////

void SceneManager::RedoAction()
{
	auto undoBufferSize = m_undoBuffer.size();
	if (undoBufferSize > 0)
	{
		auto lastShape = m_undoBuffer[undoBufferSize - 1];
		m_undoBuffer.pop_back();

		// Put back to scene
		m_shapes.push_back(lastShape);

		AppContext::GetInstance()->GetRenderer()->Refresh();
	}

	UpdateHistoryButtons();
}

/////////////////////////////////////////////////////

void SceneManager::SetButtonsId(DWORD undoButtonId, DWORD redoButtonId)
{
	m_undoButtonId = undoButtonId;
	m_redoButtonId = redoButtonId;
}

/////////////////////////////////////////////////////

void SceneManager::UpdateHistoryButtons()
{
	HWND hwnd = AppContext::GetInstance()->GetWindowHandle();
	HMENU menu = GetMenu(hwnd);
	
	EnableMenuItem(menu, m_undoButtonId, m_shapes.size() ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(menu, m_redoButtonId, m_undoBuffer.size() ? MF_ENABLED : MF_GRAYED);
}

/////////////////////////////////////////////////////