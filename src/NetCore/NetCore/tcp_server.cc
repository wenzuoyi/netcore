#include "./include/tcp_server.h"
#include "tcp_server_impl.h"
namespace netcore {
	TCPServer::TCPServer() = default;
	
	TCPServer::~TCPServer() = default;

  TCPServerPtr TCPServer::CreateInstance() {
		return std::make_shared<TCPServerImpl>();
	}
}