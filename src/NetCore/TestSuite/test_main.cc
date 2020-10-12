//#include "tcp_server_testsuite.h"
#include "udp_server_testsuite.h"

int main(int argc, const __wchar_t argv[]) {
  // testsuite::TCPServerTestSuite tcp_server_test_suite;
  // tcp_server_test_suite.AutoStart();
  // std::this_thread::sleep_for(std::chrono::hours(24));
  // tcp_server_test_suite.AutoStop();

	testsuite::UDPServerTestSuite udp_server_test_suite;
	udp_server_test_suite.AutoStart();
	std::this_thread::sleep_for(std::chrono::hours(24));
	udp_server_test_suite.AutoStop();
  return 0;
}
