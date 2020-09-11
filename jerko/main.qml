import QtQuick 2.3
import Qt3D.Render 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.3

import Voxels 1.0

Item {
    id: app
    width: 1280
    height: menuBar.height+statusBar.height+glStuff.height

    //focus: true

    function getPath(path) {
        return decodeURIComponent(path.toString().slice(7))
    }

    function basename(path) {
        return path.slice(path.lastIndexOf("/")+1);
    }

    FileDialog {
        id: openDialog
        title: "Open"
        folder: shortcuts.home
        onAccepted: {
            geom.newProject()
            geom.open(getPath(this.fileUrl))
            files.append({fileName: getPath(this.fileUrl)})
            glStuff.focus = true
            this.close()
        }
        onRejected: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: false
        nameFilters: [ "Farina files (*.far)", "Other files (*)" ]
    }

    FileDialog {
        id: saveAsDialog
        title: "Save as"
        folder: shortcuts.home
        onAccepted: {
            geom.setSave(getPath(this.fileUrl))
            geom.save()
            files.append({fileName: this.fileUrl})
            glStuff.focus = true
            this.close()
        }
        onRejected: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: false
        selectExisting: false
        nameFilters: [ "Farina files (*.far)", "Other files (*)" ]
    }

    FileDialog {
        id: importDialog
        title: "Import"
        folder: shortcuts.home
        onAccepted: {
            var paths = this.fileUrls
            for (let path of paths) {
                geom.importModel(getPath(path))
            }
            glStuff.focus = true
            this.close()
        }
        onRejected: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: true
        nameFilters: [ "Point files (*.pts)",
                       "OcTree files (*.ot)",
                       "Wavefront OBJ (*.obj)",
                       "Stanford Triangle Format PLY (*.ply)",
                       "STL (STereoLithography) (*.stl)"]
    }

    FileDialog {
        id: exportDialog
        title: "Export as"
        folder: shortcuts.home
        onAccepted: {
            geom.exportModel(getPath(this.fileUrl))
            glStuff.focus = true
            this.close()
        }
        onRejected: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: false
        nameFilters: [ "Point files (*.pts)", "OcTree files (*.ot)" ]
    }

    MessageDialog {
        id: discardAndMakeNewDialog
        title: "Discard changes?"
        icon: StandardIcon.Question
        text: "There are unsaved changes. Do you want to discard them and make a new project?"
        standardButtons: StandardButton.Yes | StandardButton.No
        Component.onCompleted: visible = false
        onYes: {
            glStuff.focus = true
            this.close()
            geom.newProject()
        }
        onNo: {
            glStuff.focus = true
            this.close()
        }
    }

    MessageDialog {
        id: discardAndOpenDialog
        title: "Discard changes?"
        icon: StandardIcon.Question
        text: "There are unsaved changes. Do you want to discard them and open a project?"
        standardButtons: StandardButton.Yes | StandardButton.No
        Component.onCompleted: visible = false
        onYes: {
            glStuff.focus = true
            this.close()
            openDialog.open()
        }
        onNo: {
            glStuff.focus = true
            this.close()
        }
    }

    MessageDialog {
        id: discardAndCloseDialog
        title: "Discard changes?"
        icon: StandardIcon.Question
        text: "There are unsaved changes. Do you want to discard them and close Farina?"
        standardButtons: StandardButton.Yes | StandardButton.No
        Component.onCompleted: visible = false
        onYes: {
            this.close()
            Qt.quit()
        }
        onNo: {
            glStuff.focus = true
            this.close()
        }
    }

    MessageDialog {
        id: aboutDialog
        title: "About Farina"
        text: "Farina v0.1 is the first stable version of the Farina OcTree editor."
        onAccepted: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
    }

    MessageDialog {
        id: helpDialog
        title: "Help"
        text: "Helpful stuff will be displayed here at some point in the future"
        onAccepted: {
            glStuff.focus = true
            this.close()
        }
        Component.onCompleted: visible = false
    }

    MenuBar {
        id: menuBar
        x: 0
        y: 0
        width: app.width

        Menu {
            title: qsTr("&File")
            Action {
                id: newAction
                text: qsTr("&New (Ctrl+N)")
                enabled: true
                shortcut: StandardKey.New
                onTriggered: {
                    if (geom.unsaved()) {
                        discardAndMakeNewDialog.open()
                    } else {
                        geom.newProject()
                    }
                }
            }
            Action {
                id: openAction
                text: qsTr("&Open (Ctrl+O)")
                enabled: true
                shortcut: StandardKey.Open
                onTriggered: {
                    if (geom.unsaved()) {
                        discardAndOpenDialog.open()
                    } else {
                        openDialog.open()
                    }
                }
            }
            Menu {
                id: openRecentAction
                title: qsTr("Open Recent")
                enabled: files.count > 0
                Instantiator {
                    // TODO: save these to a file or sth
                    model: ListModel { id: files }

                    MenuItem {
                        text: fileName
                        onTriggered: {
                            if (geom.unsaved()) {
                                discardAndOpenDialog.open()
                            } else {
                                geom.newProject()
                                geom.open(fileName)
                            }
                        }
                    }

                    onObjectAdded: {
                        openRecentAction.insertItem(index, object)
                    }
                    onObjectRemoved: openRecentAction.removeItem(object)
                }
            }
            Action {
                id: saveAction
                text: qsTr("&Save (Ctrl+S)")
                enabled: true
                shortcut: StandardKey.Save
                onTriggered: {
                    if (geom.canSave()) {
                        geom.save()
                    } else {
                        saveAsDialog.open()
                    }
                }
            }
            Action {
                id: saveAsAction
                text: qsTr("Save As")
                enabled: true
                onTriggered: saveAsDialog.open()
            }
            MenuSeparator {}
            Action {
                id: importAction
                text: qsTr("&Import (Ctrl+I)")
                shortcut: "Ctrl+I"
                enabled: true
                onTriggered: importDialog.open()
            }
            Action {
                id: importAsNewAction
                text: qsTr("Import in new project")
                enabled: true
                onTriggered: {
                    if (geom.unsaved()) {
                        discardAndImportNewDialog.open()
                    } else {
                        geom.newProject()
                        importDialog.open()
                    }
                }
            }
            Action {
                id: exportAction
                text: qsTr("&Export as (Ctrl+E)")
                enabled: false
                shortcut: "Ctrl+E"
                onTriggered: exportDialog.open()
            }
            Action {
                id: exportLayersAction
                text: qsTr("Export layers")
                enabled: false
                shortcut: "Ctrl+E"
                // onTriggered: exportLayersDialog.open()
            }

            MenuSeparator {}
            Action {
                id: quitAction
                text: qsTr("&Quit Alt+F4")
                shortcut: StandardKey.Quit
                enabled: true
                onTriggered: {
                    if (geom.unsaved()) {
                        discardAndCloseDialog.open()
                    } else {
                        Qt.quit()
                    }
                }
            }
        }
        Menu {
            title: qsTr("&Edit")
            Action {
                id: undoAction
                text: qsTr("Undo (Ctrl+&Z)")
                shortcut: StandardKey.Undo
                onTriggered: geom.undo()
                enabled: false
            }
            Action {
                id: redoAction
                text: qsTr("Redo (Ctrl+&Y)")
                shortcut: StandardKey.Redo
                onTriggered: geom.redo()
                enabled: false
            }
            Action {
                id: cutAction
                text: qsTr("Cut (Ctrl+&X)")
                shortcut: StandardKey.Cut
                onTriggered: geom.cut()
                enabled: false
            }
            Action {
                id: copyAction
                text: qsTr("&Copy (Ctrl+C)")
                shortcut: StandardKey.Copy
                onTriggered: geom.copy()
                enabled: false
            }
            Action {
                id: pasteAction
                text: qsTr("Paste (Ctrl+&V)")
                shortcut: StandardKey.Copy
                onTriggered: geom.copy()
                enabled: false
            }
            Action {
                id: changeResolutionAction
                text: qsTr("Change &Resolution")
                enabled: true
                onTriggered: {
                    let comp = Qt.createComponent("resolutionDialog.qml")
                    console.log(comp.status);
                    if (comp.status === Component.Ready) {
                        let dialog = comp.createObject(parent, {inRes: geom.getResolution()})
                        console.log(dialog)
                        dialogConnection.target = dialog
                        dialog.show()
                    } else {
                        console.log(comp.errorString())
                    }
                }
            }
            Connections {
                id: dialogConnection
                onVisibleChanged: {
                    if (!target.visible) {
                        console.log(target.resolution)
                        geom.setResolution(target.resolution)
                    }
                }
            }
        }

        MenuBarItem {
            id: aboutAction
            text: qsTr("About")
            onTriggered: aboutDialog.open()
        }
        MenuBarItem {
            id: helpAction
            text: qsTr("Help")
            // shortcut: "F1" doesn't work with MenuBarItem...
            onTriggered: helpDialog.open()
        }
    }

    Item {
        id: glStuff

        focus: true
        width: 1280
        height: 720
        y: menuBar.height

        // pretty bad, make it smooth or sth
        Keys.onPressed: {
            if (event.key === Qt.Key_W)
                geom.move(0, -0.5, 0)
            if (event.key === Qt.Key_A)
                geom.move(0.5, 0, 0)
            if (event.key === Qt.Key_S)
                geom.move(0, 0.5, 0)
            if (event.key === Qt.Key_D)
                geom.move(-0.5, 0, 0)
            if (event.key === Qt.Key_R)
                geom.move(0, 0, 0.5)
            if (event.key === Qt.Key_F)
                geom.move(0, 0, -0.5)
        }

        MouseArea {
            id: mouse
            anchors.fill: glStuff
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
            // consider just passing the mouse to geom and handling there
            onPositionChanged: {
                if ((mouse.buttons & Qt.LeftButton) && (mouse.buttons & Qt.RightButton)) {
                    geom.pan(mouse.x, mouse.y);
                } else if (mouse.buttons & Qt.LeftButton) {
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        geom.selectBox(mouse.x, mouse.y)
                    } else {
                        geom.mouseMove(mouse.x, mouse.y)
                    }
                }
            }
            onPressed: {
                if (mouse.button === Qt.LeftButton && (mouse.modifiers & Qt.ShiftModifier)) {
                    console.log("SHIFT CLICK", mouse.x, mouse.y)
                    geom.select(mouse.x, mouse.y)
                } else if (mouse.button === Qt.LeftButton) {
                    console.log("CLICK", mouse.x, mouse.y);
                    geom.setMouse(mouse.x, mouse.y)
                } else if (mouse.button === Qt.MiddleButton) {
                    console.log("SCROLL CLICK")
                    geom.setMouse(mouse.x, mouse.y)
                }
            }
            onWheel: {
                geom.move(x, y, wheel.angleDelta.y/120);
            }
        }

        Geometry {
            id: geom
            position: Qt.vector3d(0, 0, -30)
            rotation: Qt.quaternion(0, 0, 0, 0)
        }
    }
    Rectangle { // TODO: figure this out eventually
        id: statusBar
        height: 30
        y: app.height-this.height
        width: app.width
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 2
            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 20
                ProgressBar {
                    objectName: actualProgressBar
                    height: 15
                    indeterminate: false
                }
                Text {
                    objectName: progressBarText
                    text: "Ready"
                }
            }
        }
    }
}

