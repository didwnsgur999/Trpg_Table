import QtQuick 6.0
import QtQuick.Window 6.0
import QtLocation 6.0 // Map 요소를 다루기 위함
import QtPositioning 6.0 // 좌표를 다루기 위함
import QtQuick.Controls 6.0 // 다양한 UI 위젯 제공 - 예. ScrollBar 사용을 위함
import QtQuick.Layouts 6.0 // 레이아웃 추가를 위함

Item {
    id: qmlRoot
    width: 1024 // 추후 위젯에서 적용해야 함
    height: 768
    visible: true


    // 전체 화면 구성 - 세로로 화면 배치 (ColumnLayout)
    ColumnLayout {
        anchors.fill: parent // 창에 꽉 채우기

        // 1. 상단 TRPG 정보&상태 바
        Rectangle{
            Layout.fillWidth: true // 전체 가로 꽉
            height: 80 // 높이
            color: "#333333"
            border.color: "lightgray"
            border.width: 1

            Text{
                text: "당신이 선택한 전장: 크로아"
                color: "white"
                font.pixelSize: 24
                anchors.centerIn: parent
            }

            // 전장 화면 보이도록 토글 버튼
            Button{
                id: toggleBattleMapButton
                text: "이세계 입장!"
                width: 120
                height: 40
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 20

                // 지도 화면과 전장화면을 서로 토글
                onClicked: {
                    realMap.visible = !realMap.visible;
                    battleMapLoader.visible = !battleMapLoader.visible;

                    if(battleMapLoader.visible){
                        toggleBattleMapButton.text = "실제 세계로"; // 버튼 바꾸기
                    } else {
                        toggleBattleMapButton.text = "이세계 입장!";
                    }
                }
            }
        }

        // 2. 중앙 지도 부분 (남은 공간 모두 차지)
        Map{
            id: realMap
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: true // 초기에는 실제 지도가 보이도록

            zoomLevel: 15 // 초기 확대 레벨
            center: QtPositioning.coordinate(37.579577, 127.003673) // 초기좌표(교육센터)

            plugin: Plugin{
                name: "osm" // OpenStreetMap 플러그인 사용 (하단에 지도 제공자 설정 추가 가능)
            }

            // 지도 위 마커 추가 (예: 주인공 위치)
            MapQuickItem{
                id: playerMarker
                anchorPoint.x: playerMarker.width/2
                anchorPoint.y: playerMarker.height // 핀의 하단이 좌표에 오도록
                coordinate: QtPositioning.coordinate(37.579577, 127.003673)

                sourceItem: Image{
                    source: "qrc:/Downloads/charEx.png" // 주인공 이미지
                    width: 40
                    height: 40
                }

                DragHandler{
                    id: dragHandler

                    onActiveChanged: {
                        if(active){
                            console.log("드래그 시작");
                        }
                        else{
                            console.log("드래그 끝");
                        }
                    }

                    onTranslationChanged: {
                        var pointOnMap = realMap.toCoordinate(Qt.point(dragHandler.translation.x + width/2, dragHandler.translation.y + height));
                        playerMarker.coordinate = pointOnMap;
                        console.log(pointOnMap.latitude + ", " + pointOnMap.longitude);
                    }
                }
            }
        }
        // 3. 이세계 입장! 버튼 클릭시 나타나는 전장 화면
        Loader{
            id: battleMapLoader
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: false // 버튼 클릭해야 보이도록

            source: "qrc:/battleMap1.qml"
        }
    }
}
