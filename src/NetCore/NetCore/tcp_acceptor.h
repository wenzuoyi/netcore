#ifndef TCP_ACCEPTOR_H_
#define TCP_ACCEPTOR_H_
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/UUIDGenerator.h"
#include "base_session_datatype.hpp"
#include "tcp_session_handler.h"

namespace netcore {
	using ServerSocketPtr = std::shared_ptr<Poco::Net::ServerSocket>;
	class TCPAcceptor : public  Poco::Net::SocketAcceptor<TCPSessionHandler> {
	public:
		TCPAcceptor() = delete;
		TCPAcceptor(ContextPtr<Poco::Net::StreamSocket> context, ServerSocketPtr server_socket);
		virtual ~TCPAcceptor();
  protected:
	  TCPSessionHandler* createServiceHandler(Poco::Net::StreamSocket& socket) override;
	private:
		ContextPtr<Poco::Net::StreamSocket> context_;
	};
	using TCPAcceptorHandlerPtr = std::shared_ptr<TCPAcceptor>;
}
#endif // TCP_ACCEPTOR_H