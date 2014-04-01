#ifndef TTS_H
#define TTS_H

#include <QBuffer>
#include <QObject>
#include <QHttp>
#include <QBuffer>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>


/**
 * @brief The TTS class uses google text to speech to play a word or a phrase
 * use setVoice to set the language via the two letter code
 */

class TTS : public QObject
{
    Q_OBJECT
public:
    TTS(QObject *parent = 0);
    void setVoice( const QString& voice){ voice_ = voice; }
    const QString& voice() const { return voice_; }
    ~TTS();
    
signals:
    
public slots:
    void play(const QString& word);

private slots:
    void audioDownloadFinished(bool);

private:
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    QHttp *http;
    QByteArray * array;
    QBuffer * buffer;
    QString voice_;
};

#endif // TTS_H
