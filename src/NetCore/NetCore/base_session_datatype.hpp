#ifndef BASE_SESSION_DATATYPE_HPP_
#define BASE_SESSION_DATATYPE_HPP_
#include <memory>
#include <Poco/NotificationQueue.h>
#include <Poco/Net/SocketReactor.h>
namespace netcore {
  template <class T>
	class BaseSessionHandlerEvent;
	
  template <class T>
  struct Context {
	  std::shared_ptr<Poco::Net::SocketReactor> reactor = nullptr;
	  std::shared_ptr<Poco::NotificationQueue> task_queue = nullptr;
	  BaseSessionHandlerEvent<T>* event = nullptr;
  };
  
  template <class T>
  using ContextPtr = std::shared_ptr<Context<T>>;

  template<class T>
  struct HandlerParam {
		T* socket = nullptr;
		ContextPtr<T> context = nullptr;
	};

  template<class T>
	using HandlerParamPtr = std::shared_ptr<HandlerParam<T>>;
}
#endif // BASE_SESSION_DATATYPE_HPP_
