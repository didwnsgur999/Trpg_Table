import QtQuick 6.0

Rectangle {
    width: parent.width
    height: parent.height

    color: "#252525" // 이미지 없는 동안 전장화면 배경

    Image{
        id: battleBackground
        source: "qrc:/Downloads/mapEx.png"
        anchors.fill: parent // 부모(현재 qml) Rectangle에 곽 채우도록
        fillMode: Image.PreserveAspectFit // 이미지 비율 유지하면서 채우기
    }
}
