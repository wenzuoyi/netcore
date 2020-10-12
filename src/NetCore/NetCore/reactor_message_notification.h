#ifndef REACTOR_MESSAGE_NOTIFICATION_H_
#define REACTOR_MESSAGE_NOTIFICATION_H_
#include <Poco/Notification.h>
namespace netcore {

  enum class MessageType {
    kReadEvent = 0,
    kWriteEvent = 1,
    kErrorEvent = 2,
    kShutdownEvent = 3
  };

  class ReactorMessageNotification : public Poco::Notification {
  public:
    ReactorMessageNotification() = delete;
    explicit ReactorMessageNotification(MessageType message_type, const std::string& session_id);
    virtual ~ReactorMessageNotification();
    MessageType GetMessageType() const;
    void SetMessageType(MessageType message_type);
    std::string GetSessionID() const;
    void SetSessionID(const std::string& session_id);
    bool IsWritable() const;
  private:
    MessageType message_type_;
    std::string session_id_;
    bool is_write_{false};
  };
}
#endif // REACTOR_MESSAGE_NOTIFICATION_H_
