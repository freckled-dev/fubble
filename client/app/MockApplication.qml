import QtQuick 2.15
import io.fubble 1.0

FubbleApplication {

    property var appVersion: "1.0.0-alpha+deadbeef"

    joinModel: JoinModel {}
    leaveModel: LeaveModel {}
    errorModel: ErrorModel {}
    utilsModel: UtilsModel {}
    roomsModel: RoomsModel {}
    shareDesktopModel: ShareDesktopModel {}
    audioVideoModel: AudioVideoSettingsModel {}
    languageModel: LanguageSettingsModel {}
    ownMediaModel: OwnMediaModel {}
}
