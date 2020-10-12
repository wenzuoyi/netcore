#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_
#include "netcore_datatype.h"
#include "global_definition.h"
#include <Poco/FIFOBuffer.h>
#include <Poco/ThreadPool.h>
namespace netcore {
	class TCPServer;
	using TCPServerPtr = std::shared_ptr<TCPServer>;

  class API_EXPORT_HEADER TCPServerEvent {
  public:
	  TCPServerEvent() = default;
	  virtual ~TCPServerEvent() = default;
	  virtual void OnTCPServerAccept(TCPServer* server, const std::string& id, BaseSessionInfoPtr base_session_info) = 0;
	  virtual void OnTCPServerRead(TCPServer* server, const std::string& id, Poco::FIFOBuffer* buffer) = 0;
	  virtual void OnTCPServerShutdown(TCPServer* server, const std::string& id, BaseSessionInfoPtr base_session_info) = 0;
	  virtual void OnTCPServerException(TCPServer* server, const std::string& id, const std::string& message) = 0;
  };

  class API_EXPORT_HEADER TCPServer {
	public:
		TCPServer();
		virtual ~TCPServer();
		static TCPServerPtr CreateInstance();
		virtual void SetEvent(TCPServerEvent* event) = 0;
		virtual int Init() = 0;
		virtual void Fini() = 0;
		virtual int Start(unsigned port, int thread_count) = 0;
		virtual void Stop() = 0;
		virtual BaseSessionInfoListPtr GetSessionList() const = 0;
		virtual void SendAll(const Poco::Buffer<char>& buffer, SendResultListPtr send_result_list) = 0;
		virtual void SendList(const SessionIDList& session_ids, const Poco::Buffer<char>& buffer, SendResultListPtr send_result_list) = 0;
		virtual int SendOne(const std::string& id, const Poco::Buffer<char>& buffer) = 0;
	};
}
#endif // TCP_SERVER_H_
