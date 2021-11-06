#ifndef UUID_F4A34DAF_E141_427B_9B6E_595646D04193
#define UUID_F4A34DAF_E141_427B_9B6E_595646D04193

#include "connection_creator.hpp"
#include "connection_ptr.hpp"
#include "fubble/signaling/answer.hpp"
#include "fubble/signaling/ice_candidate.hpp"
#include "fubble/signaling/logger.hpp"
#include "fubble/signaling/offer.hpp"
#include <fubble/utils/signal.hpp>
#include <boost/thread/future.hpp>

namespace utils {
class one_shot_timer;
}
namespace websocket {
class connector;
class connector_creator;
} // namespace websocket
namespace signaling::client {
class factory;
struct connect_information {
  bool secure{};
  std::string host;
  std::string service;
  std::string target;
};
class client {
public:
  virtual ~client() = default;

  // TODO remove setter, move argument to create/constructor
  virtual void set_connect_information(const connect_information &set) = 0;
  virtual void connect(const std::string &token, const std::string &key) = 0;
  virtual boost::future<void> close() = 0;
  virtual void send_offer(const signaling::offer &offer_) = 0;
  virtual void send_answer(const signaling::answer &answer_) = 0;
  virtual void
  send_ice_candidate(const signaling::ice_candidate &candidate) = 0;
  virtual void send_want_to_negotiate() = 0;

  utils::signal::signal<> on_closed;
  utils::signal::signal<> on_registered;
  utils::signal::signal<> on_create_offer;
  utils::signal::signal<const signaling::offer &> on_offer;
  utils::signal::signal<const signaling::answer &> on_answer;
  utils::signal::signal<const signaling::ice_candidate &>
      on_ice_candidate;
  utils::signal::signal<const boost::system::system_error &> on_error;

  static std::unique_ptr<client>
  create(websocket::connector_creator &connector_creator,
         connection_creator &connection_creator_);
  static std::unique_ptr<client>
  create_reconnecting(factory &factory, utils::one_shot_timer &timer);
};

class factory {
public:
  virtual ~factory() = default;
  virtual std::unique_ptr<client> create() = 0;
};

class factory_reconnecting : public factory {
public:
  factory_reconnecting(factory &factory, utils::one_shot_timer &timer);

  std::unique_ptr<client> create() override;

protected:
  factory &factory_;
  utils::one_shot_timer &timer;
};

class factory_impl : public factory {
public:
  factory_impl(websocket::connector_creator &connector_creator,
               connection_creator &connection_creator_,
               const connect_information &connect_information_);
  std::unique_ptr<client> create() override;

protected:
  websocket::connector_creator &connector_creator;
  connection_creator &connection_creator_;
  const connect_information connect_information_;
};
} // namespace signaling::client

#endif
