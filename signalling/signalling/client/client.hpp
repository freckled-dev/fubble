#ifndef UUID_F4A34DAF_E141_427B_9B6E_595646D04193
#define UUID_F4A34DAF_E141_427B_9B6E_595646D04193

#include "connection_creator.hpp"
#include "connection_ptr.hpp"
#include "signalling/answer.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/logger.hpp"
#include "signalling/offer.hpp"
#include "websocket/connector.hpp"
#include "websocket/connection.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace signalling::client {
class client {
public:
  struct connect_information {
    std::string host;
    std::string service;
    std::string target;
  };
  client(websocket::connector_creator &connector_creator,
         connection_creator &connection_creator_);
  ~client();

  void set_connect_information(const connect_information &set);
  void connect(const std::string &key);
  boost::future<void> close();
  boost::signals2::signal<void()> on_closed;
  boost::signals2::signal<void()> on_registered;
  boost::signals2::signal<void()> on_create_offer;
  boost::signals2::signal<void(const signalling::offer &)> on_offer;
  boost::signals2::signal<void(const signalling::answer &)> on_answer;
  boost::signals2::signal<void(const signalling::ice_candidate &)>
      on_ice_candidate;
  boost::signals2::signal<void(const boost::system::system_error &)> on_error;

  void send_offer(const signalling::offer &offer_);
  void send_answer(const signalling::answer &answer_);
  void send_ice_candidate(const signalling::ice_candidate &candidate);
  void send_want_to_negotiate();

  connection &get_connection() const;

private:
  void connected(boost::future<websocket::connection_ptr> &result,
                 const std::string &key);
  void connect_signals(const connection_ptr &connection_) const;
  void run_done(boost::future<void> &result);

  signalling::logger logger{"client"};
  boost::inline_executor executor;
  websocket::connector_creator &connector_creator;
  connection_creator &connection_creator_;
  connect_information connect_information_;
  std::unique_ptr<websocket::connector> connector;
  connection_ptr connection_;
};
} // namespace signalling::client

#endif
