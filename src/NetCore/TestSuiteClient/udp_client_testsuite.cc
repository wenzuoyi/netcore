#include "udp_client_testsuite.h"

namespace testsuite {
  UDPClientTestSuite::UDPClientTestSuite() {
    udp_end_point_ = netcore::UDPEndPoint::CreateInstance();
    poco_assert(udp_end_point_ != nullptr);
    udp_end_point_->SetEvent(this);
  }


  UDPClientTestSuite::~UDPClientTestSuite() {
	  udp_end_point_ = nullptr;
  }

  void UDPClientTestSuite::Start() {
	  udp_end_point_->Init();
	  exit_ = false;
	  static std::string content("UDP-Client:Hello, the world!");
	  static Poco::Buffer<char> buffer(content.c_str(), content.length());
    udp_end_point_->Start(12000, netcore::IPAddressType::kIPv4, false);
    work_task_ = std::async(std::launch::async, [this]() {
      while(!exit_) {
        auto address = std::make_shared<netcore::NetworkAddress>();
        address->ip = "127.0.0.1";
        address->ip_address_type = netcore::IPAddressType::kIPv4;
        address->port = 11000;
        udp_end_point_->Send(buffer, address);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
      }
      return 0;
    });
  }

  void UDPClientTestSuite::Stop() {
	  exit_ = true;
	  work_task_.wait();
	  work_task_.get();
	  udp_end_point_->Stop();
	  udp_end_point_->Fini();
  }

  void UDPClientTestSuite::AutoStart() {
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

  void UDPClientTestSuite::AutoStop() {
	  auto_exit_ = true;
	  auto_task_.wait();
	  auto_task_.get();
  }

  void UDPClientTestSuite::OnUDPServerCreate(netcore::UDPEndPoint* server) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Client has been created!" << std::endl;
  }

  void UDPClientTestSuite::OnUDPServerRead(netcore::UDPEndPoint* server, netcore::NetworkAddressPtr address, const Poco::Buffer<char>& buffer) {
	  std::string content(buffer.begin(), buffer.size());
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Client receive from " << address->ip << ":" << address->port << " content(" << content.c_str() << ")" << std::endl;
  }

  void UDPClientTestSuite::OnUDPServerException(netcore::UDPEndPoint* server, const std::string& message) {
    std::unique_lock<std::mutex> lock(*this);
    std::cout << "UDP-Client has been catch a exception(" << message.c_str() << ")" << std::endl;
  }

  void UDPClientTestSuite::OnUDPServerDestory(netcore::UDPEndPoint* server) {
	  std::unique_lock<std::mutex> lock(*this);
	  std::cout << "UDP-Client has been released!" << std::endl;
  }
}
  
