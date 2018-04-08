// DistributorTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "Dispatcher.h"
#include "ConfigCsvParser.h"
#include "PackageConfig.h"
#include <boost/system/system_error.hpp>


using namespace std;
using namespace boost::program_options;

void waitQuit()
{
	cout << "[distributorTest] #";
	while (1)
	{
		char input = _getch();
		if (input == 'q')
			break;
		cout << "[distributorTest] #";
	}

}

int main(int argc, char* argv[])
{
	int result = 0;
	options_description opts("distributor test");
	opts.add_options()
		("help", "just a help info")
		("speed", value<int>()->default_value(1000), "connections per second")
		("ip", value<std::string>()->default_value("127.0.0.1"), "ip address of distributor server")
		("port", value<int>()->default_value(1200), "ip port of distributor server")
		("timeout", value<int>()->default_value(1000), "timeout in millisecond for connection fail")
		("start_port", value<int>()->default_value(1000), " port for the first test device, one is increased for every device")
		("device_num", value<int>()->default_value(500), "the number of devices for test")
		("config_file", value<string>()->default_value("config.csv"), "package config csv file");

	variables_map vm;
	try
	{
		store(parse_command_line(argc, argv, opts), vm);
	}
	catch (exception& e)
	{
		cout << "command error" << e.what() << std::endl;
		_getch();
		return -1;
	}

	if (vm.count("help"))
	{
		cout << opts << endl;
	}
	int speed = vm["speed"].as<int>();
	int timeout = vm["timeout"].as<int>();
	string hostAddress = vm["ip"].as<string>();
	int hostPort = vm["port"].as<int>();
	string configFile = vm["config_file"].as<string>();
	int startPort = vm["start_port"].as<int>();
	size_t deviceNumber = vm["device_num"].as<int>();
	list<PacketConfig> macInfos;

	if (!ConfigCsvParser::parse(configFile.c_str(), macInfos))
	{
		cout << "Config File: " << configFile << " does not exist!" << std::endl;
		_getch();
		return -1;
	}

	shared_ptr<Dispatcher> dispatcher(new Dispatcher(6));

	dispatcher->setConnectionSpeed(speed);
	dispatcher->setTimeout(timeout);
	dispatcher->setServerIp(hostAddress, hostPort);
	dispatcher->setStartPort(startPort);
	dispatcher->setDeviceNumber(deviceNumber);
	dispatcher->setMacList(macInfos);

	try
	{
		dispatcher->start();
	}
	catch (boost::system::system_error& e)
	{
		cout << "failed to connect, error message is  " << e.what() << std::endl;
	}

	waitQuit();

	dispatcher->stop();

	_getch();
	return 0;
}

