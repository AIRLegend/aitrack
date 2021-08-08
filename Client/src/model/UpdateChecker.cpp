#include "UpdateChecker.h"


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <QObject>
#include <QDebug>


UpdateChecker::UpdateChecker(std::string& version, IUpdateSub *obs):
    request(),
    manager(),
    current_version(version)
{
    this->observer = obs;
}

void UpdateChecker::callback(QNetworkReply* reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    QString answer = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
    QJsonArray json_array = doc.array();
    QJsonObject latest_update = json_array[0].toObject();

    Version v(latest_update["tag_name"].toString().toStdString());

    qDebug() << latest_update["tag_name"].toString();

    this->observer->on_update_check_completed((current_version < v));
}


void UpdateChecker::get_latest_update(std::string& repo)
{
    QObject::connect(
        &manager,
        SIGNAL(finished(QNetworkReply*)), 
        this, 
        SLOT(callback(QNetworkReply*))
    );

    std::cout << "   REQUEST   "<<std::endl;

    QString url = QString("https://api.github.com/repos/%1/releases").arg(QString::fromStdString(repo));
    request.setUrl(QUrl(url));
    manager.get(request);
}
