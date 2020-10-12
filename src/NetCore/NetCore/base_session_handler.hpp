#ifndef BASE_SESSION_HANDLER_H_
#define BASE_SESSION_HANDLER_H_
#include <Poco/UUIDGenerator.h>
#include <Poco/NObserver.h>
#include <Poco/FIFOBuffer.h>
#include <Poco/Net/SocketNotification.h>
#include "include/netcore_errorcode.h"
#include "base_session_datatype.hpp"
#include "reactor_message_notification.h"

namespace netcore {
#define DELIVERY_MESSAGE(MESSAGE) \
      Poco::Notification::Ptr notification(new ReactorMessageNotification(MESSAGE, id_)); \
	    if (context_ != nullptr && context_->task_queue != nullptr) { \
		    context_->task_queue->enqueueNotification(notification); \
	    }

  template <typename  T>
	class BaseSessionHandler;

	template <typename  T>
	class BaseSessionHandlerEvent {
	public:
		BaseSessionHandlerEvent() = default;
		virtual ~BaseSessionHandlerEvent() = default;
		virtual void OnCreate(const std::string& id, BaseSessionHandler<T>* session) = 0;
		virtual void OnRead(const std::string& id, Poco::FIFOBuffer* fifo) = 0;
		virtual void OnRead(const Poco::Net::SocketAddress& address,  const Poco::Buffer<char>& buffer) = 0;
		virtual void OnException(const std::string& id, const std::string& message) = 0;
		virtual void OnDestroy(const std::string& id, BaseSessionHandler<T>* session) = 0;
	};

  template <typename  T>
  class BaseSessionHandler {
  public:
	  BaseSessionHandler() = default;

	  explicit BaseSessionHandler(HandlerParamPtr<T> param) {
		  poco_assert_dbg(param != nullptr);
		  static Poco::UUIDGenerator uuid_generator;
      id_ = uuid_generator.createOne().toString();
      context_ = param->context;
      socket_ = *(param->socket);
		  socket_.setBlocking(false);
		  session_event_handler_.insert(std::make_pair(MessageType::kReadEvent, &BaseSessionHandler<T>::HandleReadSocket));
		  session_event_handler_.insert(std::make_pair(MessageType::kWriteEvent, &BaseSessionHandler<T>::HandleWriteSocket));
		  session_event_handler_.insert(std::make_pair(MessageType::kErrorEvent, &BaseSessionHandler<T>::HandleSocketError));
		  session_event_handler_.insert(std::make_pair(MessageType::kShutdownEvent, &BaseSessionHandler<T>::HandleSocketShutdown));
		  if (context_ != nullptr && context_->reactor != nullptr) {
			  context_->reactor->addEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ReadableNotification>(*this, &BaseSessionHandler<T>::OnSocketReadable));
			  context_->reactor->addEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ErrorNotification>(*this, &BaseSessionHandler<T>::OnSocketError));
			  context_->reactor->addEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ShutdownNotification>(*this, &BaseSessionHandler<T>::OnSocketShutdown));
		  }
	  }

    virtual ~BaseSessionHandler() {
		  if (context_ != nullptr && context_->reactor != nullptr) {
			  context_->reactor->removeEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ErrorNotification>(*this, &BaseSessionHandler<T>::OnSocketError));
			  context_->reactor->removeEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ReadableNotification>(*this, &BaseSessionHandler<T>::OnSocketReadable));
			  context_->reactor->removeEventHandler(socket_, Poco::NObserver<BaseSessionHandler<T>, Poco::Net::ShutdownNotification>(*this, &BaseSessionHandler<T>::OnSocketShutdown));
		  }
		  session_event_handler_.clear();
	  }

    virtual int Write(const Poco::Buffer<char>& buffer) {
      throw Poco::Exception("unrealized function", ERROR_NOT_USE);
    }

    virtual int Write(const Poco::Buffer<char>& buffer, const Poco::Net::SocketAddress address) {
      throw Poco::Exception("unrealized function", ERROR_NOT_USE);
    }

    virtual void HandleEvent(MessageType message) {
      if (session_event_handler_.find(message) != session_event_handler_.end()) {
        auto handler = session_event_handler_[message];
        (this->*handler)();
      }
    }

    T* GetStreamSocket() {
      return &socket_;
    }

	  std::string GetSessionId() const {
			  return id_;
    }

  private:
	  void OnSocketReadable(const Poco::AutoPtr< Poco::Net::ReadableNotification>& pNf) {
		  DELIVERY_MESSAGE(MessageType::kReadEvent);
	  }

	  void OnSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
		  DELIVERY_MESSAGE(MessageType::kShutdownEvent);
	  }

	  void OnSocketError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf) {
		  DELIVERY_MESSAGE(MessageType::kErrorEvent);
	  }
    
  protected:
	  virtual void TerminateConnection(const std::string& id, const std::string& message) final {
		  if (context_ != nullptr && context_->event != nullptr) {
			  context_->event->OnException(id, message);
		  }
	  }

    virtual void HandleWriteSocket() = 0;
	  virtual void HandleReadSocket() = 0;
	  virtual void HandleSocketError() = 0;
	  virtual void HandleSocketShutdown() = 0;
	  typedef void (BaseSessionHandler::*SessionEventHandler)();
	  std::map<MessageType, SessionEventHandler> session_event_handler_;
	  ContextPtr<T> context_;
	  std::string id_;
	  T   socket_;
  };
}
#endif // BASE_SESSION_HANDLER_H_
