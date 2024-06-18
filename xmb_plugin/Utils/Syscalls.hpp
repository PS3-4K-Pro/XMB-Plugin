
#pragma once

#include <sys/prx.h>
#include <sys/syscall.h>

int _sys_ppu_thread_exit(int errorcode);
int _sys_prx_stop_module(sys_prx_id_t id, unsigned int argc, void* argp, int* modres, sys_prx_flags_t flags, sys_prx_stop_module_option_t* pOpt);
sys_prx_id_t _sys_prx_get_module_id_by_address(void* address);
sys_prx_id_t _sys_prx_get_my_module_id();

int sys_dbg_get_console_type(uint64_t* type);
int sys_dbg_read_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size);
int sys_dbg_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size);

int sys_mapi_get_hen();
int sys_mapi_get_mamba();
int sys_mapi_get_version(uint32_t* version);
int sys_mapi_read_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size);
int sys_mapi_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size);

void ExitModuleThread();
void UnloadMyModule();
bool IsConsoleCex();
bool IsConsoleDex();
bool IsConsoleDeh();
bool IsPayloadHen();
bool IsPayloadMamba();
bool IsPayloadCobra();
int ReadProcessMemory(uint32_t pid, void* destination, const void* source, size_t size);
int WriteProcessMemory(uint32_t pid, void* destination, const void* source, size_t size);