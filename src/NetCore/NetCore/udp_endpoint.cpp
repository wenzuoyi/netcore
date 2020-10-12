#include "include/udp_endpoint.h"
#include "udp_endpoint_impl.h"

namespace netcore {
	UDPEndPoint::UDPEndPoint() = default;

	UDPEndPoint::~UDPEndPoint() = default;

	UDPEndPointPtr UDPEndPoint::CreateInstance() {
	  return std::make_shared<UDPEndPointImpl>();
  }
}
