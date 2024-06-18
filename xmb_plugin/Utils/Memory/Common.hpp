
#pragma once

#include <stdint.h>

struct opd_s {
	uint32_t sub;
	uint32_t toc;
};

struct exportStub_s {
	int16_t ssize;
	int16_t header1;
	int16_t header2;
	int16_t exportsCount;
	int32_t zero1;
	int32_t zero2;
	const char* name;
	uint32_t* fnid;
	opd_s** stub;
};

struct importStub_s {
	int16_t ssize;
	int16_t header1;
	int16_t header2;
	int16_t importsCount;
	int32_t zero1;
	int32_t zero2;
	const char* name;
	uint32_t* fnid;
	opd_s** stub;
	int32_t zero3;
	int32_t zero4;
	int32_t zero5;
	int32_t zero6;
};

uint32_t GetCurrentToc();
opd_s* FindExportByName(const char* module, uint32_t fnid);
opd_s* FindImportByName(const char* module, uint32_t fnid);

template <typename R, typename... Args>
inline R CallByAddr(uint32_t addr, Args... args) {
	volatile opd_s opd = { addr, GetCurrentToc() };
	R(*fn)(Args...) = (R(*)(Args...))&opd;
	return fn(args...);
}

template <typename R, typename... Args>
inline R CallByOpd(opd_s opd, Args... args) {
	if (opd.sub && opd.toc) {
		R(*fn)(Args...) = (R(*)(Args...))&opd;
		return fn(args...);
	}
	return (R)0;
}
