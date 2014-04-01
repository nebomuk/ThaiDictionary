#include "tts.h"

TTS::TTS(QObject *parent) :
    QObject(parent)
{
    array = NULL;
    voice_ = "en";
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(mediaObject, audioOutput);

    buffer = new QBuffer(this);

    http = new QHttp("translate.google.com");
    this->connect(http,SIGNAL(done(bool)),this,SLOT(audioDownloadFinished(bool)));
}

TTS::~TTS()
{
    delete array;
}

void TTS::play(const QString &word)
{
    QByteArray ContentData;
    ContentData += "ie=UTF-8&q=" + QUrl::toPercentEncoding(word) + "&tl=" + voice_;

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
void TTS::audioDownloadFinished(bool)
{
    delete array;
    array = new QByteArray(http->readAll());
    buffer->close();
    buffer->setBuffer(array);
    mediaObject->setCurrentSource(Phonon::MediaSource(buffer));
    mediaObject->play();
}
