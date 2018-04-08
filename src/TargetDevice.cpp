#ifdef WIN32
#include "stdafx.h"
#endif
#include "TargetDevice.h"
#include "EchoSession.h"
#include "JsonMapper.h"
#include "DistributorReply.h"

TargetDevice::TargetDevice(boost::asio::io_service& ioService, int port)
:_io_service(ioService)
, _localPort(port)
{
	_echoSession = std::shared_ptr<EchoSession>(new EchoSession(_io_service, port));
	_echoSession->attachReplyCallBack(std::bind(&TargetDevice::onReply, this, std::placeholders::_1, 
		std::placeholders::_2, std::placeholders::_3));
	_echoSession->attachTimeoutCallBack(std::bind(&TargetDevice::onTimeout, this));
	_echoSession->attachErrorCallBack(std::bind(&TargetDevice::onError, this, std::placeholders::_1,
		std::placeholders::_2));
}

TargetDevice::~TargetDevice()
{
}

void TargetDevice::setLocalPort(int port)
{
	_localPort = port;
}


void TargetDevice::startConnect()
{
	_echoSession->setServerEndPoint(_serverEndPoint);
	_echoSession->setTimeout(_timeoutMs);
	size_t requestLength = 0;
	JsonMapper::mapTargetDeviceToJson(*this, _request, sizeof(_request), &requestLength);
	_echoSession->doWrite(_request, requestLength);
}


void TargetDevice::onReply(const char* data, size_t dataLength, int delay)
{
	DistributorReply reply;
	
//	if (JsonMapper::mapReplyFromJson(data, dataLength, reply))
	{
		_successCallBack(shared_from_this(), delay);
	}
}

void TargetDevice::onTimeout()
{
	_failCallBack(shared_from_this());

}

void TargetDevice::onError(std::string message, int value)
{

}


void TargetDevice::attachSuccessCallBack(std::function<void(device_ptr device, int delayInMS)> successCallBack)
{
	_successCallBack = successCallBack;
}

void TargetDevice::attachFailCallBack(std::function<void(device_ptr device)> failCallBack)
{
	_failCallBack = failCallBack;
}

void TargetDevice::setRPCMethod(std::string RPCMethod)
{
	_RPCMethod = RPCMethod;
}

std::string TargetDevice::getRPCMethod() const
{
	return _RPCMethod;
}

void TargetDevice::setVendor(std::string Vendor)
{
	_Vendor = Vendor;

}

std::string TargetDevice::getVendor() const
{
	return _Vendor;
}

void TargetDevice::setModel(std::string Model)
{
	_Model = Model;
}

std::string TargetDevice::getModel() const
{
	return _Model;
}

void TargetDevice::setFirmwareVer(std::string FirmwareVer)
{
	_FirmwareVer = FirmwareVer;
}

std::string TargetDevice::getFirmwareVer() const
{
	return _FirmwareVer;
}

void TargetDevice::setHardwareVer(std::string HardwareVer)
{
	_HardwareVer = HardwareVer;
}

std::string TargetDevice::getHardwareVer() const
{
	return _HardwareVer;
}

void TargetDevice::setMAC(const std::string& localMAC)
{
	_MAC = localMAC;
}

std::string TargetDevice::getMAC() const
{
	return _MAC;
}

void TargetDevice::setPlatformID(std::string PlatformID)
{
	_PlatformID = PlatformID;
}

std::string TargetDevice::getPlatformID() const
{
	return _PlatformID;
}

void TargetDevice::setTimeout(int timeoutMs)
{
	_timeoutMs = timeoutMs;
}

void TargetDevice::setServerEndPoint(udp::endpoint serverEndPoint)
{
	_serverEndPoint = serverEndPoint;
}

int TargetDevice::getLocalPort()
{
	return _localPort;
}

int TargetDevice::getTimeout()
{
	return _timeoutMs;
}
