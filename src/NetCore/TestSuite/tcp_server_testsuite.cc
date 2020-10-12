#include "tcp_server_testsuite.h"
#include <time.h>
#include <iostream>
#include <thread>


namespace testsuite {
  TCPServerTestSuite::TCPServerTestSuite() {
	  srand(static_cast<unsigned>(time(nullptr)));
	  tcp_server_ = netcore::TCPServer::CreateInstance();
	  tcp_server_->SetEvent(this);
  };

  TCPServerTestSuite::~TCPServerTestSuite() {
	  tcp_server_ = nullptr;
  }

  void TCPServerTestSuite::Start() {
	  exit_ = false;
	  tcp_server_->Init();
    tcp_server_->Start(11990, 4);
    post_task_ = std::make_shared<std::thread>([this]() {
      while (!exit_) {
        if (tcp_server_ != nullptr) {
          std::string server_message("Server:Hello the world!");
          auto length = static_cast<int32_t>(server_message.length());
          Poco::Buffer<char> buffer(reinterpret_cast<const char*>(&length), sizeof(int32_t));
          buffer.append(&server_message[0], length);
          auto result = std::make_shared<netcore::SendResultList>();
          tcp_server_->SendAll(buffer, result);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
      return 0;
    });
  }

  void TCPServerTestSuite::Stop() {
    exit_ = true;
    post_task_->join();
    tcp_server_->Stop();
    tcp_server_->Fini();
  }

  void TCPServerTestSuite::AutoStart() {
	  exit_auto_test_ = false;
    auto_test_ = std::async(std::launch::async, [this]() {
		  while(!exit_auto_test_) {
			  SyncPrintMessage("begin server");
			  Start();
			  const auto time_intervale = rand() % 4801 + 200;
			  std::this_thread::sleep_for(std::chrono::milliseconds(time_intervale));
			  //std::this_thread::sleep_for(std::chrono::hours(24));
			  Stop();
			  SyncPrintMessage("terminate server");
		  }
		  return 0;
    });
  }

  void TCPServerTestSuite::AutoStop() {
	  exit_auto_test_ = true;
	  auto_test_.get();
  }


  void TCPServerTestSuite::OnTCPServerAccept(netcore::TCPServer*server, const std::string& id, netcore::BaseSessionInfoPtr base_session_info) {
    if (base_session_info != nullptr) {
	    std::unique_lock<std::mutex> lock(*this);
      std::cout << "accept , session id = " << id.c_str() << ", " << base_session_info->peer_ip << ":" <<
        base_session_info->peer_port << "=>" << base_session_info->local_ip << ":" << base_session_info->local_port <<
        std::endl;;
    }
  }

  void TCPServerTestSuite::OnTCPServerRead(netcore::TCPServer*server, const std::string& id, Poco::FIFOBuffer* buffer) {
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
    if (receive_string != "Client: Hello, the world!") {
		  std::cout << "not , right" << std::endl;
    }
  }

  void TCPServerTestSuite::OnTCPServerShutdown(netcore::TCPServer* server, const std::string& id,  netcore::BaseSessionInfoPtr base_session_info) {
    if (base_session_info != nullptr) {
      std::unique_lock<std::mutex> lock(*this);
      std::cout << "shutdown , session id = " << id.c_str() << ", " << base_session_info->peer_ip << ":" <<
        base_session_info->peer_port << "=>" << base_session_info->local_ip << ":" << base_session_info->local_port <<
        std::endl;
    }
  }

  void TCPServerTestSuite::OnTCPServerException(netcore::TCPServer*server, const std::string& id, const std::string& message) {
	  std::unique_lock<std::mutex> lock(*this);
    std::cout << "exception:id=" << id.c_str() << ",message" << message << std::endl;
  }

  void TCPServerTestSuite::SyncPrintMessage(const std::string& message) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << message << std::endl;
  }
}
