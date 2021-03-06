#ifndef UUID_E1787C7F_9E5F_4756_9483_18AC5D05415D
#define UUID_E1787C7F_9E5F_4756_9483_18AC5D05415D

#include <fubble/matrix/module.hpp>
#include <fubble/rtc/module.hpp>
#include <fubble/signaling/client_module.hpp>
#include <fubble/temporary_room/client_module.hpp>
#include <fubble/utils/executor_module.hpp>
#include <fubble/version/client_module.hpp>

namespace client {
class factory;
class joiner;
class leaver;
class own_media;
class participant_creator_creator;
class peer_creator;
class peers;
class room_creator;
class rooms;
class tracks_adder;
class FUBBLE_PUBLIC session_module {
public:
  struct config {
    bool receive_audio{true};
    bool receive_video{true};
  };
  session_module(
      std::shared_ptr<utils::executor_module> executor_module,
      std::shared_ptr<matrix::module> matrix_module,
      std::shared_ptr<rtc::module> rtc_module,
      std::shared_ptr<signaling::client_module> signaling_module,
      std::shared_ptr<temporary_room::client_module> temporary_room_module,
      std::shared_ptr<version::client_module> version_client_module,
      const config &config_);

  std::shared_ptr<joiner> get_joiner();
  std::shared_ptr<leaver> get_leaver();
  std::shared_ptr<tracks_adder> get_tracks_adder();
  std::shared_ptr<rooms> get_rooms();
  std::shared_ptr<own_media> get_own_media();
  std::shared_ptr<peers> get_peers();

protected:
  std::shared_ptr<room_creator> get_room_creator();
  std::shared_ptr<factory> get_factory();
  std::shared_ptr<peer_creator> get_peer_creator();
  std::shared_ptr<participant_creator_creator>
  get_participant_creator_creator();

  const std::shared_ptr<utils::executor_module> executor_module;
  const std::shared_ptr<matrix::module> matrix_module;
  const std::shared_ptr<rtc::module> rtc_module;
  const std::shared_ptr<signaling::client_module> signaling_module;
  const std::shared_ptr<temporary_room::client_module> temporary_room_module;
  const std::shared_ptr<version::client_module> version_client_module;
  const config config_;

  std::shared_ptr<joiner> joiner_;
  std::shared_ptr<leaver> leaver_;
  std::shared_ptr<tracks_adder> tracks_adder_;
  std::shared_ptr<rooms> rooms_;
  std::shared_ptr<own_media> own_media_;
  std::shared_ptr<factory> factory_;
  std::shared_ptr<room_creator> room_creator_;
  std::shared_ptr<participant_creator_creator> participant_creator_creator_;
  std::shared_ptr<peer_creator> peer_creator_;
  std::shared_ptr<peers> peers_;
};
} // namespace client

#endif
