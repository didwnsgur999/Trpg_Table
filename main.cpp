#include "mainwindow.h"
#include "backend.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <memory> // unique_ptr

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Trpg_Table_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //backend = uniqueptr로 관리됨 -> 프로그램당 하나.
    std::unique_ptr<Backend> backend = std::make_unique<Backend>();
    //backend의 rawpointer를 w에 준다.
    MainWindow w(backend.get());
    w.show();
    return a.exec();
}
