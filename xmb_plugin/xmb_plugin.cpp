
#include "xmb_plugin.hpp"
#include <cell/cell_fs.h>
#include "Utils/Memory/Detours.hpp"

using address_t = char[0x10];

// helpers
sys_prx_id_t GetModuleHandle(const char* moduleName)
{
	return (moduleName) ? sys_prx_get_module_id_by_name(moduleName, 0, nullptr) : sys_prx_get_my_module_id();
}

sys_prx_module_info_t GetModuleInfo(sys_prx_id_t handle)
{
	sys_prx_module_info_t info{};
	static sys_prx_segment_info_t segments[10]{};
	static char filename[SYS_PRX_MODULE_FILENAME_SIZE]{};

	stdc::memset(segments, 0, sizeof(segments));
	stdc::memset(filename, 0, sizeof(filename));

	info.size = sizeof(info);
	info.segments = segments;
	info.segments_num = sizeof(segments) / sizeof(sys_prx_segment_info_t);
	info.filename = filename;
	info.filename_size = sizeof(filename);

	sys_prx_get_module_info(handle, 0, &info);
	return info;
}

std::string GetModuleFilePath(const char* moduleName)
{
	sys_prx_module_info_t info = GetModuleInfo(GetModuleHandle(moduleName));
	return std::string(info.filename);
}

std::string RemoveBaseNameFromPath(const std::string& filePath)
{
	size_t lastPath = filePath.find_last_of("/");
	if (lastPath == std::string::npos)
		return filePath;
	return filePath.substr(0, lastPath);
}

std::string GetCurrentDir()
{
	static std::string cachedModulePath;
	if (cachedModulePath.empty())
	{
		std::string path = RemoveBaseNameFromPath(GetModuleFilePath(nullptr));

		path += "/";  // Include trailing slash

		cachedModulePath = path;
	}
	return cachedModulePath;
}

bool FileExists(const std::string& filePath)
{
	CellFsStat stat;
	if (cellFsStat(filePath.c_str(), &stat) == CELL_FS_SUCCEEDED)
		return (stat.st_mode & CELL_FS_S_IFREG);
	return false;
}

bool ReadFile(const std::string& filePath, void* data, size_t size)
{
	int fd;
	if (cellFsOpen(filePath.c_str(), CELL_FS_O_RDONLY, &fd, nullptr, 0) == CELL_FS_SUCCEEDED)
	{
		cellFsLseek(fd, 0, CELL_FS_SEEK_SET, nullptr);
		cellFsRead(fd, data, size, nullptr);
		cellFsClose(fd);

		return true;
	}
	return false;
}

bool ReplaceStr(std::wstring& str, const std::wstring& from, const std::string& to)
{
	size_t startPos = str.find(from);
	if (startPos == std::wstring::npos)
		return false;
	str.replace(startPos, from.length(), std::wstring(to.begin(), to.end()));
	return true;
}


// Global variables
bool gIsDebugXmbPlugin{ false };
wchar_t gIpBuffer[512]{0};
paf::View* xmb_plugin{};
paf::View* system_plugin{};
paf::PhWidget* page_xmb_indicator{};
paf::PhWidget* page_notification{};

// Function implementations			   
bool LoadIpText()
{
	//std::string ipTextPath = GetCurrentDir() + "xmb_plugin";
	std::string ipTextPath = "/dev_flash/vsh/resource/explore/xmb/pro.xml";
	char fileBuffer[512]{0};

	if (!FileExists(ipTextPath) || !ReadFile(ipTextPath, fileBuffer, sizeof(fileBuffer)))
		return false; // It's set to false to only shows the informations when PS3 4K Pro is installed (pro.xml file)

	// Not sure how but it converts utf8 chars
	stdc::swprintf(gIpBuffer, 512, L"%s", fileBuffer);

	// If ip_text already exist then we using the dex/deh xmb_plugin.sprx
	system_plugin = paf::View::Find("system_plugin");
	page_notification = system_plugin ? system_plugin->FindWidget("page_notification") : nullptr;

	if (page_notification && page_notification->FindChild("ip_text", 0) != nullptr)
		gIsDebugXmbPlugin = true;

	return true;
}

bool CanCreateIpText()
{
	paf::PhWidget* parent = GetParent();
	return parent ? parent->FindChild("ip_text", 0) == nullptr : false;
}

paf::PhWidget* GetParent()
{
	/* Crashes when changing users
	if (gIsDebugXmbPlugin)
		return page_notification;
	*/

	if (!page_xmb_indicator)
		return nullptr;

	return page_xmb_indicator->FindChild("indicator", 0);
}

