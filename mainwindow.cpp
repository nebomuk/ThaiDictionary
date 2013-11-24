#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardItemModel>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); 

    model = new SortFilterProxyModel(this);
    model->setFilterKeyColumn(0);
    model->setSourceModel(this->modelFromFile(":/resources/thaiengdict.txt"));
    ui->tableView->setModel(model);

    ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false); // hide row numbers


    this->setWindowTitle(qApp->applicationName());

    timer = new QTimer(this);
    timer->setInterval(50);
    timer->setSingleShot(true);

    clipboard = QApplication::clipboard();

    QSettings settings(qApp->organizationName(),qApp->applicationName());
    this->restoreGeometry(settings.value("geometry",QByteArray()).toByteArray());

    Qt::CheckState checkState = static_cast<Qt::CheckState>(settings.value("checkBox", Qt::Checked).toInt());
    ui->checkBox->setCheckState(checkState);
    this->checkBoxStateChanged(checkState); // the corresponding signal is not automatically called

    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(mediaObject, audioOutput);

    buffer = new QBuffer(this);
    array = 0;

    http = new QHttp("translate.google.com");
    this->connect(http,SIGNAL(done(bool)),this,SLOT(audioDownloadFinished(bool)));

    this->connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(checkBoxStateChanged(int)));
    connect(timer,SIGNAL(timeout()),this,SLOT(updateView()));

    this->connect(ui->playButton,SIGNAL(clicked()),this,SLOT(play()));

    ui->lineEdit->setFocus();
}

/**
 * the text is either directly send to updateView when the clipboard mode is active or send via a signal from
 *  the lineEdit when the clipboard mode is disabled
 **/
void MainWindow::checkBoxStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        connect(clipboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(clipboardChanged(QClipboard::Mode)));
        disconnect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(startUpdateViewTimer(QString)));
    }
    else
    {
        connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(startUpdateViewTimer(QString)));
        disconnect(clipboard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(clipboardChanged(QClipboard::Mode)));
    }
}

/**
 * @brief MainWindow::play
 *
 * selects the selected row's first column's entry and sends a http download reqest to google translate's tts engine
 */
void MainWindow::play()
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QList<QModelIndex> list = selectionModel->selectedIndexes();
    if(list.isEmpty())
        return;

    QModelIndex index = model->index(list.first().row(),0);

    QString voice = "th";

    QByteArray ContentData;
    ContentData += "ie=UTF-8&q=" + QUrl::toPercentEncoding(model->data(index).toString()) + "&tl=" + voice;

    QHttpRequestHeader Header;
    Header.addValue("Host", "translate.google.com");

    Header.setContentLength(ContentData.length());
    Header.setRequest("POST", "/translate_tts", 1, 1);

    http->request(Header, ContentData);
}

/**
 * @brief MainWindow::audioDownloadFinished
 * plays the mp3 audio file via phonon after it has been downloaded
 */
void MainWindow::audioDownloadFinished(bool)
{
    delete array;
    array = new QByteArray(http->readAll());
    buffer->close();
    buffer->setBuffer(array);
    mediaObject->setCurrentSource(Phonon::MediaSource(buffer));
    mediaObject->play();
}


void MainWindow::clipboardChanged(QClipboard::Mode mode)
{
    if(mode != QClipboard::Selection)
        return;
    QString text = clipboard->text(QClipboard::Selection).trimmed();
    ui->lineEdit->setText(text);

    startUpdateViewTimer(text);
}



void MainWindow::updateView()
{
    if(currentSearchText.isEmpty()) // would load complete dictionary
        return;

    model->setFilterStartsWith(currentSearchText);
    model->invalidate();
    if(currentSearchText.length() >1)
    {
        // slower for more rows
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    }
}

/**
  * the timer calls updateView a few ms after the last keypress to avoid overloading the gui thread with calls to updateView()
  * when multiple key events are fired in rapid succession
 */
void MainWindow::startUpdateViewTimer(const QString &text)
{
    currentSearchText = text;
    timer->stop();
    timer->start();
}


QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug("Error in MainWindow::modelFromFile : could not open file");
    }

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

void MainWindow::closeEvent(QCloseEvent *event)
 {
     QSettings settings(qApp->organizationName(),qApp->applicationName());
     settings.setValue("geometry", saveGeometry());
     settings.setValue("checkBox", ui->checkBox->checkState());
     QMainWindow::closeEvent(event);
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
