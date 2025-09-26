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
        width : (parent.width) / 2
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top : parent.top
        color: "#222"
    }

    // Container for dynamically created MovableBoxes
    Item {
        id: rootItem
        anchors.fill: parent
        objectName: "rootItem"
    }
}
