#ifndef NETCORE_DATATYPE_H_
#define NETCORE_DATATYPE_H_
#include <string>
#include <memory>
#include <vector>
#include "Poco/ThreadPool.h"
namespace netcore {
	using ThreadPoolPtr = std::shared_ptr<Poco::ThreadPool>;
  struct BaseSessionInfo {
    std::string id;
    std::string peer_ip;
    unsigned peer_port{0};
    std::string local_ip;
    unsigned local_port{0};
  };
  using BaseSessionInfoPtr = std::shared_ptr<BaseSessionInfo>;
  using BaseSessionInfoList = std::vector<BaseSessionInfoPtr>;
  using BaseSessionInfoListPtr = std::shared_ptr<BaseSessionInfoList>;
  struct SendResultItem {
    std::string id;
    int result{0};
  };
  using SendResultItemPtr = std::shared_ptr<SendResultItem>;
  using SendResultList = std::vector<SendResultItemPtr>;
  using SendResultListPtr = std::shared_ptr<SendResultList>;
  using SessionIDList = std::vector<std::string>;
  enum class IPAddressType {
    kIPv4 = 0,
    kIPv6
  };
  struct NetworkAddress {
    std::string ip;
    int port{0};
    IPAddressType ip_address_type;
  };
  using NetworkAddressPtr = std::shared_ptr<NetworkAddress>;
}
#endif // NETCORE_DATATYPE_H_
