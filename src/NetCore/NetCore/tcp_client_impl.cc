#include <Poco/UUIDGenerator.h>
#include <Poco/Event.h>
#include "include/netcore_errorcode.h"
#include "tcp_client_impl.h"
#include "tcp_session_handler.h"

namespace netcore {
	TCPClientImpl::TCPClientImpl() = default;

	TCPClientImpl::~TCPClientImpl() = default;

  void TCPClientImpl::SetEvent(TCPClientEvent* event) {
	  event_ = event;
  }

  int TCPClientImpl::Init() {
	  return BaseSession::Init();
  }

  void TCPClientImpl::Fini() {
	  return BaseSession::Fini();
  }

  int TCPClientImpl::Connect(const std::string& host, int port, IPAddressType ip_address_type) {
	  Poco::Net::StreamSocket socket;
    std::shared_ptr<Poco::Net::SocketAddress> address{nullptr};
    if (ip_address_type == IPAddressType::kIPv4) {
      address = std::make_shared<Poco::Net::SocketAddress>(Poco::Net::AddressFamily::IPv4, host, port);
    } else {
      address = std::make_shared<Poco::Net::SocketAddress>(Poco::Net::AddressFamily::IPv6, host, port);
    }
    Poco::Timespan timespan(60, 0);
    try {
      socket.connect(*address, timespan);
    } catch (Poco::TimeoutException& /*timeout_exception*/) {
      return ERROR_SYSTEM_TIMEOUT;
    } catch (Poco::Exception& /*exception*/) {
      return ERROR_CONNECT_EXCEPTION;
    }
    auto result = BaseSession<Poco::Net::StreamSocket>::Start();
    if (result != OK) {
      return result;
    }
    auto param = std::make_shared<HandlerParam<Poco::Net::StreamSocket>>();
    static Poco::UUIDGenerator uuid_generator;
    param->socket = &socket;
    param->context = context_;
	  auto handler = new TCPSessionHandler(param);
    if (handler == nullptr) {
      return ERROR_ALLOC_MEMORY;
    }
    session_info_ = GetConnectionBaseInfo(handler);
    return OK;
  }

  void TCPClientImpl::Disconnect() {
    BaseSession<Poco::Net::StreamSocket>::Stop();
  }

  BaseSessionInfoPtr TCPClientImpl::GetSessionBaseInfo() const {
    return session_info_;
  }

  int TCPClientImpl::Send(const Poco::Buffer<char>& buffer) {
    BaseSession<Poco::Net::StreamSocket>::GetSession(false, [buffer](const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
        session->Write(buffer);
      });
	  return OK;
  }

  void TCPClientImpl::OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
	  BaseSession<Poco::Net::StreamSocket>::OnCreate(id, session);
    if (event_ != nullptr && session != nullptr) {
      auto session_info = GetConnectionBaseInfo(session);
      event_->OnTCPClientConnect(this, session_info);
    }
  }

  void TCPClientImpl::OnRead(const std::string& id, Poco::FIFOBuffer* fifo) {
    if (event_ != nullptr && fifo != nullptr) {
      event_->OnTCPClientRead(this, fifo);
    }
  }

  void TCPClientImpl::OnException(const std::string& id, const std::string& message) {
    if (event_ != nullptr) {
      event_->OnTCPClientException(this, message);
    }
    BaseSession<Poco::Net::StreamSocket>::OnException(id, message);
  }

  void TCPClientImpl::OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
    if (event_ != nullptr && session != nullptr) {
      event_->OnTCPClientDisconnect(this, nullptr);
    }
  }
}
