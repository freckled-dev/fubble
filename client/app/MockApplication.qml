import QtQuick 2.12
import io.fubble 1.0

FubbleApplication {

    property var appVersion: "1.0.0-alpha+deadbeef"

    joinModel: JoinModel {}
    leaveModel: LeaveModel {}
    errorModel: ErrorModel {}
    utilsModel: UtilsModel {}
    shareDesktopModel: ShareDesktopModel {}
    audioVideoModel: AudioVideoSettingsModel {}
    ownMediaModel: OwnMediaModel {}
}
