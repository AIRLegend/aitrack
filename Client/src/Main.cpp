#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !_DEBUG


#include <QtWidgets/QApplication>
#include "view/WindowMain.h"
#include "presenter/presenter.h"
#include "model/Config.h"
#include <omp.h>
#include "tracker/TrackerFactory.h"



int main(int argc, char *argv[])
{

    SetEnvironmentVariable(LPWSTR("OMP_NUM_THREADS"), LPWSTR("1"));
    omp_set_num_threads(1);  // Disable ONNX paralelization so we dont steal all cpu cores.
    omp_set_dynamic(0);

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    WindowMain w;
    w.show();

    auto conf_mgr = std::make_unique<ConfigMgr>("./prefs.ini");
    auto t_factory = std::make_unique<TrackerFactory>("./models/");

    Presenter p((IView&)w, std::move(t_factory), std::move(conf_mgr));

    return app.exec();
}
