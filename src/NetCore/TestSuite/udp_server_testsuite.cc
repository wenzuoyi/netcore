#include "udp_server_testsuite.h"

namespace testsuite {
  UDPServerTestSuite::UDPServerTestSuite() {
    udp_end_point_ = netcore::UDPEndPoint::CreateInstance();
    poco_assert(udp_end_point_ != nullptr);
    udp_end_point_->SetEvent(this);
  }


  UDPServerTestSuite::~UDPServerTestSuite() {
	  udp_end_point_ = nullptr;
  }

  void UDPServerTestSuite::Start() {
	  udp_end_point_->Init();
	  exit_ = false;
	  static std::string content("UDP-Server:Hello, the world!");
	  static Poco::Buffer<char> buffer(content.c_str(), content.length());
    udp_end_point_->Start(11000, netcore::IPAddressType::kIPv4, false);
    work_task_ = std::async(std::launch::async, [this]() {
      while(!exit_) {
        auto address = std::make_shared<netcore::NetworkAddress>();
        address->ip = "127.0.0.1";
        address->ip_address_type = netcore::IPAddressType::kIPv4;
        address->port = 12000;
        udp_end_point_->Send(buffer, address);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
      return 0;
    });
  }

  void UDPServerTestSuite::Stop() {
	  exit_ = true;
	  work_task_.wait();
	  work_task_.get();
	  udp_end_point_->Stop();
	  udp_end_point_->Fini();
  }

  void UDPServerTestSuite::AutoStart() {
	  auto_exit_ = false;
    auto_task_= std::async(std::launch::async, [this]() {
      while(!auto_exit_) {
        Start();
        const auto time_intervale = rand() % 4801 + 200;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_intervale));
        //std::this_thread::sleep_for(std::chrono::hours(24));
        Stop();
      }
		  return 0;
	  });
  }

  void UDPServerTestSuite::AutoStop() {
	  auto_exit_ = true;
	  auto_task_.wait();
	  auto_task_.get();
  }

  void UDPServerTestSuite::OnUDPServerCreate(netcore::UDPEndPoint* server) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Server has been created!" << std::endl;
  }

  void UDPServerTestSuite::OnUDPServerRead(netcore::UDPEndPoint* server, netcore::NetworkAddressPtr address, const Poco::Buffer<char>& buffer) {
	  std::string content(buffer.begin(), buffer.size());
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Server receive from " << address->ip << ":" << address->port << " content(" << content.c_str() << ")" << std::endl;;
  }

  void UDPServerTestSuite::OnUDPServerException(netcore::UDPEndPoint* server, const std::string& message) {
    std::unique_lock<std::mutex> lock(*this);
    std::cout << "UDP-Server has been catch a exception(" << message.c_str() << ")" << std::endl;
  }

  void UDPServerTestSuite::OnUDPServerDestory(netcore::UDPEndPoint* server) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Server has been released!" << std::endl;
  }
}
  
