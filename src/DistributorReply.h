#ifndef _DISTRIBUTOR_REPLY_H_
#define _DISTRIBUTOR_REPLY_H_
#include <string>
class DistributorReply
{
private: 
	enum 
	{
		MAX_CHALLENGE_CODE_LEN = 32
	};
public:
	int _Result;
	std::string _ChallengeCode;
	int _Interval;
	std::string _ServerIP;

};

#endif