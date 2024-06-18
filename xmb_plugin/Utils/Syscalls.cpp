
#include "Syscalls.hpp"
#include <errno.h>


int _sys_ppu_thread_exit(int errorcode)
{
	system_call_1(41, (uint64_t)errorcode);
	return_to_user_prog(int);
}

int _sys_prx_stop_module(sys_prx_id_t id, unsigned int argc, void* argp, int* modres, sys_prx_flags_t flags, sys_prx_stop_module_option_t* pOpt)
{
	system_call_6(482, (uint64_t)id, (uint64_t)argc, (uint64_t)argp, (uint64_t)modres, (uint64_t)flags, (uint64_t)pOpt);
	return_to_user_prog(int);
}

sys_prx_id_t _sys_prx_get_module_id_by_address(void* address)
{
	system_call_1(461, (uint64_t)address);
	return_to_user_prog(sys_prx_id_t);
}

sys_prx_id_t _sys_prx_get_my_module_id()
{
	return _sys_prx_get_module_id_by_address((void*)_sys_prx_get_my_module_id);
}


// sys_dbg
int sys_dbg_get_console_type(uint64_t* type)
{
	system_call_1(985, (uint64_t)type);
	return_to_user_prog(int);
}

int sys_dbg_read_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
	system_call_4(904, (uint64_t)pid, (uint64_t)destination, size, (uint64_t)source);
	return_to_user_prog(int);
}

int sys_dbg_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
	system_call_4(905, (uint64_t)pid, (uint64_t)destination, size, (uint64_t)source);
	return_to_user_prog(int);
}

// sys_mapi
int sys_mapi_get_hen()
{
	system_call_1(8, 0x1337ULL);
	return_to_user_prog(int);
}

int sys_mapi_get_mamba()
{
	system_call_1(8, 0x7FFFULL);
	return_to_user_prog(int);
}

int sys_mapi_get_version(uint32_t* version)
{
	system_call_2(8, 0x7000ULL, (uint64_t)(uint32_t)version);
	return_to_user_prog(int);
}

int sys_mapi_read_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
	system_call_6(8, 0x7777ULL, 0x0031ULL, (uint64_t)pid, (uint64_t)destination, (uint64_t)source, size);
	return_to_user_prog(int);
}

int sys_mapi_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
	system_call_6(8, 0x7777ULL, 0x0032ULL, (uint64_t)pid, (uint64_t)destination, (uint64_t)source, size);
	return_to_user_prog(int);
}

// Customs
void ExitModuleThread()
{
	_sys_ppu_thread_exit(0);
}

void UnloadMyModule()
{
	sys_prx_id_t myModuleId = _sys_prx_get_my_module_id();
	uint64_t memInfo[5]{0x28, 2, 0, 0, 0};
	_sys_prx_stop_module(myModuleId, 0, memInfo, nullptr, 0, nullptr);
}

bool IsConsoleCex()
{
	uint64_t type;
	sys_dbg_get_console_type(&type);
	return type == 1;
}

bool IsConsoleDex()
{
	uint64_t type;
	sys_dbg_get_console_type(&type);
	return type == 2;
}

bool IsConsoleDeh()
{
	uint64_t type;
	sys_dbg_get_console_type(&type);
	return type == 3;
}

bool IsPayloadHen()
{
	return sys_mapi_get_hen() == 0x1337;
}

bool IsPayloadMamba()
{
	return sys_mapi_get_mamba() == 0x666;
}

bool IsPayloadCobra()
{
	uint32_t version = 0x99999999;
	if (sys_mapi_get_version(&version) < 0)
		return false;

	return (version != 0x99999999); // If value changed, it is cobra
}

static bool bCanUseDbgSyscalls = true;
static bool bCanUseCobraSyscalls = true;
static bool bConsoleTypeChecked = false;

int ReadProcessMemory(uint32_t pid, void* destination, const void* source, size_t size)
{
	if (!bConsoleTypeChecked)
	{
		bCanUseDbgSyscalls = !IsConsoleCex();
		bCanUseCobraSyscalls = IsPayloadCobra();
		bConsoleTypeChecked = true;
	}

	if (bCanUseDbgSyscalls)
		return sys_dbg_read_process_memory(pid, destination, source, size);

	if (bCanUseCobraSyscalls)
		return sys_mapi_read_process_memory(pid, destination, source, size);

	return ENOSYS; /* The feature is not yet implemented. */
}

int WriteProcessMemory(uint32_t pid, void* destination, const void* source, size_t size)
{
	if (!bConsoleTypeChecked)
	{
		bCanUseDbgSyscalls = !IsConsoleCex();
		bCanUseCobraSyscalls = IsPayloadCobra();
		bConsoleTypeChecked = true;
	}

	if (bCanUseDbgSyscalls)
		return sys_dbg_write_process_memory(pid, destination, source, size);

	if (bCanUseCobraSyscalls)
		return sys_mapi_write_process_memory(pid, destination, source, size);

	return ENOSYS; /* The feature is not yet implemented. */
}