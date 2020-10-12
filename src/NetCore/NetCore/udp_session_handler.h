#ifndef UDP_SESSION_HANDLER_H_
#define UDP_SESSION_HANDLER_H_
#include <utility>
#include <deque>
#include <vector>
#include <Poco/Net/DatagramSocket.h>
#include "base_session_handler.hpp"
namespace netcore {
  class UDPSessionHandler : public BaseSessionHandler<Poco::Net::DatagramSocket> {
  public:
	  UDPSessionHandler() = delete;
    explicit UDPSessionHandler(HandlerParamPtr<Poco::Net::DatagramSocket> param);
    virtual ~UDPSessionHandler();
    int Write(const Poco::Buffer<char>& buffer, const Poco::Net::SocketAddress address) override;
  protected:
    void HandleWriteSocket() override;
    void HandleReadSocket() override;
    void HandleSocketError() override;
    void HandleSocketShutdown() override;
  private:
	  using Item = std::pair<Poco::Buffer<char>, Poco::Net::SocketAddress>;
	  std::deque<Item> post_queue_;
	  std::vector<char> receive_buffer_;
  };
}
#endif // UDP_SESSION_HANDLER_H_
