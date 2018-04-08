#ifndef _TARGETDEVICE_H_
#define _TARGETDEVICE_H_

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

using boost::asio::ip::udp;
class EchoSession;
class TargetDevice : public std::enable_shared_from_this<TargetDevice>,
	private boost::noncopyable
{
private:
	enum { MAX_LENGTH = 1024 };

public:
	typedef std::shared_ptr<TargetDevice> device_ptr;

	TargetDevice(boost::asio::io_service& ioService, int port);
	~TargetDevice();
	void setServerEndPoint(udp::endpoint serverEndPoint);
	void setLocalPort(int port);
	int getLocalPort();
	void setTimeout(int timeoutMs);
	int getTimeout();

	void setRPCMethod(std::string RPCMethod);
	std::string getRPCMethod() const;
	void setVendor(std::string Vendor);
	std::string getVendor() const;
	void setModel(std::string Model);
	std::string getModel() const;
	void setFirmwareVer(std::string FirmwareVer);
	std::string getFirmwareVer() const;
	void setHardwareVer(std::string HardwareVer);
	std::string getHardwareVer() const;
	void setMAC(const std::string& MAC);
	std::string getMAC() const;
	void setPlatformID(std::string PlatformID);
	std::string getPlatformID() const;



	void startConnect();
	void attachSuccessCallBack(std::function<void(device_ptr device, int delayInMS)> successCallBack);
	void attachFailCallBack(std::function<void(device_ptr device)> failCallBack);

private:
	void onReply(const char* data, size_t dataSize, int delay);
	void onTimeout();
	void onError(std::string message, int value);

private: 
	int totalConnections;
	boost::asio::io_service &_io_service;
	udp::endpoint _serverEndPoint;
	int _localPort;
	int _timeoutMs;
	std::shared_ptr<EchoSession> _echoSession;
	std::function<void(device_ptr device, int delayInMS)> _successCallBack;
	std::function<void(device_ptr device)> _failCallBack;
	char _request[MAX_LENGTH];

	std::string _RPCMethod;
	std::string _Vendor;
	std::string _Model;
	std::string _FirmwareVer;
	std::string _HardwareVer;
	std::string _MAC;
	std::string _PlatformID;
	
};

#endif