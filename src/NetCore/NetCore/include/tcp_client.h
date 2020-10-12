#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_
#include "netcore_datatype.h"
#include "global_definition.h"
#include <Poco/Buffer.h>
#include <Poco/FIFOBuffer.h>

namespace netcore {
	class TCPClientEvent;
	class TCPClient;
	using TCPClientPtr = std::shared_ptr<TCPClient>;

  class API_EXPORT_HEADER TCPClientEvent {
  public:
	  TCPClientEvent() = default;
	  virtual ~TCPClientEvent() = default;
	  virtual void OnTCPClientConnect(TCPClient* client, BaseSessionInfoPtr peer_base_info) = 0;
	  virtual void OnTCPClientDisconnect(TCPClient* client, BaseSessionInfoPtr peer_base_info) = 0;
	  virtual void OnTCPClientException(TCPClient* client,  const std::string& exception_message) = 0;
	  virtual void OnTCPClientRead(TCPClient* client, Poco::FIFOBuffer* buffer) = 0;
  };

	class API_EXPORT_HEADER TCPClient {
	public:
		TCPClient();
		virtual ~TCPClient();
		virtual void SetEvent(TCPClientEvent* event) = 0;
		virtual int Init() = 0;
		virtual void Fini() = 0;
		virtual int Connect(const std::string& host, int port, IPAddressType ip_address_type) = 0;
		virtual void Disconnect() = 0;
		virtual BaseSessionInfoPtr GetSessionBaseInfo() const = 0;
		virtual int Send(const Poco::Buffer<char>& buffer) = 0;
		static TCPClientPtr CreateInstance();
	};
}

#endif // TCP_CLIENT_H_
