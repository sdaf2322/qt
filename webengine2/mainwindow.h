#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QLineEdit>

class WebBrowser : public QMainWindow
{
    Q_OBJECT

public:
    WebBrowser(QWidget *parent = nullptr);
    ~WebBrowser();

private:
    QLineEdit *lineEditURL;
    QWebEngineView *webEngineView;
public slots:
    void goSlot();
    void goBackwardSlot();
    void goForwardSlot();
    void goHomeSlot();

};
#endif // MAINWINDOW_H
