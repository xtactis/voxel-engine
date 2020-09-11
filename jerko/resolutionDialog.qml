import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    id: resourceDialog
    title: "Set Resolution"
    width: 150
    height: 100
    flags: Qt.Dialog
    modality: Qt.WindowModal
    property double resolution: 0.1
    property double inRes: 0.1

    function finish() {
        resolution = parseFloat(inputBox.text);
        resourceDialog.close();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 20
            TextField {
                id: inputBox
                focus: true
                validator: DoubleValidator {
                    bottom: 0.01
                    notation: DoubleValidator.StandardNotation
                    decimals: 3
                }
                Keys.onReturnPressed: finish()
                text: inRes.toString()
            }
        }
        Rectangle {
            height: 30
            Layout.fillWidth: true
            Button {
                text: "Apply"
                anchors.centerIn: parent
                onClicked: finish()
            }
        }
    }
}
