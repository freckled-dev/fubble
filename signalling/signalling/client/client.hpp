#ifndef UUID_F4A34DAF_E141_427B_9B6E_595646D04193
#define UUID_F4A34DAF_E141_427B_9B6E_595646D04193

#include "connection_creator.hpp"
#include "connection_ptr.hpp"
#include "signalling/answer.hpp"
#include "signalling/ice_candidate.hpp"
#include "signalling/logger.hpp"
#include "signalling/offer.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace utils {
class one_shot_timer;
}
namespace websocket {
class connector;
class connector_creator;
} // namespace websocket
namespace signalling::client {
class client_factory;
class client {
public:
  struct connect_information {
    bool secure{};
    std::string host;
    std::string service;
    std::string target;
  };
  virtual ~client() = default;

  // TODO remove setter, move argument to create/constructor
  virtual void set_connect_information(const connect_information &set) = 0;
  virtual void connect(const std::string &key) = 0;
  virtual boost::future<void> close() = 0;
  virtual void send_offer(const signalling::offer &offer_) = 0;
  virtual void send_answer(const signalling::answer &answer_) = 0;
  virtual void
  send_ice_candidate(const signalling::ice_candidate &candidate) = 0;
  virtual void send_want_to_negotiate() = 0;

  boost::signals2::signal<void()> on_closed;
  boost::signals2::signal<void()> on_registered;
  boost::signals2::signal<void()> on_create_offer;
  boost::signals2::signal<void(const signalling::offer &)> on_offer;
  boost::signals2::signal<void(const signalling::answer &)> on_answer;
  boost::signals2::signal<void(const signalling::ice_candidate &)>
      on_ice_candidate;
  boost::signals2::signal<void(const boost::system::system_error &)> on_error;

  static std::unique_ptr<client>
  create(websocket::connector_creator &connector_creator,
         connection_creator &connection_creator_);
  static std::unique_ptr<client>
  create_reconnecting(client_factory &factory, utils::one_shot_timer &timer);
};

// TODO rename to factory?
class client_factory {
public:
  virtual ~client_factory() = default;
  virtual std::unique_ptr<client> create() = 0;
};

class client_factory_reconnecting : public client_factory {
public:
  client_factory_reconnecting(client_factory &factory,
                              utils::one_shot_timer &timer);

  std::unique_ptr<client> create() override;

protected:
  client_factory &factory;
  utils::one_shot_timer &timer;
};

class client_factory_impl : public client_factory {
public:
  client_factory_impl(websocket::connector_creator &connector_creator,
                      connection_creator &connection_creator_,
                      const client::connect_information &connect_information_);
  std::unique_ptr<client> create() override;

protected:
  websocket::connector_creator &connector_creator;
  connection_creator &connection_creator_;
  const client::connect_information connect_information_;
};
} // namespace signalling::client

#endif
