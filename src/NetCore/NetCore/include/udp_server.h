#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_
#include "global_definition.h"
#include "netcore_datatype.h"
#include <Poco/Buffer.h>



namespace netcore {
	class UDPServer;
	using UDPServerPtr = std::shared_ptr<UDPServer>;

  class API_EXPORT_HEADER UDPServerEvent {
  public:
	  UDPServerEvent() = default;
    virtual ~UDPServerEvent() = default;
    virtual void OnUDPServerCreate(UDPServer* server) = 0;
    virtual void OnUDPServerRead(UDPServer* server, NetworkAddressPtr address, const Poco::Buffer<char>& buffer) = 0;
	  virtual void OnUDPServerException(UDPServer* server, const std::string& message) = 0;
	  virtual void OnUDPServerDestory(UDPServer* server) = 0;
  };

  class API_EXPORT_HEADER UDPServer {
  public:
    UDPServer();
    virtual ~UDPServer();
    virtual void SetEvent(UDPServerEvent* event) = 0;
    virtual int Init(ThreadPoolPtr thread_pool) = 0;
    virtual void Fini() = 0;
    virtual int Start(unsigned port, IPAddressType ip_address_type, bool broadcast_flag) = 0;
    virtual void Stop() = 0;
    virtual void Send(const  Poco::Buffer<char>& buffer, NetworkAddressPtr address) = 0;
    static UDPServerPtr CreateInstance();
  };
}
#endif // UDP_SERVER_H_
