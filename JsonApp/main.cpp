#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
//#include <QJonArray>
#include <QJsonValue>

void sendRequest();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    sendRequest();
    return a.exec();
}

void sendRequest()
{
    QEventLoop eventLoop;

    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkRequest req(QUrl(QString("http://time.jsontest.com/")));
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        QString strReply = (QString)reply->readAll();
        qDebug() << "Response: " << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
        qDebug() << "Time: " << jsonObj["time"].toString();
        qDebug() << "Date: " << jsonObj["date"].toString();
        delete reply;
    }else
    {
        qDebug() << "Failure" << reply->errorString();
        delete reply;
    }
}

void writeJson(QString path)
{
    test t;
    t.fill();
    convert::writeJson(&t, path);
}

void readJson(QString path)
{
    test* t;
    t.fill();
    convert::writeJson(&t, path);
}

void readJson(QString path)
{
    test* t;
    t = converter::readJson(path);
    if(!t)
    {
        q
    }
}

QNetworkRequest req( QUrl( QString("https://jsonplaceholder.typicode.com/posts/1/comments") ) );
 QNetworkReply *reply = mgr.get(req);
 eventLoop.exec( );           // "finished( )" 가 호출 될때까지 블록

 if (reply->error( ) == QNetworkReply::NoError) {
     QString strReply = (QString)reply->readAll( );
     // Json 파싱
     qDebug( ) << "Response:" << strReply;

     QJsonDocument jsonResponse =
             QJsonDocument::fromJson(strReply.toUtf8( ));

     QJsonArray jsonArr = jsonResponse.array();
     for(int i = 0; i < jsonArr.size(); i++) {
         QJsonObject jsonObj = jsonArr.at(i).toObject();    //jsonResponse.object();
#if 1
         foreach(const QString& key, jsonObj.keys()) {
             QJsonValue value = jsonObj.value(key);
             qDebug() << "Key = " << key << ", Value = " << value.toString();
         }
#else
         qDebug( ) << "Name:" << jsonObj["name"].toString( );
         qDebug( ) << "Body:" << jsonObj["body"].toString( );
#endif
     }

     delete reply;
