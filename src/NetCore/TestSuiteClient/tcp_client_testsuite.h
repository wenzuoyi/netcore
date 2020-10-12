#ifndef TCP_CLIENT_TESTSUITE_H_
#define TCP_CLIENT_TESTSUITE_H_
#include <future>
#include <mutex>
#include "tcp_client.h"
namespace testsuite {
	class TCPClientTestSuite : public netcore::TCPClientEvent , std::mutex {
	public:
		TCPClientTestSuite();
		virtual ~TCPClientTestSuite();
		void Start();
		void Stop();
		void AutoStart();
		void AutoStop();
  protected:
    void OnTCPClientConnect(netcore::TCPClient* client, netcore::BaseSessionInfoPtr peer_base_info) override;
    void OnTCPClientDisconnect(netcore::TCPClient* client, netcore::BaseSessionInfoPtr peer_base_info) override;
    void OnTCPClientException(netcore::TCPClient* client, const std::string& exception_message) override;
    void OnTCPClientRead(netcore::TCPClient* client, Poco::FIFOBuffer* buffer) override;
  private:
    void SyncPrintMessage(const std::string& message);
		netcore::TCPClientPtr client_ = nullptr;
		std::future<void> post_task_;
		std::future<void> auto_task_;
		bool exit_ = true;
		bool auto_test_ = false;
	};
}
#endif // TCP_CLIENT_TESTSUITE_H_
