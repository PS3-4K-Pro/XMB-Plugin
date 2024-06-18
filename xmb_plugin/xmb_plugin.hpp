
#pragma once

#include <vshlib.hpp>
//extern bool gIsDebugXmbPlugin;
extern wchar_t gIpBuffer[512];
extern paf::View* xmb_plugin;
extern paf::View* system_plugin;
extern paf::PhWidget* page_xmb_indicator;
extern paf::PhWidget* page_notification;

bool LoadIpText();
bool CanCreateIpText();
paf::PhWidget* GetParent();
std::wstring GetText();
void CreateIpText(); 
void Install();
void Remove();