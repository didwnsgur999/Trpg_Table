#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout> // 위젯 레이아웃 관리를 위한
#include <QDebug> // 디버깅 메시지 위한

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFrame *mapContainer = ui->centralwidget->findChild<QFrame*>("mapContainer"); // ui내에서 mapContainer는 centralWidget의 자식

    if(mapContainer){
        // QML 맵 호스팅할 QQuickWidget 생성
        m_quickWidget = new QQuickWidget(this); // 부모 위젯이 MainWindow
        m_quickWidget->setSource(QUrl(QStringLiteral("qrc:/qml/qml/main.qml")));
        m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView); // QML 루트 객체가 뷰 크기에 맞춰지도록 설정

        // QML 맵 배치할 컨테이너 위젯 찾기
        QVBoxLayout *layout = new QVBoxLayout(mapContainer);
        layout->setContentsMargins(0, 0, 0, 0); // 여백 제거
        layout->addWidget(m_quickWidget); // QQuickWidget을 레이아웃에 추가

        qDebug() << "QML Map integrated successfully into mapContainer!";
    }
    else {
        qDebug() << "Error - QFrame doesn't exist.";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
