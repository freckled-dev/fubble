#include "session_module.hpp"
#include "client/factory.hpp"
#include "client/joiner.hpp"
#include "client/leaver.hpp"
#include "client/own_media.hpp"
#include "client/participant_creator_creator.hpp"
#include "client/room_creator.hpp"
#include "client/rooms.hpp"
#include "client/tracks_adder.hpp"
#include "peer_creator.hpp"

using namespace client;

session_module::session_module(
    std::shared_ptr<utils::executor_module> executor_module,
    std::shared_ptr<matrix::module> matrix_module,
    std::shared_ptr<rtc::google::module> rtc_module,
    std::shared_ptr<signaling::client_module> signaling_module,
    std::shared_ptr<temporary_room::client_module> temporary_room_module,
    std::shared_ptr<version::client_module> version_client_module,
    const config &config_)
    : executor_module{executor_module}, matrix_module{matrix_module},
      rtc_module{rtc_module}, signaling_module{signaling_module},
      temporary_room_module{temporary_room_module},
      version_client_module{version_client_module}, config_{config_} {}

std::shared_ptr<joiner> session_module::get_joiner() {
  if (!joiner_)
    joiner_ = std::make_shared<joiner>(*get_room_creator(), *get_rooms(),
                                       *matrix_module->get_authentification(),
                                       *temporary_room_module->get_client(),
                                       version_client_module->get_getter());
  return joiner_;
}

std::shared_ptr<leaver> session_module::get_leaver() {
  if (!leaver_)
    leaver_ = std::make_shared<leaver>(*get_rooms());
  return leaver_;
}

std::shared_ptr<tracks_adder> session_module::get_tracks_adder() {
  if (!tracks_adder_)
    tracks_adder_ = std::make_shared<tracks_adder>();
  return tracks_adder_;
}

std::shared_ptr<rooms> session_module::get_rooms() {
  if (!rooms_)
    rooms_ = std::make_shared<rooms>();
  return rooms_;
}

std::shared_ptr<own_media> session_module::get_own_media() {
  if (!own_media_)
    own_media_ = std::make_shared<own_media>();
  return own_media_;
}

std::shared_ptr<room_creator> session_module::get_room_creator() {
  if (!room_creator_)
    room_creator_ = std::make_shared<client::room_creator>(
        *get_participant_creator_creator());
  return room_creator_;
}

std::shared_ptr<factory> session_module::get_factory() {
  if (!factory_)
    factory_ = std::make_shared<factory>();
  return factory_;
}

std::shared_ptr<peer_creator> session_module::get_peer_creator() {
  if (!peer_creator_)
    peer_creator_ = std::make_shared<peer_creator>(
        *executor_module->get_boost_executor(),
        *signaling_module->get_client_creator(), *rtc_module->get_factory());
  return peer_creator_;
}

std::shared_ptr<participant_creator_creator>
session_module::get_participant_creator_creator() {
  if (!participant_creator_creator_)
    participant_creator_creator_ =
        std::make_shared<client::participant_creator_creator>(
            *get_factory(), *get_peer_creator(), *get_tracks_adder(),
            *get_own_media());
  return participant_creator_creator_;
}
