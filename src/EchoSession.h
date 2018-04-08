#ifndef _ECHO_SESSION_H_
#define _ECHO_SESSION_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::udp;

class EchoSession:	private boost::noncopyable
{
	typedef std::function<void(const char* data, size_t dataSize, int delay)> ReplayCallBack_t;
	typedef std::function<void()> TimeoutCallBack_t;
	typedef std::function<void(const std::string& message, int value)> ErrorCallBack_t;
private:
	enum { MAX_LENGTH = 1024 };
	typedef std::shared_ptr<boost::asio::io_service> io_service_ptr;
public:
	EchoSession(boost::asio::io_service& service, int port);
	~EchoSession();
	void setServerEndPoint(udp::endpoint serverEndPoint);
	void doWrite(const char* data, size_t dataSize);
	void attachReplyCallBack(ReplayCallBack_t replyCallBack);
	void attachTimeoutCallBack(TimeoutCallBack_t timeoutCallback);
	void attachErrorCallBack(ErrorCallBack_t errorCallback);
	//设置等待响应超时时间，单位为ms
	void setTimeout(int timeoutMS);	
	//返回响应延时，单位为ms。
	int getDelay();	
private:
	void doRead();
	void check_deadline();
	void stop();
private:
	udp::socket _socket;
	int _localPort;
	udp::endpoint _serverEndPoint;
	char _reply[MAX_LENGTH];

	ReplayCallBack_t _replyCallBack;
	TimeoutCallBack_t _timeoutCallBack;
	ErrorCallBack_t _errorCallback;

	int _timeoutMs;
	boost::asio::deadline_timer _timerTimeout;

	bool _isStopped;
	clock_t _sendClock;
};

#endif