#include "stdafx.h"
#include "Toolbar.h"
#include "AppContext.h"
#include <algorithm>

using namespace paint;

/////////////////////////////////////////////////////

Toolbar::Toolbar()
{
	// Setup default style
	m_style.m_penColor = 0xFF000000;
	m_style.m_brushColor = 0xFFFFFFFF;
	m_style.m_penThickness = 2;

	InitDefaultFont();
}

/////////////////////////////////////////////////////

void Toolbar::InitDefaultFont()
{
	LOGFONTA logFont;
	ZeroMemory(&logFont, sizeof(LOGFONTA));

	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	LPCSTR fontName = "Arial";
	memcpy(logFont.lfFaceName, fontName, strlen(fontName));
	logFont.lfWidth = 0;
	logFont.lfHeight = -24;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = FALSE;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = FF_DONTCARE;

	m_font.reset(new Font(&logFont));
}

/////////////////////////////////////////////////////

ICreator* Toolbar::GetTool(Tool tool) const
{	
	return InternalGetTool(tool);
}

/////////////////////////////////////////////////////

ICreator* Toolbar::RegisterTool(Tool toolType, ICreator* creator)
{
	auto mapTool = InternalGetTool(toolType);
	if (!mapTool)
	{
		int toolId = toolType.value();
		m_tools.insert(std::pair<int, ICreator*>(toolId, creator));
	}

	return creator;
}

/////////////////////////////////////////////////////

void Toolbar::SelectTool(Tool tool)
{
	m_activeTool = InternalGetTool(tool);
	CheckMenuTool(tool);
}

/////////////////////////////////////////////////////

ICreator* Toolbar::InternalGetTool(Tool tool) const
{
	int toolId = tool.value();
	auto it = m_tools.find(toolId);
	if (it != m_tools.end())
	{
		return it->second;
	}

	return nullptr;
}

/////////////////////////////////////////////////////

void Toolbar::CheckMenuTool(Tool tool)
{
	HWND hwnd = AppContext::GetInstance()->GetWindowHandle();
	HMENU menu = GetMenu(hwnd);

	// Uncheck all
	for (auto item : m_menuItemsAssoc)
	{
		CheckMenuItem(menu, item.first, MF_UNCHECKED);
	}

	auto predicate = [tool](std::pair<const DWORD, paint::Tool>& pair)
	{
		return (pair.second.value() == tool.value());
	};

	auto selectedItem = std::find_if(m_menuItemsAssoc.begin(), m_menuItemsAssoc.end(), predicate);
	if (selectedItem != m_menuItemsAssoc.end())
	{
		CheckMenuItem(menu, selectedItem->first, MF_CHECKED);
	}
}

/////////////////////////////////////////////////////

Tool Toolbar::GetToolByName(const char* name)
{
	for (auto it : m_tools)
	{
		Tool tool = static_cast<Tool::type>(it.first);
		if (strcmp(tool.name(), name) == 0)
		{
			return tool;
		}
	}

	return Tool::None;
}

/////////////////////////////////////////////////////
