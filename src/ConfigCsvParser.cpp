#ifdef WIN32
#include "stdafx.h"
#endif
#include "ConfigCsvParser.h"

#include "minicsv.h"

bool ConfigCsvParser::parse(const char* csvFileName, std::list<PacketConfig>& macInfos)
{
	if (NULL == csvFileName)
		return false;
	macInfos.clear();
	csv::ifstream csvFile(csvFileName, std::ios_base::in);
	csvFile.set_delimiter(',');

	if (csvFile.is_open())
	{
		std::string head1, head2, head3;
		csvFile >> head1 >> head2 >> head3;

		PacketConfig temp;
		while (!csvFile.eof())
		{
			csvFile >> temp._RpcMode >> temp._MacAddress >> temp._SN;
			macInfos.push_back(temp);
		}
	}
	return true;
}
