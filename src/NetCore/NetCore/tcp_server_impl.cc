#include "tcp_server_impl.h"
#include "./include/netcore_errorcode.h"

namespace netcore {
  TCPServerImpl::TCPServerImpl() = default;

  TCPServerImpl::~TCPServerImpl() = default;

  void TCPServerImpl::SetEvent(TCPServerEvent* event) {
	  event_ = event;
  }

  int TCPServerImpl::Init() {
	  return BaseSession<Poco::Net::StreamSocket>::Init();
  }

  void TCPServerImpl::Fini() {
	  return BaseSession<Poco::Net::StreamSocket>::Fini();
  }

  int TCPServerImpl::Start(unsigned port, int thread_count) {
    auto result = BaseSession<Poco::Net::StreamSocket>::Start(thread_count);
    if (result != OK) {
      return result;
    }
    server_ = std::make_shared<Poco::Net::ServerSocket>(port);
    if (server_ == nullptr) {
      return ERROR_ALLOC_MEMORY;
    }
    acceptor_ = std::make_shared<TCPAcceptor>(context_, server_);
    if (acceptor_ == nullptr) {
      return ERROR_ALLOC_MEMORY;
    }
    return OK;
  }

  void TCPServerImpl::Stop() {
    acceptor_.reset();
    server_.reset();
    BaseSession<Poco::Net::StreamSocket>::Stop();
  }

  BaseSessionInfoListPtr TCPServerImpl::GetSessionList() const {
    auto session_list = std::make_shared<BaseSessionInfoList>();
    GetSession(false, [session_list](const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
      if (session != nullptr) {
        auto base_session_info = GetConnectionBaseInfo(session);
        session_list->emplace_back(base_session_info);
      }
    });
    return session_list;
  }

  void TCPServerImpl::SendAll(const Poco::Buffer<char>& buffer, SendResultListPtr send_result_list) {
    GetSession(false, [buffer, send_result_list](const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
      auto item = std::make_shared<SendResultItem>();
      item->id = id;
      if (send_result_list != nullptr) {
        item->result = session->Write(buffer);
      } else {
        item->result = ERROR_NULL_POINTER;
      }
      send_result_list->emplace_back(item);
	  });
  }

  void TCPServerImpl::SendList(const SessionIDList& session_ids, const Poco::Buffer<char>& buffer,   SendResultListPtr send_result_list) {
	  GetSession(session_ids, false,  [buffer, send_result_list](const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
			auto item = std::make_shared<SendResultItem>();
			item->id = id;
		  if (send_result_list != nullptr) {
			  item->result = session->Write(buffer);
		  } else {
			  item->result = ERROR_SESSION_NOT_EXIST;
		  }
		  send_result_list->emplace_back(item);
	  });
  }

  int TCPServerImpl::SendOne(const std::string& id, const Poco::Buffer<char>& buffer) {
    auto result = OK;
	  GetSession(id, false, [buffer, &result](BaseSessionHandler<Poco::Net::StreamSocket>* session) {
		  result = session->Write(buffer);
	  });
	  return result;
  }

  void TCPServerImpl::OnCreate(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
    BaseSession<Poco::Net::StreamSocket>::OnCreate(id, session);
    if (event_ != nullptr && session != nullptr) {
      auto base_session_info = GetConnectionBaseInfo(session);
      event_->OnTCPServerAccept(this, id, base_session_info);
    }
  }

  void TCPServerImpl::OnRead(const std::string& id, Poco::FIFOBuffer* fifo) {
    if (event_ != nullptr) {
      event_->OnTCPServerRead(this, id, fifo);
    }
  }

  void TCPServerImpl::OnException(const std::string& id, const std::string& message) {
    if (event_ != nullptr) {
      event_->OnTCPServerException(this, id, message);
    }
    BaseSession<Poco::Net::StreamSocket>::OnException(id, message);
  }

  void TCPServerImpl::OnDestroy(const std::string& id, BaseSessionHandler<Poco::Net::StreamSocket>* session) {
    if (event_ != nullptr) {
		  auto base_session_info = GetConnectionBaseInfo(session);
		  event_->OnTCPServerShutdown(this, id, base_session_info);
    }
  }
}
