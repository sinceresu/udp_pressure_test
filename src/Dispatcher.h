#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <boost/shared_ptr.hpp>
#include <mutex>
#include <thread>
#include <atomic>
#include "PackageConfig.h"
#include <list>
#include <vector>

class TargetDevice;
class io_service_pool;
class Statistic;
template <class T>class BoundedPool;

class Dispatcher
{
private:
	enum 
	{
		DEFAULT_DEVICE_NUM = 500,
		SEND_INTERVAL_MS = 100,	//每次突发的时长(毫秒)。
	};
	typedef std::shared_ptr<TargetDevice> device_ptr;
public:
	Dispatcher(size_t maxThreads);
	~Dispatcher();
	void setConnectionSpeed(int connectionsPerSec);
	int getConnectionSpeed() const ;
	void setTimeout(int timeoutMs);
	void setServerIp(const std::string& address, int port);
	void setStartPort(int startPort);
	void setDeviceNumber(size_t deviceNumber);
	void setMacList(const std::list<PacketConfig>& macInfos);
	bool start();
	void stop();

private:
	std::mutex m_mutex;

	bool initialize();
	void clean();
	void run();
	void onConnectionSuccess(device_ptr device, int delayInMS);
	void onConnectionFail(device_ptr device);
	bool _stopFlag;
	int _timeoutMs;
	std::string _serverAddress;
	int _serverPort;
	int _maxThreads;
	int _connectionSpeed;
	std::shared_ptr<io_service_pool> _servicePool;
	std::shared_ptr<BoundedPool<device_ptr> > _freeTargetDevicePool;
	std::shared_ptr<std::thread> _workThread;
	std::shared_ptr<Statistic>	_statistic;
	int _startPort;
	int _currentPort;
	size_t _deviceNumber;
	std::vector<PacketConfig> _packageConfig;
	std::atomic<int> _cntPackages;
};
#endif
