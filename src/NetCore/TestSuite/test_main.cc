#include "Poco/Thread.h"
#include "tcp_server_testsuite.h"

int main(int argc, const __wchar_t argv[]) {
  testsuite::TCPServerTestSuite tcp_server_test_suite;
  tcp_server_test_suite.AutoStart();
  //auto time = 0U;
  //while (time < 60 * 60 * 2) {
  //  Poco::Thread::current()->sleep(1000);
  //  time++;
  //}
  std::this_thread::sleep_for(std::chrono::hours(24));
  tcp_server_test_suite.AutoStop();
  return 0;
}
