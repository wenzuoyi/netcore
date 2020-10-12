#ifndef TCP_CLIENT_IMPL_H_
#define TCP_CLIENT_IMPL_H_
#include "./include/tcp_client.h"
#include <Poco/Net/StreamSocket.h>
#include "base_session.hpp"
namespace netcore {
  class TCPClientImpl : public TCPClient, BaseSession<Poco::Net::StreamSocket> {
  public:
    TCPClientImpl();
    virtual ~TCPClientImpl();
  protected:
    void SetEvent(TCPClientEvent* event) override;
    int Init() override;
    void Fini() override;
    int Connect(const std::string& host, int port, IPAddressType ip_address_type) override;
    void Disconnect() override;
    BaseSessionInfoPtr GetSessionBaseInfo() const override;
    int Send(const Poco::Buffer<char>& buffer) override;
    void OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) override;
    void OnRead(const std::string& id, Poco::FIFOBuffer* fifo) override;
    void OnException(const std::string& id, const std::string& message) override;
    void OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) override;
  private:
	  TCPClientEvent* event_ = nullptr;
	  BaseSessionInfoPtr session_info_{ nullptr };
  };
}
#endif // TCP_CLIENT_IMPL_H_
