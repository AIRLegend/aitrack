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
   
    omp_set_num_threads(1);  // Disable ONNX paralelization so we dont steal all cpu cores.
    omp_set_dynamic(0);

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    WindowMain w;
    w.show();

    ConfigMgr conf_mgr("./prefs.ini");
    TrackerFactory t_factory("./models/");
    
    Presenter p((IView&)w, &t_factory, (ConfigMgr*)&conf_mgr);

    return app.exec();
}
