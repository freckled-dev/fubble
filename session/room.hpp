#ifndef UUID_5D5307B7_976F_4300_8388_7400A858C71C
#define UUID_5D5307B7_976F_4300_8388_7400A858C71C

#include "logging/logger.hpp"
#include "participant.hpp"
#include <boost/signals2/signal.hpp>
#include <nakama-cpp/Nakama.h>

namespace session {
class client;
class room {
public:
  room(client &client_, Nakama::NChannelPtr channel);
  using participants = std::vector<participant>;
  boost::signals2::signal<void(participants)> on_joins;
  boost::signals2::signal<void(std::vector<std::string>)> on_leaves;
  boost::signals2::signal<void(participants)> on_updates;
  const participants &get_participants() const;
  const std::string &get_name() const;
  const std::string &own_id() const;

protected:
  void on_channel_message(const Nakama::NChannelMessage &message);
  void on_channel_presence(const Nakama::NChannelPresenceEvent &event);
  void on_nakama_joins(const std::vector<Nakama::NUserPresence> &presences);
  void on_nakama_leaves(const std::vector<Nakama::NUserPresence> &presences);
  void on_names(const Nakama::NUsers &users);
  void on_error(const Nakama::NError &error);

  logging::logger logger;
  client &client_;
  Nakama::NChannelPtr channel;
  participants participants_;
  std::vector<boost::signals2::scoped_connection> signal_connections;
};
} // namespace session

#endif
