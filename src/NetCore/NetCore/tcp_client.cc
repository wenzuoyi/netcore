#include "./include/tcp_client.h"
#include "tcp_client_impl.h"
namespace netcore {
	TCPClient::TCPClient() = default;

	TCPClient::~TCPClient() = default;

  TCPClientPtr TCPClient::CreateInstance() {
	  return std::make_shared<TCPClientImpl>();
  }
}
