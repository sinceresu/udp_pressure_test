#ifdef WIN32
#include "stdafx.h"
#endif
#include "EchoSession.h"
#include <cstdlib>
#include <iostream>

EchoSession::EchoSession(boost::asio::io_service& service,
	int port)
	: _socket(service, udp::endpoint(udp::v4(), port))
	, _timerTimeout(service)
	, _localPort(port)
	, _isStopped(true)
{

}


EchoSession::~EchoSession()
{
}

void EchoSession::setTimeout(int timeoutMS)
{
	_timeoutMs = timeoutMS;
}


void EchoSession::setServerEndPoint(udp::endpoint serverEndPoint)
{
	_serverEndPoint = serverEndPoint;
}

void EchoSession::attachReplyCallBack(ReplayCallBack_t replyCallBack)
{
	_replyCallBack = replyCallBack;
}

void EchoSession::attachTimeoutCallBack(TimeoutCallBack_t timeoutCallback)
{
	_timeoutCallBack = timeoutCallback;
}

void EchoSession::attachErrorCallBack(ErrorCallBack_t errorCallback)
{
	_errorCallback = errorCallback;
}


void EchoSession::doWrite(const char* data, size_t dataSize)
{
	if (!_socket.is_open())
	{
		_socket = udp::socket(_socket.get_io_service(), udp::endpoint(udp::v4(), _localPort));
	}
	_isStopped = false;
	_sendClock = clock();
	_socket.async_send_to(boost::asio::buffer(data, dataSize), _serverEndPoint, 
		[this](boost::system::error_code ec, std::size_t bytes_sent)
	{
		if (!ec)
		{
			doRead();
		}
		else
		{
			std::cout << ec.message() << std::endl;
		}
	});

}

void EchoSession::doRead()
{
	_timerTimeout.expires_from_now(boost::posix_time::seconds(_timeoutMs/1000));
	_socket.async_receive_from(
		boost::asio::buffer(_reply, sizeof(_reply)), _serverEndPoint,
		[this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
			if (!ec)
			{
				_timerTimeout.cancel();
				_isStopped = true;
				int duration = clock() - _sendClock;
				_replyCallBack(_reply, bytes_recvd, duration);
			}
			else if (ec.value() == 995) //io cancelled
			{
				//std::cout << ec.message() << " value " << ec.value() << ", port " << _localPort << std::endl;
				_isStopped = true;
				_timeoutCallBack();
			} 
			else // other io error
			{
				std::cout << ec.message() << " value " << ec.value() << ", port " << _localPort << std::endl;
				stop();
				_errorCallback(ec.message(), ec.value());
				_isStopped = true;
			}
        });

	_timerTimeout.async_wait(
		std::bind(&EchoSession::check_deadline, this));
		/*
		[this]()
		{
			_socket.cancel();
		}
	*/
}
void EchoSession::check_deadline()
{
	if (_timerTimeout.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		_socket.cancel();
		//std::cout << "time out happened, port " << _localPort << std::endl;
		//_timeoutCallBack();
	}
}


void EchoSession::stop()
{
	_timerTimeout.cancel();
	boost::system::error_code ignored_ec;
	_socket.close(ignored_ec);
}

