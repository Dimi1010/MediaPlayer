#include "MediaPlayer.h"

#include <limits>

#include <QFileDialog>
#include <QMessageBox>

constexpr int slider_max_resolution = 10000;

MediaPlayer::MediaPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    player = new QMediaPlayer(this);
    player->setVideoOutput(ui.videoWidget);

    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);

    vol_slider = new QSlider(this);
    vol_slider->setOrientation(Qt::Horizontal);
    vol_slider->setMaximum(100);
    vol_slider->setValue(20);
    vol_slider->setToolTip("20");
    vol_slider->setMaximumWidth(100);
    ui.mainToolBar->addWidget(vol_slider);

    ui.mainToolBar->addSeparator();

    connect(player, &QMediaPlayer::volumeChanged, vol_slider, &QSlider::setValue);
    connect(vol_slider, &QSlider::sliderMoved, player, &QMediaPlayer::setVolume);
    connect(vol_slider, &QSlider::sliderMoved, [=](const int &value) { vol_slider->setToolTip(QString::number(value)); });

    pos_slider = new QSlider(this);
    pos_slider->setOrientation(Qt::Horizontal);
    pos_slider->setMaximum(slider_max_resolution);
    ui.mainToolBar->addWidget(pos_slider);

    connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::on_playerPositionChanged);
    connect(pos_slider, &QSlider::sliderMoved, this, &MediaPlayer::on_sliderMoved);

    pbrate = new QLabel("Playback Rate", this);
    ui.statusBar->addPermanentWidget(pbrate);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayer::on_mediaStatusChanged);
    connect(player, &QMediaPlayer::playbackRateChanged, this, &MediaPlayer::on_playbackRateChanged);
}

void MediaPlayer::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", "", "Audio Files (*.mp3; *.flac);;Video Files (*.avi; *.mp4);;All Files (*.*)");
    if (!filename.size()) return;

    this->on_actionStop_triggered();
    playlist->clear();
    playlist->addMedia(QUrl::fromLocalFile(filename));
    this->on_actionPlay_triggered();
}


void MediaPlayer::on_actionPlay_triggered()
{
    player->setPlaybackRate(1);
    player->play();
    ui.statusBar->showMessage("Playing");
}

void MediaPlayer::on_actionPause_triggered()
{
    player->pause();
    ui.statusBar->showMessage("Paused");
}

void MediaPlayer::on_actionStop_triggered()
{
    player->stop();
    ui.statusBar->showMessage("Stopped");
}

void MediaPlayer::on_actionRepeat_triggered()
{
    if (playlist->playbackMode() != QMediaPlaylist::PlaybackMode::Loop) {
        playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    }
    else {
        playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Sequential);
    }
}

void MediaPlayer::on_actionFastForward_triggered()
{
    if (player->playbackRate() >= 0) {
        player->setPlaybackRate(player->playbackRate() * 2);
    }
    else {
        if (player->playbackRate() == -1) {
            player->setPlaybackRate(1);
        }
        else {
            player->setPlaybackRate(player->playbackRate() / 2);
        }
    }
}

void MediaPlayer::on_actionRewind_triggered()
{
    if (player->playbackRate() >= 0) {
        if (player->playbackRate() == 1) {
            player->setPlaybackRate(-1);
        }
        else {
            player->setPlaybackRate(player->playbackRate() / 2);
        }
    }
    else {
        player->setPlaybackRate(player->playbackRate() * 2);
    }
}

void MediaPlayer::on_actionFileDetails_triggered()
{
    this->fileDetails = new FileMetadataDetails(this->player, this);
    fileDetails->show();
}

void MediaPlayer::on_playbackRateChanged(qreal rate)
{
    QString str("Playback Rate: x");
    str += QString::number(rate);
    this->pbrate->setText(str);
}

void MediaPlayer::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::UnknownMediaStatus:
        break;
    case QMediaPlayer::NoMedia:
        break;
    case QMediaPlayer::LoadingMedia:
        break;
    case QMediaPlayer::LoadedMedia:
        break;
    case QMediaPlayer::StalledMedia:
        break;
    case QMediaPlayer::BufferingMedia:
        break;
    case QMediaPlayer::BufferedMedia:
        break;
    case QMediaPlayer::EndOfMedia:
        break;
    case QMediaPlayer::InvalidMedia:
        QMessageBox::critical(this, "Playback Error", "Media could not be played.\nFormat is unsupported.");
        break;
    default:
        break;
    }
}

void MediaPlayer::on_playerDurationChanged(qint64 duration)
{
    if (duration >= std::numeric_limits<int>::max() && 0) {
        scaled = true;
        pos_slider->setMaximum(slider_max_resolution);
    }
    else {
        scaled = false;
        pos_slider->setMaximum(duration);
    }
}

void MediaPlayer::on_playerPositionChanged(qint64 position)
{
    if (scaled) {
        if (player->duration() == 0) return;
        pos_slider->setValue(static_cast<int>((position * slider_max_resolution) / player->duration()));
    }
    else {
        pos_slider->setValue(position);
    }
}

void MediaPlayer::on_sliderMoved(int position)
{
    if (scaled) {
        auto new_pos = static_cast<qint64>((player->duration() * position) / slider_max_resolution);
        player->setPosition(new_pos);
    }
    else {
        player->setPosition(static_cast<qint64>(position));
    }
}
