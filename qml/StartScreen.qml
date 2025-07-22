import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    anchors.fill: parent

    // 배경 이미지
    Image {
        id: background
        source: "qrc:/image/background/tavern.jpg"
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }

    // 어둡게 오버레이
    Rectangle {
        anchors.fill: parent
        color: "#88000000"
    }

    // 로고
    Image {
        id: logo
        source: "qrc:/image/background/logo.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.1
        width: parent.width * 0.8
        height: width * 0.3
        fillMode: Image.PreserveAspectFit
        z: 1
    }

    // 전체 배경 위 sweep 효과
    Rectangle {
        id: screenSweep
        visible: true
        width: parent.width
        height: parent.height * 3
        rotation: 30
        transformOrigin: Item.Center
        anchors.verticalCenter: parent.verticalCenter
        y: -height * 0.4
        z: 2

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00FFFFFF" }   // 완전 투명
            GradientStop { position: 0.3; color: "#11FFFFFF" }   // 약한 흰색 (6% 투명도)
            GradientStop { position: 0.5; color: "#22FFFFFF" }   // 중간 (13% 불투명도)
            GradientStop { position: 0.7; color: "#11FFFFFF" }
            GradientStop { position: 1.0; color: "#00FFFFFF" }
        }
        opacity: 0.0

        // 자연스러운 흐름을 위한 애니메이션
        SequentialAnimation {
            running: true
            loops: Animation.Infinite

            ParallelAnimation {
                PropertyAnimation {
                    target: screenSweep
                    property: "x"
                    from: -screenSweep.width
                    to: parent.width
                    duration: 4000
                    easing.type: Easing.InOutQuad
                }
                SequentialAnimation {
                    PropertyAnimation { target: screenSweep; property: "opacity"; from: 0.0; to: 0.3; duration: 400 }
                    PauseAnimation { duration: 3200 }
                    PropertyAnimation { target: screenSweep; property: "opacity"; from: 0.6; to: 0.0; duration: 400 }
                }
            }

            PauseAnimation { duration: 1000 } // 각 스윕 사이 여유
        }
    }
}
