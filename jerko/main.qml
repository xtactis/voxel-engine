import QtQuick 2.3
import QtQuick.Controls 2.3
import Qt3D.Render 2.0
import Voxels 1.0
import QtQuick.Dialogs 1.2

Item {
    id: app
    width: 1280
    height: 720

    focus: true

    function getPath(path) {
        return decodeURIComponent(path.toString().slice(7))
    }

    FileDialog {
        id: openDialog
        title: "Open"
        folder: shortcuts.home
        onAccepted: {
            geom.open(getPath(this.fileUrl))
            this.close()
        }
        onRejected: {
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
            geom.save(getPath(this.fileUrl))
            this.close()
        }
        onRejected: {
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: false
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
            this.close()
        }
        onRejected: {
            this.close()
        }
        Component.onCompleted: visible = false
        selectMultiple: true
        nameFilters: [ "Point files (*.pts)", "OcTree files (*.ot)" ]
    }

    FileDialog {
        id: exportDialog
        title: "Export as"
        folder: shortcuts.home
        onAccepted: {
            geom.exportModel(getPath(this.fileUrl))
            this.close()
        }
        onRejected: {
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
            this.close()
            geom.newProject()
        }
        onNo: this.close()
    }

    MessageDialog {
        id: discardAndOpenDialog
        title: "Discard changes?"
        icon: StandardIcon.Question
        text: "There are unsaved changes. Do you want to discard them and open a project?"
        standardButtons: StandardButton.Yes | StandardButton.No
        Component.onCompleted: visible = false
        onYes: {
            this.close()
            openDialog.open()
        }
        onNo: this.close()
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
        onNo: this.close()
    }

    MessageDialog {
        id: aboutDialog
        title: "About Farina"
        text: "Farina v0.1 is the first stable version of the Farina OcTree editor."
        onAccepted: this.close()
        Component.onCompleted: visible = false
    }

    MessageDialog {
        id: helpDialog
        title: "Help"
        text: "Helpful stuff will be displayed here at some point in the future"
        onAccepted: this.close()
        Component.onCompleted: visible = false
    }

    MenuBar {
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
                enabled: false
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
                enabled: false // TODO: add this functionality lol
            }
            Action {
                id: saveAction
                text: qsTr("&Save (Ctrl+S)")
                enabled: false
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
                enabled: false
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

    Geometry {
        id: geom
        position: Qt.vector3d(0, 0, -30)
        rotation: Qt.quaternion(0, 0, 0, 0)
    }

    // pretty bad, make it smooth or sth
    Keys.onUpPressed: geom.position.y += 0.1 // change to Z
    Keys.onDownPressed: geom.position.y -= 0.1 // change to Z
    Keys.onRightPressed: geom.position.x += 0.1
    Keys.onLeftPressed: geom.position.x -= 0.1

}