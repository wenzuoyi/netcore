#ifndef UDP_ENDPOINT_IMPL_H_
#define UDP_ENDPOINT_IMPL_H_
#include <Poco/Net/DatagramSocket.h>
#include "include/udp_endpoint.h"
#include "base_session.hpp"

namespace netcore {
  class UDPEndPointImpl : public UDPEndPoint , BaseSession<Poco::Net::DatagramSocket>{
  public:
    UDPEndPointImpl();
    virtual ~UDPEndPointImpl();
  protected:
    void SetEvent(UDPEndPointEvent* event) override;
    int Init() override;
    void Fini() override;
    int Start(unsigned port, IPAddressType ip_address_type, bool broadcast_flag) override;
    void Stop() override;
    void Send(const Poco::Buffer<char>& buffer, NetworkAddressPtr address) override;
    void OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) override;
    void OnRead(const Poco::Net::SocketAddress& address, const Poco::Buffer<char>& buffer) override;
    void OnException(const std::string& id, const std::string& message) override;
    void OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) override;
  private:
	  UDPEndPointEvent* event_{ nullptr };
  };
}
#endif // UDP_ENDPOINT_IMPL_H_
