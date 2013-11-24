#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "sortfilterproxymodel.h"

#include <QMainWindow>
#include <QClipboard>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QHttp>
#include <QBuffer>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void clipboardChanged(QClipboard::Mode mode);
    void updateView();
    void startUpdateViewTimer(const QString &text);
    void checkBoxStateChanged(int state);
    void play();
    void audioDownloadFinished(bool);
private:
    QAbstractItemModel *modelFromFile(const QString &fileName);

    Ui::MainWindow *ui;
    QClipboard *clipboard;
    void dataFromFile(const QString &fileName);
    SortFilterProxyModel *model;
    QTimer *timer;
    QString currentSearchText;
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    QHttp *http;
    QByteArray * array;
    QBuffer * buffer;
};

#endif // MAINWINDOW_H
