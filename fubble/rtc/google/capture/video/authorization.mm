#include "authorization.hpp"
#import <AVFoundation/AVCaptureDevice.h>
#import <Foundation/Foundation.h>

using namespace rtc::google::capture::video;

namespace {
struct authorization_apple : authorization {
  void request(on_finished_type on_finished) override {
    switch (
        [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
    case AVAuthorizationStatusAuthorized:
      on_finished(true);
      break;
    case AVAuthorizationStatusNotDetermined:
      [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo
                               completionHandler:[on_finished](BOOL granted) {
                                 on_finished(granted);
                               }];
      break;
    case AVAuthorizationStatusDenied: {
      // The user has previously denied access.
      return;
    }
    case AVAuthorizationStatusRestricted: {
      // The user can't grant access due to restrictions.
      return;
    }
    }
  }

  state get_state() override {
    switch (
        [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo]) {
    case AVAuthorizationStatusAuthorized:
      return state::authorized;
    case AVAuthorizationStatusNotDetermined:
      return state::not_determined;
    case AVAuthorizationStatusDenied:
      return state::denied;
    case AVAuthorizationStatusRestricted:
      return state::restricted;
    }
  }
};
}

std::unique_ptr<authorization> authorization::create() {
  return std::make_unique<authorization_apple>();
}
