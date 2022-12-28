#include "mainwindow.h"
#include "saveurls.h"
#include <QtWidgets>
//#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QToolBar *toolbar = addToolBar(tr("Open"));

    /* RSS 피드를 열기 위한 콤보박스 */
    combo = new QComboBox;
    ListView *lv = new ListView;
    combo->setView(lv);
    combo->setEditable(true);
    combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(combo, SIGNAL(activated(int)), SLOT(openRssFeed( )));
    toolbar->addWidget(combo);

    /* RSS 피드 버튼을 위한 액션 */
    QAction *act = new QAction(tr("Open RSS Feed"), this);
    act->setIcon(style( )->standardIcon(QStyle::SP_BrowserReload));
    connect(act, SIGNAL(triggered( )), SLOT(openRssFeed( )));
    toolbar->addAction(act);

    QLibrary lib("SaveUrls");
    if(!lib.isLoaded())lib.load();
    SaveURLS* saveUrl = (SaveURLS*)lib.resolve("SaveUrls");
    QStringList urlList = saveUrl->load();
    for(int i = 0; i < urlList.count(); i++)
        combo->addItem(urlList.at(i));
    if(lib.isLoaded())lib.unload();
    if(urlList.count()==0)
        combo->addItem("http://rss.joins.com/joins_news_list.xml");


    /* 기본 RSS 피드 사이트 등록 */
    combo->addItem("http://news.google.co.kr/news?cf=all&hl=ko&output=rss");

    /* 저장된 RSS 데이터 표시 */
    QSettings settings;
    int size = settings.beginReadArray("rssFeeds");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QString url = settings.value("url").toString( );
        combo->addItem(url);
    }

    settings.endArray( );
    /* 콤보박스가 비어있는 경우의 기본값 설정 */
    if (size == 0)
         combo->addItem("http://news.google.co.kr/news?cf=all&hl=ko&output=rss");

    /* 네트워크 매니저를 위한 설정 */
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(replyFinished(QNetworkReply*)));
}

MainWindow::~MainWindow( )
{
    /* RSS 피드 리스트 저장하기 */
    QLibrary lib("SaveUrls");
    if(!lib.isLoaded())lib.load();
    SaveURLS* saveUrl = (SaveURLS*)lib.resolve("SaveUrls");
    QStringList urlList;
    for(int i = 0; i < combo->count(); i++)
        urlList.append(combo->itemText(i));
    saveUrl->save(urlList);
    if(lib.isLoaded())lib.unload();
}
//    QSettings settings;
//    settings.beginWriteArray("rssFeeds");
//    settings.remove("");
//    for (int i = 0; i < combo->count( ); i++) {
//        settings.setArrayIndex(i);
//        settings.setValue("url", combo->itemText(i));
//    }
//    QSettings.endArray( );
//}

void MainWindow::openRssFeed( )
{
    /* 콤보박스에서 동일한 항목이 있는지 검사 */
    int i = combo->findText(combo->currentText( ));
    if (i != -1) {
        combo->setCurrentIndex(i);
    } else {
        combo->addItem(combo->currentText( ));
        combo->setCurrentIndex(combo->count( ) - 1);
    }

    /*  RSS 사이트 접속 */
    manager->get(QNetworkRequest(QUrl(combo->currentText( ))));
}

void MainWindow::replyFinished(QNetworkReply *netReply)
{
    QString str(netReply->readAll( ));
    qDebug("%s", qPrintable(str));
}

/* QListView에서 삭제 키 사용 설정 */
void ListView::keyPressEvent(QKeyEvent *event)
{
    if (event->key( ) == Qt::Key_Delete) {
        event->accept( );
        QModelIndexList l = selectedIndexes( );
        if (l.length( ) > 0) {
            model( )->removeRow(l.at(0).row( ), l.at(0).parent( ));
        }
    } else {
        QListView::keyPressEvent(event);
    }
}
