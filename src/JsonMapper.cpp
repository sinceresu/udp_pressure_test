#ifdef WIN32
#include "stdafx.h"
#endif
#include "JsonMapper.h"
#include "TargetDevice.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "DistributorReply.h"

using boost::property_tree::ptree;

void JsonMapper::mapTargetDeviceToJson(const TargetDevice& targetDevice, char* szOutoutBuf, size_t bufSize, size_t* outputLen)
{
	ptree pt;
	pt.put("RPCMethod", targetDevice.getRPCMethod());
	pt.put("Vendor", targetDevice.getVendor());
	pt.put("Model", targetDevice.getModel());
	pt.put("FirmwareVer", targetDevice.getFirmwareVer());
	pt.put("HardwareVer", targetDevice.getHardwareVer());
	pt.put("MAC", targetDevice.getMAC());
	pt.put("PlatformID", targetDevice.getPlatformID());
	std::ostringstream outputStrStream;
	write_json(outputStrStream, pt, false);

	strncpy(szOutoutBuf, outputStrStream.str().c_str(), bufSize);

	std::string::size_type len = outputStrStream.str().length();

	//ptree will append CR/LF£¬rid of the character¡£
	if (szOutoutBuf[len - 1] == '\n')
	{
		szOutoutBuf[len - 1] = '\0';
		len--;
	}

	*outputLen = len;

}

bool JsonMapper::mapReplyFromJson(const char* data, size_t dataLen, DistributorReply& distribuorReply)
{
	if (NULL == data || 0 == dataLen)
	{
		return false;
	}
	char reply[512];
	memcpy(reply, data, dataLen);
	reply[dataLen] = '\0';
	std::istringstream inputStrStream(reply);
	ptree pt;
	try
	{
		read_json(inputStrStream, pt);
		distribuorReply._Result = pt.get<int>("Result");
	}
	catch (std::exception &)
	{
		return false;
	}
	return true;
}