std::wstring GetText()
{
	char ip[16];
	netctl::netctl_main_9A528B81(16, ip);
	
	std::wstring text(gIpBuffer);
	
	std::wstring systemIpAddress = L"System IP Address: ";
	
	std::wstring IpAddress;
    if (ip[0] != '\0') {
        IpAddress = std::wstring(ip, ip + strlen(ip));
    } else {
        IpAddress = L"0.0.0.0";
    }
	
	std::wstring serverName = L"Online Server: ";
	
	// DNS information
    xsetting_F48C0548_t* net = xsetting_F48C0548();
    xsetting_F48C0548_t::net_info_t netInfo;
    int error = net->GetNetworkConfig(&netInfo);

    std::wstring dnsPrimary(netInfo.primaryDns, netInfo.primaryDns + strlen(netInfo.primaryDns));
    std::wstring dnsSecondary(netInfo.secondaryDns, netInfo.secondaryDns + strlen(netInfo.secondaryDns));

    if (dnsPrimary == L"185.194.142.4" || dnsSecondary == L"185.194.142.4")
    {
        serverName += L"PlayStation Online Network Emulated";
    }
    else if (dnsPrimary == L"51.79.41.185" || dnsSecondary == L"51.79.41.185")
    {
        serverName += L"PlayStation Online Returnal Games";
    }
    else if (dnsPrimary == L"146.190.205.197" || dnsSecondary == L"146.190.205.197")
    {
        serverName += L"PlayStation Reborn";
    }
	else if (dnsPrimary == L"135.148.144.253" || dnsSecondary == L"135.148.144.253")
    {
        serverName += L"PlayStation Rewired";
    }
	else if (dnsPrimary == L"146.190.205.197" || dnsSecondary == L"146.190.205.197")
    {
        serverName += L"Far Cry 2 Revived";
    }
	else if (dnsPrimary == L"45.7.228.197" || dnsSecondary == L"45.7.228.197")
    {
        serverName += L"Open Spy";
    }
	else if (dnsPrimary == L"142.93.245.186" || dnsSecondary == L"142.93.245.186")
    {
        serverName += L"The ArchStones";
    }
	else if (dnsPrimary == L"188.225.75.35" || dnsSecondary == L"188.225.75.35")
    {
        serverName += L"WareHouse";
    }
	else if (dnsPrimary == L"64.20.35.146" || dnsSecondary == L"64.20.35.146")
    {
        serverName += L"Home Headquarters";
    }
	else if (dnsPrimary == L"52.86.120.101" || dnsSecondary == L"52.86.120.101")
    {
        serverName += L"Destination Home";
    }
    else
    {
        serverName += L"PlayStation™ Network";
    }
	
	//Append the IpAddress to systemIpAddress
	systemIpAddress += IpAddress ;
	
	text += L"\n";
    text += serverName + L"\n";
    text += systemIpAddress;

    return text;
}

void CreateIpText()
{
	paf::PhWidget* parent = GetParent();
	if (!parent)
		return;

	paf::PhText* ip_text = new paf::PhText(parent, nullptr);
	if (!ip_text)
		return;

	ip_text->SetName("ip_text");

	// Added due to the removal of the parent of dex/deh xmb_plugin.sprx. The ip text was overlapping, so I disabled the alpha in dex/deh xmb_plugin.sprx.
	if (gIsDebugXmbPlugin)
	{
		ip_text->SetColor({ 1.f, 1.f, 1.f, 0.f });
	}
	else
	{
		ip_text->SetColor({ 1.f, 1.f, 1.f, 1.f });
	}

	ip_text->SetStyle(19, int(112));
	ip_text->SetLayoutPos(0x60000, 0x50000, 0, { 820.f, -465.f, 0.f, 0.f });
	ip_text->SetLayoutStyle(0, 20, 0.f);
	ip_text->SetLayoutStyle(1, 217, 0.f);
	ip_text->SetStyle(56, true);
	ip_text->SetStyle(18, int(34));
	ip_text->SetStyle(49, int(2));
}


// Hooking
Detour* pafWidgetDrawThis_Detour;

int pafWidgetDrawThis_Hook(paf::PhWidget* _this, unsigned int r4, bool r5)
{
	if (_this && _this->m_Data.name == "ip_text")
	{
		paf::PhText* ip_text = (paf::PhText*)_this;

		if (vshmain::GetCooperationMode() == vshmain::CooperationMode::Game)
			ip_text->m_Data.metaAlpha = xmb_plugin ? 1.f : 0.f;

		if (ip_text->m_Data.metaAlpha > 0.1f)
		{
			ip_text->SetText(GetText(), 0);
		}
			
	}
	return pafWidgetDrawThis_Detour->GetOriginal<int>(_this, r4, r5);
}

void Install()
{
	pafWidgetDrawThis_Detour = new Detour(((opd_s*)paf::paf_63D446B8)->sub, pafWidgetDrawThis_Hook);
}

void Remove()
{
	if (pafWidgetDrawThis_Detour)
		delete pafWidgetDrawThis_Detour;
}