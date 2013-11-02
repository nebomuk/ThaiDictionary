#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "sortfilterproxymodel.h"

#include <QMainWindow>
#include <QClipboard>
#include <QSortFilterProxyModel>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void clipboardChanged(QClipboard::Mode mode);
    void updateView(const QString &text);
    void checkBoxStateChanged(int state);
private:
    Ui::MainWindow *ui;
    QClipboard *clipboard;
    void dataFromFile(const QString &fileName);
    SortFilterProxyModel *model;
    QAbstractItemModel *modelFromFile(const QString &fileName);
};

#endif // MAINWINDOW_H
