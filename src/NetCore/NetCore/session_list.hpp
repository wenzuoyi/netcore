#ifndef SESSION_LIST_HPP_
#define SESSION_LIST_HPP_
#include <map>
#include <functional>
#include <Poco/RWLock.h>
#include "./include/netcore_datatype.h"
#include "base_session_handler.hpp"
namespace netcore {
  template <class T>
  class SessionList {
  public:
    SessionList() = default;

    virtual ~SessionList() = default;

    void Append(const std::string& id, BaseSessionHandler<T>* session) const {
      Poco::ScopedWriteRWLock write_lock(rw_lock_);
      session_map_.insert(std::make_pair(id, session));
    }

    bool Remove(const std::string& id) const {
      auto item = session_map_.find(id);
      if (item == session_map_.end()) {
        return false;
      }
      auto session = item->second;
      delete session;
      session_map_.erase(item);
      return true;
    }

	  void GetSession(const std::string& id, bool write, std::function<void(BaseSessionHandler<T>*)>&& handler) const {
		  Poco::ScopedRWLock read_lock(rw_lock_, write);
		  if (session_map_.find(id) == session_map_.end()) {
			  return;
		  }
		  handler(session_map_[id]);
	  }

	  void GetSession(const SessionIDList& session_ids, bool write, std::function<void(const std::string& id, BaseSessionHandler<T>*)>&& handler) const {
		  Poco::ScopedRWLock read_lock(rw_lock_, write);
		  for (auto iter = session_ids.begin(); iter != session_ids.end(); ++iter) {
			  auto item = session_map_.find(*iter);
			  if (item != session_map_.end()) {
				  handler(item->first, item->second);
			  } else {
				  handler(*iter, nullptr);
			  }
		  }
	  }

    void GetSession(bool write, std::function<void(const std::string& id, BaseSessionHandler<T>*)>&& handler) const {
      Poco::ScopedRWLock read_lock(rw_lock_, write);
      for (auto iter = session_map_.begin(); iter != session_map_.end(); ++iter) {
        handler(iter->first, iter->second);
      }
    }

    void CloseSession() const {
      Poco::ScopedWriteRWLock write_lock(rw_lock_);
      for (auto iter = session_map_.begin(); iter != session_map_.end(); ++iter) {
        auto item = iter->second;
        delete item;
      }
      session_map_.clear();
    }

  private:
	  mutable Poco::RWLock rw_lock_;
	  mutable std::map<std::string, BaseSessionHandler<T>*> session_map_;
  };
}
#endif // SESSION_LIST_HPP_
