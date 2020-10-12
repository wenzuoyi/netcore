#ifndef TCP_SESSION_HANDLER_H_
#define TCP_SESSION_HANDLER_H_
#include <Poco/Net/StreamSocket.h>
#include "base_session_handler.hpp"

namespace netcore {
  class TCPSessionHandler : public BaseSessionHandler<Poco::Net::StreamSocket> {
  public:
	  TCPSessionHandler() = delete;
	  TCPSessionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);
    explicit TCPSessionHandler(HandlerParamPtr<Poco::Net::StreamSocket> param);
    virtual ~TCPSessionHandler();
  protected:
    int Write(const Poco::Buffer<char>& buffer) override;
	  void HandleWriteSocket() override;
	  void HandleReadSocket() override;
    void HandleSocketError() override;
    void HandleSocketShutdown() override;
  private:
	  Poco::FIFOBuffer read_fifo_;
	  Poco::FIFOBuffer write_fifo_;
  };
}
#endif // TCP_SESSION_HANDLER_H_
