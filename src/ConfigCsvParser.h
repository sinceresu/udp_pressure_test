#pragma once
#include <string>
#include <memory>
#include <list>
#include "PackageConfig.h"

class ConfigCsvParser
{
public:
	static bool parse(const char* csvFileName, std::list<PacketConfig>& macInfos);

};

