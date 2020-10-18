import QtQuick 2.14
import QtQml 2.0

QtObject {
    id: errorContainer
    property string text: ""
    property string technical: ""

    signal error
}
