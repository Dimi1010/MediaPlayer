#pragma once

#include <QtWidgets/QMainWindow>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QSlider>
#include <QLabel>

#include "ui_MediaPlayer.h"

#include "FileMetadataDetails.h"

class MediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    MediaPlayer(QWidget *parent = Q_NULLPTR);

private slots:
    void on_actionOpen_triggered();

    void on_actionPlay_triggered();

    void on_actionPause_triggered();

    void on_actionStop_triggered();

    void on_actionRepeat_triggered();

    void on_actionFastForward_triggered();

    void on_actionRewind_triggered();

    void on_actionFileDetails_triggered();

    void on_playbackRateChanged(qreal rate);

    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);

    void on_playerDurationChanged(qint64 duration);

    void on_playerPositionChanged(qint64 position);

    void on_sliderMoved(int position);


private:
    Ui::MediaPlayerClass ui;

    FileMetadataDetails* fileDetails;

    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QSlider* vol_slider;
    QSlider* pos_slider;
    QLabel* pbrate;

    bool scaled = true;
};
