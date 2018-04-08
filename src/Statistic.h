#ifndef _STATISTIC_H_
#define _STATISTIC_H_

#include <mutex>
#include <stdint.h>
class Statistic
{
public: typedef struct StatResult_s
	{
		int _lostPermillage;
		int _averageDelay;
	}StatResult;

	static const float	DELAY_WEIGHTS;
public:
	Statistic();
	~Statistic();
	void onConnectionSucceed(int delayMS);
	void onConnectionFailed();
	StatResult getCurrentStatResult();
	StatResult getTotalStatResult();
private:
	uint64_t _currentDelay;
	uint64_t _totalDelay;
	uint64_t _currentSucceeds;
	uint64_t _totalSucceeds;
	uint64_t _currentFails;
	uint64_t _totalFails;
	std::mutex m_mutex;

};
#endif
