#include "reactor_message_notification.h"

namespace netcore {
  ReactorMessageNotification::ReactorMessageNotification(MessageType message_type, const std::string& session_id) {
	  message_type_ = message_type;
	  session_id_ = session_id;
	  is_write_ = (message_type_ == MessageType::kShutdownEvent || message_type_ == MessageType::kErrorEvent);
  }

	ReactorMessageNotification::~ReactorMessageNotification() = default;

  MessageType ReactorMessageNotification::GetMessageType() const {
    return message_type_;
  }

  void ReactorMessageNotification::SetMessageType(MessageType message_type) {
	  message_type_ = message_type;
  }

  std::string ReactorMessageNotification::GetSessionID() const {
    return session_id_;
  }

  void ReactorMessageNotification::SetSessionID(const std::string& session_id) {
    session_id_ = session_id;
  }

  bool ReactorMessageNotification::IsWritable() const {
	  return is_write_;
  }
}


