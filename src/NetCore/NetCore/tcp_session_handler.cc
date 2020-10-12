#include "tcp_session_handler.h"
#include "Poco/Exception.h"
namespace netcore {
  #define DEFAULT_FIFO_SIZE  65536

  TCPSessionHandler::TCPSessionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : read_fifo_(DEFAULT_FIFO_SIZE, false)
                                                                                        , write_fifo_(DEFAULT_FIFO_SIZE, false) {
    poco_assert_msg_dbg(false, "This constructor has been deprecated!");
  }

  TCPSessionHandler::TCPSessionHandler(HandlerParamPtr<Poco::Net::StreamSocket> param) : BaseSessionHandler(param)
                                                                                        , read_fifo_(DEFAULT_FIFO_SIZE, false)
                                                                                        , write_fifo_( DEFAULT_FIFO_SIZE, false) {
	  if (context_ != nullptr && context_->event != nullptr) {
		  context_->event->OnCreate(id_, this);
	  }
  }

  TCPSessionHandler::~TCPSessionHandler() {
	  if (context_ != nullptr && context_->event != nullptr) {
		  context_->event->OnDestroy(id_, this);
	  }
  }

  int TCPSessionHandler::Write(const Poco::Buffer<char>& buffer) {
    if (buffer.empty()) {
      return ERROR_EMPTY_BUFFER;
    }
    if (!write_fifo_.isWritable()) {
      return ERROR_NOT_READABLE;
    }
    write_fifo_.write(buffer);
    DELIVERY_MESSAGE(MessageType::kWriteEvent);
    return OK;
  }

  void TCPSessionHandler::HandleWriteSocket() {
    try {
      socket_.sendBytes(write_fifo_);
    } catch (Poco::IOException& io_exception) {
      if (io_exception.code() != POCO_EINTR && io_exception.code() != POCO_EAGAIN) {
        DELIVERY_MESSAGE(MessageType::kShutdownEvent);
        return;
      }
      DELIVERY_MESSAGE(MessageType::kWriteEvent);
    } catch (Poco::Exception& /*exception*/) {
      DELIVERY_MESSAGE(MessageType::kShutdownEvent);
    }
  }

  void TCPSessionHandler::HandleReadSocket() {
    try {
      if (0 > socket_.receiveBytes(read_fifo_)) {
		    DELIVERY_MESSAGE(MessageType::kReadEvent);
      } else if (context_ != nullptr && context_->event != nullptr) {
		    context_->event->OnRead(id_, &read_fifo_);
      }
    } catch (Poco::Exception& /*exception*/) {
      DELIVERY_MESSAGE(MessageType::kShutdownEvent);
    }
  }

  void TCPSessionHandler::HandleSocketError() {
	  TerminateConnection(id_, "Error detected while reactor working!");
  }

  void TCPSessionHandler::HandleSocketShutdown() {
	  TerminateConnection(id_, "handle shutdown, Pear has been shutdown gracefully!");
  }
}
