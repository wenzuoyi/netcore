#include "udp_session_handler.h"
namespace netcore {
  #define MAX_BUFFER_SIZE 65535

  UDPSessionHandler::UDPSessionHandler(HandlerParamPtr<Poco::Net::DatagramSocket> param) : BaseSessionHandler<Poco::Net::DatagramSocket>(param), receive_buffer_(MAX_BUFFER_SIZE, 0){
    if (context_ != nullptr && context_->event != nullptr) {
      context_->event->OnCreate(id_, this);
    }
  }

	UDPSessionHandler::~UDPSessionHandler() {
		if (context_ != nullptr && context_->event != nullptr) {
			context_->event->OnDestroy(id_, this);
		}
  }

  int UDPSessionHandler::Write(const Poco::Buffer<char>& buffer, const Poco::Net::SocketAddress address) {
	  auto item = std::make_pair(buffer, address);
	  {
		  Poco::Mutex::ScopedLock lock(mutex_);
		  post_queue_.emplace_back(item);
	  }
	  DELIVERY_MESSAGE(MessageType::kWriteEvent)
    return OK;
  }

  void UDPSessionHandler::HandleWriteSocket() {
	  Poco::Mutex::ScopedLock lock(mutex_);
    while (!post_queue_.empty()) {
      auto item = post_queue_.front();
      post_queue_.pop_front();
      auto buffer = item.first;
      auto address = item.second;
      try {
        socket_.sendTo(buffer.begin(), static_cast<int>(buffer.size()), address);
      } catch (Poco::IOException& io_exception) {
        if (io_exception.code() != POCO_EINTR && io_exception.code() != POCO_EAGAIN) {
    			DELIVERY_MESSAGE(MessageType::kShutdownEvent)
        }
        DELIVERY_MESSAGE(MessageType::kWriteEvent)
      } catch (...) {
      }
    }
  }

  void UDPSessionHandler::HandleReadSocket() {
	  try {
		  Poco::Net::SocketAddress address;
      auto receive_bytes = socket_.receiveFrom(&receive_buffer_[0], static_cast<int>(receive_buffer_.size()), address);
      if (receive_bytes < 0) {
        DELIVERY_MESSAGE(MessageType::kReadEvent)
      } else if (context_ != nullptr && context_->event != nullptr) {
        Poco::Buffer<char> buffer(reinterpret_cast<const char*>(&receive_buffer_[0]), receive_bytes);
			  context_->event->OnRead(address, buffer);
      }
	  } catch (...) {
	  }
  }

  void UDPSessionHandler::HandleSocketError() {
	  TerminateConnection(id_, "socket error");
  }

  void UDPSessionHandler::HandleSocketShutdown() {
	  TerminateConnection(id_, "handle shutdown, Pear has been shutdown gracefully!");
  }
}
