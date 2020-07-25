#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <QtWidgets/QApplication>
#include "view/WindowMain.h"
#include "presenter/presenter.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    /*QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;*/

    WindowMain w;
    w.show();

    Presenter c((IView&)w, nullptr);

    //c.run_loop();


    return app.exec();
}
