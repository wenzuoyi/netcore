#include "tcp_acceptor.h"

namespace netcore {
  TCPAcceptor::TCPAcceptor(ContextPtr<Poco::Net::StreamSocket> context, ServerSocketPtr server_socket) : Poco::Net::SocketAcceptor<TCPSessionHandler>(*server_socket), context_(context) {
	  SocketAcceptor<TCPSessionHandler>::registerAcceptor(*(context_->reactor));
  }

	TCPAcceptor::~TCPAcceptor() {
    SocketAcceptor<TCPSessionHandler>::unregisterAcceptor();
  }

	TCPSessionHandler* TCPAcceptor::createServiceHandler(Poco::Net::StreamSocket& socket) {
    auto param =std::make_shared<HandlerParam<Poco::Net::StreamSocket>>();
	  param->context = context_;
	  param->socket = &socket;
	  return new TCPSessionHandler(param);
  }
}
