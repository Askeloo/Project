import QtQuick 2.12
import QtQuick.Controls 2.5

LoginPageForm {
    id: rect
    buttonLogIn.onClicked: {
        guiManager.logIn2()
        stackView.push("MainPage.qml")
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/