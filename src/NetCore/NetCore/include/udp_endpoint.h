#ifndef UDP_ENDPOINT_H_
#define UDP_ENDPOINT_H_
#include "global_definition.h"
#include "netcore_datatype.h"
#include <Poco/Buffer.h>

namespace netcore {
	class UDPEndPoint;
	using UDPEndPointPtr = std::shared_ptr<UDPEndPoint>;

  class API_EXPORT_HEADER UDPEndPointEvent {
  public:
	  UDPEndPointEvent() = default;
    virtual ~UDPEndPointEvent() = default;
    virtual void OnUDPServerCreate(UDPEndPoint* server) = 0;
    virtual void OnUDPServerRead(UDPEndPoint* server, NetworkAddressPtr address, const Poco::Buffer<char>& buffer) = 0;
	  virtual void OnUDPServerException(UDPEndPoint* server, const std::string& message) = 0;
	  virtual void OnUDPServerDestory(UDPEndPoint* server) = 0;
  };

  class API_EXPORT_HEADER UDPEndPoint {
  public:
    UDPEndPoint();
    virtual ~UDPEndPoint();
    virtual void SetEvent(UDPEndPointEvent* event) = 0;
    virtual int Init() = 0;
    virtual void Fini() = 0;
    virtual int Start(unsigned port, IPAddressType ip_address_type, bool broadcast_flag) = 0;
    virtual void Stop() = 0;
    virtual void Send(const  Poco::Buffer<char>& buffer, NetworkAddressPtr address) = 0;
    static UDPEndPointPtr CreateInstance();
  };
}
#endif // UDP_ENDPOINT_H_
