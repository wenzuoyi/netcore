#include "udp_server_impl.h"
#include "udp_session_handler.h"
namespace netcore {

	UDPServerImpl::UDPServerImpl() = default;

	UDPServerImpl::~UDPServerImpl() = default;

  void UDPServerImpl::SetEvent(UDPServerEvent* event) {
    event_ = event;
  }

  int UDPServerImpl::Init(ThreadPoolPtr thread_pool) {
	  return BaseSession<Poco::Net::DatagramSocket>::Init();
  }

  void UDPServerImpl::Fini() {
    return BaseSession<Poco::Net::DatagramSocket>::Fini();
  }

  int UDPServerImpl::Start(unsigned port, IPAddressType ip_address_type, bool broadcast_flag) {
    auto result = BaseSession<Poco::Net::DatagramSocket>::Start();
    if (result != OK) {
      return result;
    }
    Poco::Net::DatagramSocket socket;
    std::shared_ptr<Poco::Net::SocketAddress> socket_address{nullptr};
    if (ip_address_type == IPAddressType::kIPv4) {
      socket_address = std::make_shared<Poco::Net::SocketAddress>(Poco::Net::AddressFamily::IPv4, port);
    } else {
      socket_address = std::make_shared<Poco::Net::SocketAddress>(Poco::Net::AddressFamily::IPv6, port);
    }
    socket.bind(*socket_address, true, true);
    socket.setBroadcast(broadcast_flag);
    auto param = std::make_shared<HandlerParam<Poco::Net::DatagramSocket>>();
    if (param == nullptr) {
      return ERROR_ALLOC_MEMORY;
    }
    param->socket = &socket;
    param->context = context_;
    auto handler = new UDPSessionHandler(param);
    if (handler == nullptr) {
      return ERROR_ALLOC_MEMORY;
    }
    return OK;
  }

  void UDPServerImpl::Stop() {
	  BaseSession<Poco::Net::DatagramSocket>::Stop();
  }

  void UDPServerImpl::Send(const Poco::Buffer<char>& buffer, NetworkAddressPtr address) {
    GetSession(false, [buffer, address](const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) {
		  Poco::Net::IPAddress ip_address(address->ip, (address->ip_address_type == IPAddressType::kIPv4 ? Poco::Net::AddressFamily::IPv4 : Poco::Net::AddressFamily::IPv6));
		  Poco::Net::SocketAddress socket_address(ip_address, address->port);
		  session->Write(buffer, socket_address);
    });
  }

  void UDPServerImpl::OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) {
	  BaseSession<Poco::Net::DatagramSocket>::OnCreate(id, session);
    if (event_ != nullptr) {
      event_->OnUDPServerCreate(this);
    }
  }

  void UDPServerImpl::OnRead(const Poco::Net::SocketAddress& address, const Poco::Buffer<char>& buffer) {
    if (event_ != nullptr) {
      auto network_address = std::make_shared<NetworkAddress>();
      network_address->ip = address.host().toString();
      network_address->ip_address_type = (address.family() == Poco::Net::AddressFamily::IPv4 ? IPAddressType::kIPv4 :  IPAddressType::kIPv6);
      network_address->port = address.port();
      event_->OnUDPServerRead(this, network_address, buffer);
    }
  }

  void UDPServerImpl::OnException(const std::string& id, const std::string& message) {
    if (event_ != nullptr) {
      event_->OnUDPServerException(this, message);
    }
    BaseSession<Poco::Net::DatagramSocket>::OnException(id, message);
  }

  void UDPServerImpl::OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::DatagramSocket>* session) {
    if (event_ != nullptr) {
      event_->OnUDPServerDestory(this);
    }
  }
}
