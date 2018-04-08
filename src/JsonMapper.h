#ifndef _JsonMapper_H_
#define _JsonMapper_H_

class TargetDevice;
class DistributorReply;

class JsonMapper
{
public:
	static void mapTargetDeviceToJson(const TargetDevice& targetDevice, char* szOutoutBuf, size_t bufSize, size_t* outputLen);
	static bool mapReplyFromJson(const char* data, size_t dataLen, DistributorReply& distrubuorReply);
};

#endif