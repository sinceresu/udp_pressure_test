#ifdef WIN32
#include "stdafx.h"
#endif
#include "Dispatcher.h"
#include <boost/asio.hpp>

#include <functional>
#include <iostream>

#include "io_service_pool.hpp"
#include "BoundedPool.h"
#include "TargetDevice.h"
#include "Statistic.h"

using boost::asio::ip::udp;

Dispatcher::Dispatcher(size_t maxThreads)
:_servicePool(new io_service_pool(maxThreads))
, _deviceNumber(DEFAULT_DEVICE_NUM)
, _statistic(new Statistic())
, _cntPackages(0)
{

}

Dispatcher::~Dispatcher()
{
}

void Dispatcher::setConnectionSpeed(int connectionsPerSec)
{
	_connectionSpeed = connectionsPerSec;
}

int Dispatcher::getConnectionSpeed() const
{
	return _connectionSpeed;
}

bool Dispatcher::initialize()
{
	_freeTargetDevicePool = std::shared_ptr<BoundedPool<device_ptr> >(new BoundedPool<device_ptr>(_deviceNumber + 1));

	for (size_t i = 0; i < _deviceNumber; i++)
	{
		boost::asio::io_service &service = _servicePool->get_io_service();
		_freeTargetDevicePool->add(device_ptr(new TargetDevice(service, _startPort++)));
	}

	_stopFlag = false;
	return true;
}

void Dispatcher::clean()
{
	_freeTargetDevicePool->clear();
	_freeTargetDevicePool.reset();
}

void Dispatcher::setServerIp(const std::string& address, int port)
{
	_serverAddress = address;
	_serverPort = port;
}

void Dispatcher::setStartPort(int startPort)
{
	_startPort = startPort;
	_currentPort = _startPort;
}

void Dispatcher::setDeviceNumber(size_t deviceNumber)
{
	_deviceNumber = deviceNumber;
}

void Dispatcher::run()
{
	if (false == initialize())
		return ;

	std::shared_ptr<boost::thread> pool_thread(new boost::thread(
		boost::bind(&io_service_pool::run, _servicePool)));

	int packagsPerTime = _connectionSpeed / (1000 / SEND_INTERVAL_MS);
	udp::endpoint endpoint(boost::asio::ip::address::from_string(_serverAddress), _serverPort);

	if (_packageConfig.empty())
	{
		_packageConfig.push_back(PacketConfig({ "92b000000022", "00000" }));
	}

	std::vector<PacketConfig>::size_type configIndex = 0;

	while (!_stopFlag)
	{
		auto start = std::chrono::system_clock::now();
		auto nextSecond = start + std::chrono::seconds(1);
		auto next = start;

		//control speed of sending packets¡£
		for (size_t i = 0; i < 1000 / SEND_INTERVAL_MS; i++)
		{
			next += std::chrono::milliseconds(SEND_INTERVAL_MS);
			for (size_t j = 0; j < packagsPerTime; j++)
			{
				device_ptr device;
				if (_freeTargetDevicePool->pop(&device, 10))
				{
					device->setServerEndPoint(endpoint);
				//	device->setLocalPort(getPort());
					device->setTimeout(_timeoutMs);
					//device->setRPCMethod("BootFirst");
					device->setRPCMethod(_packageConfig[configIndex]._RpcMode);
					device->setVendor("Vendor");
					device->setFirmwareVer("1");
					device->setHardwareVer("1");
					device->setMAC(_packageConfig[configIndex]._MacAddress);
					device->setPlatformID("OTHER");
					device->attachSuccessCallBack(std::bind(&Dispatcher::onConnectionSuccess, this, 
						std::placeholders::_1, std::placeholders::_2));
					device->attachFailCallBack(std::bind(&Dispatcher::onConnectionFail, this, 
						std::placeholders::_1));

					//std::cout << "start connection, port: " << device->getLocalPort() << std::endl;
					device->startConnect();

					configIndex++;
					if (configIndex == _packageConfig.size())
						configIndex = 0;
				}
			}
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			if (now > nextSecond)
				break;
			std::this_thread::sleep_until(next);
		}
	}
	pool_thread->join();

	clean();
	std::cout <<  std::endl << "test finished! ";
	auto statResult = _statistic->getTotalStatResult();
	std::cout << _cntPackages << " connections sent! average delay is " << statResult._averageDelay << "ms" <<
		" lost rate is " << statResult._lostPermillage << "¡ë";
	std::cout << std::endl;
}


bool Dispatcher::start()
{
	_workThread = std::shared_ptr<std::thread> (new std::thread(std::bind(&Dispatcher::run, this)));
	return true;
}

void Dispatcher::stop()
{
	_servicePool->stop();
	_stopFlag = true;
	_workThread->join();

}

void Dispatcher::onConnectionSuccess(device_ptr device, int delayInMS)
{
	_statistic->onConnectionSucceed(delayInMS);
	int packages = _cntPackages.fetch_add(1);
	//std::cout << "Success, port: " << device->getLocalPort() << " delay is " << delayInMS << "ms" << std::endl;
	if (packages % _connectionSpeed == 0)
	{
		auto statResult = _statistic->getCurrentStatResult();
		std::cout <<  std::endl << packages << " connections sent! average delay is " << statResult._averageDelay << "ms" << 
			" lost rate is " << statResult._lostPermillage << "¡ë";
	}
	_freeTargetDevicePool->push(device);
}


void Dispatcher::onConnectionFail(device_ptr device)
{
	_statistic->onConnectionFailed();
	_cntPackages++;
	std::cout << std::endl << "Fail, port: " << device->getLocalPort();
	_freeTargetDevicePool->push(device);
}

void Dispatcher::setTimeout(int timeoutMs)
{
	_timeoutMs = timeoutMs;
}

void Dispatcher::setMacList(const std::list<PacketConfig>& macInfos)
{
	_packageConfig.assign(macInfos.begin(), macInfos.end());
}




