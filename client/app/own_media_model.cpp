#include "own_media_model.hpp"

using namespace client;

own_media_model::own_media_model(own_media &own_media_,
                                 own_audio_information &audio_information_)
    : own_media_(own_media_), audio_information_(audio_information_) {}

own_media_model::~own_media_model() = default;
