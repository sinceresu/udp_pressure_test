# UDP pressure test softward based on boost asio and thread pool.

##Description
	The program utilizes thread pool to simulate multiple test machines that send udp packets to target device simutaneously. The sending speed and  protocal content can be configured.
	
##How To Compile
	The Programe is in dependency of boost, so you must download and compile boost. Then you set enviorment variable "BOOST_PATH" to your actual location of boost. The program is compiled with vs2015.