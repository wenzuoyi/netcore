
#include "tcp_client_testsuite.h"
#include "netcore_errorcode.h"

namespace testsuite {
	TCPClientTestSuite::TCPClientTestSuite() = default;

	TCPClientTestSuite::~TCPClientTestSuite() = default;

  void TCPClientTestSuite::Start() {
    client_ = netcore::TCPClient::CreateInstance();
    if (client_ == nullptr) {
      return;
    }
    client_->SetEvent(this);
    client_->Init();
    if (netcore::OK != client_->Connect("127.0.0.1", 11990, netcore::IPAddressType::kIPv4)) {
		  std::cout << "connect false!!" << std::endl;
      return;
    }
    exit_ = false;
    post_task_ = std::async(std::launch::async, [this]() {
      while (!exit_) {
        if (client_ != nullptr) {
          std::string server_message("Client: Hello, the world!");
          auto length = static_cast<int32_t>(server_message.length());
          Poco::Buffer<char> buffer(reinterpret_cast<const char*>(&length), sizeof(int32_t));
          buffer.append(&server_message[0], length);
          client_->Send(buffer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
    });
  }

  void TCPClientTestSuite::Stop() {
    if (client_ != nullptr) {
      if (!exit_) {
        exit_ = true;
        post_task_.wait();        
        client_->Disconnect();
      }
      client_->Fini();
      client_ = nullptr;
    }
  }

  void TCPClientTestSuite::AutoStart() {
	  auto_test_ = true;
	  auto_task_ = std::async(std::launch::async, [this]() {
      while (auto_test_) {
        SyncPrintMessage("client start connect");
        Start();
        const auto time_intervale = rand() % 1801 + 200;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_intervale));
		    //std::this_thread::sleep_for(std::chrono::hours(24));
        Stop();
        SyncPrintMessage("client stop connection");
      }
	  });
  }

  void TCPClientTestSuite::AutoStop() {
	  auto_test_ = false;
	  auto_task_.wait();
  }

  void TCPClientTestSuite::OnTCPClientConnect(netcore::TCPClient* client, netcore::BaseSessionInfoPtr peer_base_info) {
	  if (peer_base_info != nullptr) {
		  std::unique_lock<std::mutex> lock(*this);
		  std::cout << "connected ," << peer_base_info->peer_ip << ":" <<
			  peer_base_info->peer_port << "=>" << peer_base_info->local_ip << ":" << peer_base_info->local_port <<std::endl;
	  }
  }

  void TCPClientTestSuite::OnTCPClientDisconnect(netcore::TCPClient* client, netcore::BaseSessionInfoPtr peer_base_info) {
	  if (peer_base_info != nullptr) {
		  std::unique_lock<std::mutex> lock(*this);
		  std::cout << "disconnected ," << peer_base_info->peer_ip << ":" <<
			  peer_base_info->peer_port << "=>" << peer_base_info->local_ip << ":" << peer_base_info->local_port <<std::endl;;
	  } else {
      std::cout << "disconnectd connection!" << std::endl;
	  }
  }

  void TCPClientTestSuite::OnTCPClientException(netcore::TCPClient* client, const std::string& exception_message) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "exception: message = " << exception_message << std::endl;
  }

  void TCPClientTestSuite::OnTCPClientRead(netcore::TCPClient* client, Poco::FIFOBuffer* buffer) {
	  if (buffer->used() < 4) {
		  return;
	  }
	  int32_t length = 0;
	  buffer->peek(reinterpret_cast<char*>(&length), sizeof(int32_t));
	  if (buffer->used() < sizeof(int32_t) + static_cast<unsigned>(length)) {
		  return;
	  }
	  buffer->drain(sizeof(int32_t));
	  std::string receive_string(length, 0);
	  buffer->read(&receive_string[0], length);
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << receive_string << std::endl;
	  if (receive_string != "Server:Hello the world!") {
		  std::cout << "not , right" << std::endl;
	  }
  }

  void TCPClientTestSuite::SyncPrintMessage(const std::string& message) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << message << std::endl;
  }
}

