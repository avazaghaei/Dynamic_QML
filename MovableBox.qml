import QtQuick

Rectangle {
    id: box
    property string objectId: ""
    property string dataSource: ""
    property string colorHex: "#4ab471"
    property string displayText: "—"
    width: 120
    height: 60
    visible: true
    color: colorHex
    radius: 8
    border.width: 1
    border.color: "white"
    opacity: 0.95

    property real dx: 3
    property real windowWidth: parent ? parent.width : 800

    Text {
        id: label
        anchors.centerIn: parent
        text: displayText
        font.pixelSize: 18
        color: "white"
        font.bold: true
    }

    Timer {
        id: moveTimer
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            var newX = box.x + dx
            var rightBound = windowWidth - box.width

            if (newX < 0) {
                box.x = 0
                dx = Math.abs(dx)
            } else if (newX > rightBound) {
                box.x = rightBound
                dx = -Math.abs(dx)
            } else {
                box.x = newX
            }
        }
    }

    onParentChanged: {
        if (parent) {
            windowWidth = parent.width
            console.log("Box", objectId, "parent set, width:", windowWidth)
        }
    }

    Component.onCompleted: {
        console.log("Box", objectId, "completed at", x, y, "color:", colorHex)
        if (parent) {
            windowWidth = parent.width
        }
    }

    // Update window width when parent width changes
    Connections {
        target: parent
        enabled: parent !== null
        function onWidthChanged() {
            if (parent) {
                windowWidth = parent.width
            }
        }
    }
}
