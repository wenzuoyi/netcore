#ifndef UDP_SERVER_IMPL_H_
#define UDP_SERVER_IMPL_H_
#include "./include/udp_server.h"
#include <Poco/Mutex.h>
#include <Poco/Net/DatagramSocket.h>
#include "base_session.hpp"
#include "udp_session_handler.h"

namespace netcore {
  class UDPServerImpl : public UDPServer , BaseSession<Poco::Net::DatagramSocket>{
  public:
    UDPServerImpl();
    virtual ~UDPServerImpl();
  protected:
    void SetEvent(UDPServerEvent* event) override;
    int Init(ThreadPoolPtr thread_pool) override;
    void Fini() override;
    int Start(unsigned port, IPAddressType ip_address_type, bool broadcast_flag) override;
    void Stop() override;
    void Send(const Poco::Buffer<char>& buffer, NetworkAddressPtr address) override;
    void OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) override;
    void OnRead(const Poco::Net::SocketAddress& address, const Poco::Buffer<char>& buffer) override;
    void OnException(const std::string& id, const std::string& message) override;
    void OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) override;
  private:
	  UDPServerEvent* event_{ nullptr };
  };
}
#endif // UDP_SERVER_IMPL_H_
