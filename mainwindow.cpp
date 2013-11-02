#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QStandardItemModel>
#include <QTextStream>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("ThaiDictionary");

    model = new SortFilterProxyModel(this);
    model->setFilterKeyColumn(0);
    model->setSourceModel(this->modelFromFile(":/resources/thaiengdict.txt"));
    ui->tableView->setModel(model);

    ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false); // hide row numbers

    clipboard = QApplication::clipboard();
    this->connect(clipboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(clipboardChanged(QClipboard::Mode)));

    this->connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChanged(int)));
}

/*
 * the text is either directly send to updateView when the clipboard mode is active or send via a signal from
 *  the lineEdit when the clipboard mode is disabled
 **/
void MainWindow::checkBoxStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        connect(clipboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(clipboardChanged(QClipboard::Mode)));
        disconnect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateView()));
    }
    else
    {
        connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateView(QString)));
        disconnect(clipboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(clipboardChanged(QClipboard::Mode)));
    }
}

void MainWindow::clipboardChanged(QClipboard::Mode mode)
{
    if(mode != QClipboard::Selection)
        return;
    QString text = clipboard->text(QClipboard::Selection).trimmed();
    ui->lineEdit->setText(text);

    updateView(text);
}

void MainWindow::updateView(const QString& text)
{
    if(text.isEmpty()) // would load complete dictionary
        return;


    model->setFilterStartsWith(text);
    model->invalidate();
    if(text.length() >1)
    {
        // slower for more rows
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    }
}


QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    Q_ASSERT(file.open(QFile::ReadOnly));

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QStringList> lines;

    QTextStream in(&file);
    in.setCodec("UTF-8");

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList row = line.split(QChar('\t'));
            QString& rowLast = row.last();

            const int maxLength = 48;
            if(rowLast.length() > maxLength)
            { // QRegExp("[\s\\;]")
                int index = rowLast.indexOf(QRegExp("[\\ \\;]"),maxLength);
                if(index != -1)
                {
                    rowLast.at(index) == ';' ? rowLast.insert(index+1,"\n") : rowLast.replace(index,1,'\n');

                    int lastLineBreak = index+1;
                    int remainingLength = rowLast.length() - index;
                    while(remainingLength > maxLength)
                    {
                        int index1 = rowLast.indexOf(QRegExp("[\\ \\;]"),lastLineBreak+1);

                        if(index1 == -1)
                        {
                            break;
                        }

                        rowLast.insert(index1+1,'\n');
                        lastLineBreak = index1+1;
                        remainingLength = rowLast.length() - index1;
                    }
                }
            }

            lines << (QStringList()<< row.first() << rowLast);// skip middle item
            //lines << (QStringList() << line.left(line.indexOf(QChar('['))) << line.right(line.length() - line.indexOf(QChar(']'))-1));

        }

        const int columnCount = 2;
    QStandardItemModel *m = new QStandardItemModel(lines.count(), columnCount, this);

    for (int i = 0; i < lines.count(); ++i) {

        // distribute the words on the columns from left to right
        for(int j = 0; j< columnCount; ++j)
        {
            QModelIndex wordIdx = m->index(i, j);
            QString word = lines[i][j]; //[i] gives the QStringList and [j] gives the QString
            m->setData(wordIdx, word);
        }
    }

    QApplication::restoreOverrideCursor();

    return m;
}

MainWindow::~MainWindow()
{
    delete ui;
}
























//void MainWindow::dataFromFile(const QString& fileName)
//{
//    QFile file(fileName);
//    Q_ASSERT(file.open(QFile::ReadOnly));


//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

//    QTextStream in(&file);
//    in.setCodec("UTF-8");

//        while (!in.atEnd())
//        {
//            QString line = in.readLine();
//            QStringList row = line.split(QChar('\t'));
//            QString& rowLast = row.last();

//            const int maxLength = 48;
//            if(rowLast.length() > maxLength)
//            { // QRegExp("[\s\\;]")
//                int index = rowLast.indexOf(QRegExp("[\\ \\;]"),maxLength);
//                if(index != -1)
//                {
//                    rowLast.at(index) == ';' ? rowLast.insert(index+1,"\n") : rowLast.replace(index,1,'\n');

//                    int lastLineBreak = index+1;
//                    int remainingLength = rowLast.length() - index;
//                    while(remainingLength > maxLength)
//                    {
//                        int index1 = rowLast.indexOf(QRegExp("[\\ \\;]"),lastLineBreak+1);

//                        if(index1 == -1)
//                        {
//                            break;
//                        }

//                        rowLast.insert(index1+1,'\n');
//                        lastLineBreak = index1+1;
//                        remainingLength = rowLast.length() - index1;
//                    }
//                }
//            }
//            data.insert(row.first(), rowLast);
//            //lines << (QStringList() << line.left(line.indexOf(QChar('['))) << line.right(line.length() - line.indexOf(QChar(']'))-1));
//        }


//      QApplication::restoreOverrideCursor();
//}
