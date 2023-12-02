#include "widget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

QWidget*pmain;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "myShapez_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    a.setQuitOnLastWindowClosed(true);
    Widget w;
    pmain=&w;
    w.show();
    return a.exec();
}
