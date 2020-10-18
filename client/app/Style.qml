pragma Singleton

import QtQuick 2.14

QtObject {
    property Theme current: themeLight

    property Theme themeLight: ThemeLight {}
    property Theme themeDark: ThemeDark {}
}
