pragma Singleton

import QtQuick 2.0

QtObject {
    property Theme current: themeLight
    property bool isLightTheme: true

    property Theme themeLight: ThemeLight {}
    property Theme themeDark: ThemeDark {}
}
