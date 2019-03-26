import QtQuick 2.4
import QtQuick.Controls 2.3

Page {
    property alias addButton: addButton
    property alias friendList: friendList

    Rectangle {
        x: 0
        y: 36
        width: friendList.width
        height: friendList.height
        color: friendListColor
    }

    ListView {

        id: friendList
        x: 0
        y: 36
        width: 269
        height: 600
        highlightRangeMode: ListView.ApplyRange
        flickableDirection: Flickable.VerticalFlick
        //color: "lightgrey";
        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Vertical

        focus: true
        highlight: Rectangle {
            color: "lightsteelblue"
            radius: 5
        }
    }

    Button {
        id: addButton
        x: 270
        y: 560
        text: qsTr("Add")
    }
}


/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
