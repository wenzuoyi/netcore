#ifndef UDP_CLIENT_TESTSUITE_H_
#define UDP_CLIENT_TESTSUITE_H_
#include "udp_endpoint.h"
#include <future>
namespace testsuite {
  class UDPClientTestSuite : public netcore::UDPEndPointEvent , std::mutex {
  public:
    UDPClientTestSuite();
    ~UDPClientTestSuite();
    void Start();
    void Stop();
    void AutoStart();
    void AutoStop();
  protected:
    void OnUDPServerCreate(netcore::UDPEndPoint* server) override;
    void OnUDPServerRead(netcore::UDPEndPoint* server, netcore::NetworkAddressPtr address,  const Poco::Buffer<char>& buffer) override;
    void OnUDPServerException(netcore::UDPEndPoint* server, const std::string& message) override;
    void OnUDPServerDestory(netcore::UDPEndPoint* server) override;
  private:
	  netcore::UDPEndPointPtr udp_end_point_{ nullptr };
	  bool exit_{ false };
    std::future<int> work_task_;
    bool auto_exit_{false};
	  std::future<int> auto_task_;
  };
}
#endif // UDP_CLIENT_TESTSUITE_H_
