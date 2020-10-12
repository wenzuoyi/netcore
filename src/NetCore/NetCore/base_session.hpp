#ifndef BASE_CONNECTION_OBJECT_HPP_
#define BASE_CONNECTION_OBJECT_HPP_
#include "./include/netcore_datatype.h"
#include <functional>
#include "session_list.hpp"
#include "base_session_handler.hpp"

namespace netcore {
  template <typename T>
  class BaseSession : public Poco::Runnable, public BaseSessionHandlerEvent<T> {
  public:
    BaseSession() : thread_pool_(1) {
    }

    virtual ~BaseSession() {
    }

  protected:
    virtual int Init() {
      context_ = std::make_shared<Context<T>>();
      if (context_ == nullptr) {
        return ERROR_ALLOC_MEMORY;
      }
      context_->reactor = std::make_shared<Poco::Net::SocketReactor>();
      if (context_->reactor == nullptr) {
        return ERROR_ALLOC_MEMORY;
      }
      context_->task_queue = std::make_shared<Poco::NotificationQueue>();
      if (context_->task_queue == nullptr) {
        return ERROR_ALLOC_MEMORY;
      }
      context_->event = this;
      reactor_thread_.start(*(context_->reactor));
      return OK;
    }

    virtual void Fini() {
      context_->reactor->stop();
      context_->reactor->wakeUp();
      reactor_thread_.join();
      context_->task_queue.reset();
      context_->reactor.reset();
      context_.reset();
    }

    virtual int Start(unsigned thread_count = 1) {
      if (thread_count > thread_pool_.capacity() && thread_count == 0) {
        return ERROR_PARAM;
      }
      exit_ = false;
      for (auto i = 0U; i < thread_count; ++i) {
        thread_pool_.start(*this);
      }
      return OK;
    }

    virtual void Stop() {
      if (context_ == nullptr || context_->reactor == nullptr || context_->task_queue == nullptr) {
        return;
      }
      session_list_.CloseSession();
      exit_ = true;
      context_->task_queue->wakeUpAll();
      thread_pool_.stopAll();
    }

    static BaseSessionInfoPtr GetConnectionBaseInfo(BaseSessionHandler<T>* session) {
      if (session == nullptr) {
        return nullptr;
      }
      auto peer_address = session->GetStreamSocket()->peerAddress();
      auto local_address = session->GetStreamSocket()->address();
      auto base_session_info = std::make_shared<BaseSessionInfo>();
      base_session_info->id = session->GetSessionId();
      base_session_info->peer_ip = peer_address.host().toString();
      base_session_info->peer_port = peer_address.port();
      base_session_info->local_ip = local_address.host().toString();
      base_session_info->local_port = local_address.port();
      return base_session_info;
    }

    void OnCreate(const std::string& id, BaseSessionHandler<T>* session) override {
      session_list_.Append(id, session);
    }

    void OnRead(const std::string& id, Poco::FIFOBuffer* fifo) override {
      throw Poco::Exception("Not implement!");
    }

    void OnRead(const Poco::Net::SocketAddress& address, const Poco::Buffer<char>& buffer) override {
      throw Poco::Exception("Not implement!");
    }

    void OnException(const std::string& id, const std::string& message) override {
      auto result = session_list_.Remove(id);
      poco_assert_msg_dbg(result, "release session resouce lost!");
    }

    void OnDestroy(const std::string& id, BaseSessionHandler<T>* session) override {
      throw Poco::Exception("Not implement!");
    }

    virtual void GetSession(const std::string& session_id, bool write,  std::function<void(BaseSessionHandler<T>*)>&& handler) final {
		  session_list_.GetSession(session_id, write, std::move(handler));
    }

	  virtual void GetSession(const SessionIDList& session_ids, bool write, std::function<void(const std::string&, BaseSessionHandler<T>*)>&& handler) const final {
		  session_list_.GetSession(session_ids, write, std::move(handler));
	  }

	  virtual void GetSession(bool write, std::function<void(const std::string&, BaseSessionHandler<T>*)>&& handler) const final {
		  session_list_.GetSession(write, std::move(handler));
    }

    ContextPtr<T> context_ = nullptr;
  private:
    void run() override {
      while (!exit_) {
        Poco::Notification::Ptr message = context_->task_queue->waitDequeueNotification();
        if (exit_ || message.isNull()) {
          break;
        }
        auto reactor_message_notification = message.cast<ReactorMessageNotification>();
        auto message_type = reactor_message_notification->GetMessageType();
        auto session_id = reactor_message_notification->GetSessionID();
        auto is_writable = reactor_message_notification->IsWritable();
		     GetSession(session_id, is_writable, [message_type](BaseSessionHandler<T>* session) {
          session->HandleEvent(message_type);
        });
      }
    }

    SessionList<T> session_list_;
    Poco::Thread reactor_thread_;
    Poco::ThreadPool thread_pool_;
    bool exit_ = false;
  };
}
#endif // BASE_CONNECTION_OBJECT_HPP_
