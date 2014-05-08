import QtQuick 1.1
import QtWebKit 1.0

Flipable {
    id: flipable
    width: web.width
    height: 300

    property bool flipped: false
    property string urlVar: "http://h.fs-et.de/1240"

    front:
      Rectangle{
      width:web.width
      height: parent.height

        Image {
        id: settings
        width: 40; height: 40
        source: "config.png"
        x: parent.width-width
        z:5
        y: parent.height-height
          //anchors.bottom: flipable.anchors.bottom
              MouseArea {
                  anchors.fill: parent
                  onPressed: {
                  }
                  onReleased: {
                    flipable.flipped = !flipable.flipped
                  }
              }
        }

        Flickable {
          id:flicker
        width: web.width
        height: parent.height
        contentWidth: web.width;
        contentHeight: web.height


          WebView {
              id: web
              url: urlVar
              preferredWidth: 250
              preferredHeight: 700
              scale: 1
              smooth: false
          }
        }
      }


    Component {
        id: webDelegate
        Item {
            width: web.width; height: 35
            Column {
              spacing: 5
              width: parent.width
              Text {
                  width: parent.width
                  id: nameText
                  horizontalAlignment: Text.AlignHCenter
                  text: name
                  font.family: "Helvetica"
                  font.pointSize: 24
                  color: "grey"
                }
            }
            MouseArea {
                anchors.fill: parent
                onPressed: {
                  nameText.color = "black"
                }
                onReleased: {
                  nameText.color = "grey"
                  urlVar = uri
                  flipable.flipped = !flipable.flipped
                  flicker.contentY=0
                }
            }

        }
    }


    back: ListView {
        width: web.width
        height: 300
        model: station
        delegate: webDelegate
    }

    transform: Rotation {
        id: rotation
        origin.x: flipable.width/2
        origin.y: flipable.height/2
        axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
        angle: 0    // the default angle
    }

    states: State {
        name: "back"
        PropertyChanges { target: rotation; angle: 180 }
        when: flipable.flipped
    }

    transitions: Transition {
        NumberAnimation { target: rotation; property: "angle"; duration: 500}
    }


    ListModel {
      id:station
        ListElement {
            name: "Universität Süd"
            uri: "http://h.fs-et.de/1240"
        }
        ListElement {
            name: "Theater"
            uri: "http://h.fs-et.de/1010"
        }
        ListElement {
            name: "Ehinger Tor"
            uri: "http://h.fs-et.de/1350"
        }
        ListElement {
            name: "Hauptbahnhof"
            uri: "http://h.fs-et.de/1008"
        }
    }
}
