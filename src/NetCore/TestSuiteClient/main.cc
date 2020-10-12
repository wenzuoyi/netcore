//#include "tcp_client_testsuite.h"
#include "udp_client_testsuite.h"
int main(int argc, wchar_t* argv[]) {
	//testsuite::TCPClientTestSuite client_test_suite;
	//client_test_suite.AutoStart();
	//std::this_thread::sleep_for(std::chrono::hours(24));
	//client_test_suite.AutoStop();

	testsuite::UDPClientTestSuite udp_client_test;
	udp_client_test.AutoStart();
	std::this_thread::sleep_for(std::chrono::hours(24));
	udp_client_test.AutoStop();
	return 0;
}