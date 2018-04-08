#include "Statistic.h"

const float Statistic::DELAY_WEIGHTS = 0.05f;

Statistic::Statistic()
:_currentDelay(0)
, _currentSucceeds(0)
, _currentFails(0)
, _totalDelay(0)
, _totalSucceeds(0)
, _totalFails(0)
{
}


Statistic::~Statistic()
{
}

void Statistic::onConnectionSucceed(int delayMS)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	_currentSucceeds++;
	_totalSucceeds++;

	_currentDelay += delayMS;
	_totalDelay += delayMS;
}

void Statistic::onConnectionFailed()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	_currentFails++;
	_totalFails++;
}

Statistic::StatResult Statistic::getCurrentStatResult()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	uint64_t currentConnections = _currentSucceeds + _currentFails;
	Statistic::StatResult result = Statistic::StatResult
	{
		currentConnections == 0 ? 0 : static_cast<int>(_currentFails * 1000 / currentConnections),
		_currentSucceeds == 0 ? 0 : static_cast<int>(_currentDelay / _currentSucceeds)
	};
	_currentFails = _currentSucceeds = _currentDelay = 0;

	return result;
}

Statistic::StatResult Statistic::getTotalStatResult()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	uint64_t totalConnections = _totalSucceeds + _totalFails;
	return Statistic::StatResult
	{
		totalConnections == 0 ? 0 : static_cast<int>(_totalFails * 1000 / totalConnections),
		_totalSucceeds == 0 ? 0 : static_cast<int>(_totalDelay / _totalSucceeds)
	};
}

