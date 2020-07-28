//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <QtWidgets/QApplication>
#include "view/WindowMain.h"
#include "presenter/presenter.h"
#include "model/Config.h"

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

    ConfigMgr conf_mgr("./prefs.ini");
    ConfigData conf_prefs = conf_mgr.getConfig();


    int img_width = 640;
    int img_heigth = 480;
    float solver_prior_pitch = conf_prefs.prior_pitch;
    float solver_prior_yaw = conf_prefs.prior_yaw;
    float solver_prior_distance = conf_prefs.prior_distance;

    std::wstring MODEL_DETECT_PATH = QString(std::string("./models/mnv3_detection_opt.onnx").data()).toStdWString();
    std::wstring MODEL_LANDMARK_PATH = QString(std::string("./models/mnv3_opt_b.onnx").data()).toStdWString();
    //std::wstring MODEL_DETECT_PATH = QString(std::string("C:\\Users\\Alvaro\\source\\repos\\Camera\\x64\\Release\\models\\mnv3_detection_opt.onnx").data()).toStdWString();
    //std::wstring MODEL_LANDMARK_PATH = QString(std::string("C:\\Users\\Alvaro\\source\\repos\\Camera\\x64\\Release\\models\\mnv3_opt_b.onnx").data()).toStdWString();


    PositionSolver solver = PositionSolver(img_width, img_heigth, solver_prior_pitch, solver_prior_yaw, solver_prior_distance);
    Tracker t = Tracker(&solver, MODEL_DETECT_PATH, MODEL_LANDMARK_PATH);

    Presenter p((IView&)w, &t, (ConfigMgr*)&conf_mgr);

    


    return app.exec();
}
