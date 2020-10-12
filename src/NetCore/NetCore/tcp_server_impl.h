#ifndef TCP_SERVER_IMPL_H_
#define TCP_SERVER_IMPL_H_
#include "./include/tcp_server.h"
#include <Poco/Net/StreamSocket.h>
#include "base_session_datatype.hpp"
#include "tcp_acceptor.h"
#include "base_session.hpp"

namespace netcore {
  class TCPServerImpl : public TCPServer , BaseSession<Poco::Net::StreamSocket> {
  public:
    TCPServerImpl();
    virtual ~TCPServerImpl();
  protected:
    void SetEvent(TCPServerEvent* event) override;
    int Init() override;
    void Fini() override;
    int Start(unsigned port, int thread_count) override;
    void Stop() override;
    BaseSessionInfoListPtr GetSessionList() const override;
    void SendAll(const Poco::Buffer<char>& buffer, SendResultListPtr send_result_list) override;
    void SendList(const SessionIDList& session_ids, const Poco::Buffer<char>& buffer,SendResultListPtr send_result_list) override;
    int SendOne(const std::string& id, const Poco::Buffer<char>& buffer) override;
    void OnCreate(const std::string& ida, BaseSessionHandler<Poco::Net::StreamSocket>* session) override;
    void OnRead(const std::string& id, Poco::FIFOBuffer* fifo) override;
    void OnException(const std::string& id, const std::string& message) override;
    void OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) override;
  private:
	  ServerSocketPtr server_ = nullptr;
	  TCPAcceptorHandlerPtr acceptor_ = nullptr;
	  TCPServerEvent* event_ = nullptr;
  };
}
#endif // TCP_SERVER_IMPL_H_
