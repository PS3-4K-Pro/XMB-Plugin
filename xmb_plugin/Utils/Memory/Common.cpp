
#include "Common.hpp"
#include <vsh/stdc.hpp>

uint32_t GetCurrentToc()
{
	uint32_t* entry_point = *reinterpret_cast<uint32_t**>(0x1001C); // ElfHeader->e_entry 
	return entry_point[1];
}

opd_s* FindExportByName(const char* module, uint32_t fnid)
{
	uint32_t* segment15 = *reinterpret_cast<uint32_t**>(0x1008C); // 0x1008C or 0x10094
	uint32_t exportAdressTable = segment15[0x984 / sizeof(uint32_t)];
	exportStub_s* exportStub = reinterpret_cast<exportStub_s*>(exportAdressTable);

	while (exportStub->ssize == 0x1C00) 
	{
		if (!stdc::strcmp(module, exportStub->name)) 
		{
			for (int16_t i = 0; i < exportStub->exportsCount; i++) 
			{
				if (exportStub->fnid[i] == fnid)
					return exportStub->stub[i];
			}
		}
		exportStub++;
	}

	return nullptr;
}

opd_s* FindImportByName(const char* module, uint32_t fnid)
{
	uint32_t* segment15 = *reinterpret_cast<uint32_t**>(0x1008C); // 0x1008C or 0x10094
	uint32_t exportAdressTable = segment15[0x984 / sizeof(uint32_t)];
	importStub_s* importStub = reinterpret_cast<importStub_s*>(exportAdressTable);

	while (importStub->ssize == 0x2C00) 
	{
		if (!stdc::strcmp(module, importStub->name)) 
		{
			for (int16_t i = 0; i < importStub->importsCount; i++) 
			{
				if (importStub->fnid[i] == fnid)
					return importStub->stub[i];
			}
		}
		importStub++;
	}

	return nullptr;
}