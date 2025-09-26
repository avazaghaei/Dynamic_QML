import QtQuick
import QtQuick.Window
Window {
    id: rootWindow
    width: 800
    height: 480
    visible: true
    title: "Data Generator Demo"

    // A simple background
    Rectangle {
        anchors.fill: parent
        color: "#222"
    }

    // Container for dynamically created MovableBoxes
    Item {
        id: rootItem
        anchors.fill: parent
        objectName: "rootItem"
    }
}
