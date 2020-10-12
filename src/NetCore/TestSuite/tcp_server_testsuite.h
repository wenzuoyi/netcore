#ifndef TCP_SERVER_TESTSUITE_H_
#define TCP_SERVER_TESTSUITE_H_
#include "tcp_server.h"
#include <mutex>
#include <future>
namespace testsuite {
  class TCPServerTestSuite : public netcore::TCPServerEvent , std::mutex {
  public:
    TCPServerTestSuite();
    virtual ~TCPServerTestSuite();
	  void Start();
	  void Stop();
	  void AutoStart();
	  void AutoStop();
  protected:
    void OnTCPServerAccept(netcore::TCPServer*server, const std::string& id, netcore::BaseSessionInfoPtr base_session_info) override;
    void OnTCPServerRead(netcore::TCPServer*server, const std::string& id, Poco::FIFOBuffer* buffer) override;
    void OnTCPServerShutdown(netcore::TCPServer*server, const std::string& id, netcore::BaseSessionInfoPtr base_session_info) override;
    void OnTCPServerException(netcore::TCPServer*server, const std::string& id, const std::string& message) override;
  private:
	  void SyncPrintMessage(const std::string& message);
	  netcore::TCPServerPtr tcp_server_ = nullptr;
	  std::shared_ptr<std::thread> post_task_;
	  std::future<int> auto_test_;
     
	  std::atomic_bool exit_ = false;
	  std::atomic_bool exit_auto_test_ = false;
  };
}
#endif // TCP_SERVER_TESTSUITE_H_
