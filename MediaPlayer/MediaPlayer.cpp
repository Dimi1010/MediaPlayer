#include "MediaPlayer.h"

#include <limits>

#include <QFileDialog>
#include <QMessageBox>

constexpr int slider_max_resolution = 10000;

MediaPlayer::MediaPlayer(QWidget *parent)
    : QMainWindow(parent), fileDetails(nullptr)
{
    ui.setupUi(this);

#pragma region CorePlayback Controls
    player = new QMediaPlayer(this);
    player->setVideoOutput(ui.videoWidget);

    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);
    ui.playlistWidget->setPlaylist(playlist);

    connect(ui.actionNext, &QAction::triggered, playlist, &QMediaPlaylist::next);
    connect(ui.actionPrevious, &QAction::triggered, playlist, &QMediaPlaylist::previous);
    connect(ui.playlistWidget, &QPlaylistWidget::itemDoubleClicked, this, &MediaPlayer::on_actionPlay_triggered);
#pragma endregion
    
#pragma region Volume Controls
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
#pragma endregion

#pragma region Seek Controls
    pos_slider = new QSlider(this);
    pos_slider->setOrientation(Qt::Horizontal);
    pos_slider->setMaximum(slider_max_resolution);
    ui.mainToolBar->addWidget(pos_slider);

    connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::on_playerPositionChanged);
    connect(pos_slider, &QSlider::sliderMoved, this, &MediaPlayer::on_sliderMoved);
#pragma endregion

#pragma region Playback Rate
    pbrate = new QLabel("Playback Rate", this);
    ui.statusBar->addPermanentWidget(pbrate);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayer::on_mediaStatusChanged);
    connect(player, &QMediaPlayer::playbackRateChanged, this, &MediaPlayer::on_playbackRateChanged);
#pragma endregion
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

void MediaPlayer::on_actionRepeatDefault_triggered()
{
    playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Sequential);
    ui.actionRepeatDefault->setChecked(true);
    ui.actionRepeatPlaylist->setChecked(false);
    ui.actionRepeatTrack->setChecked(false);
}

void MediaPlayer::on_actionRepeatPlaylist_triggered()
{
    playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    ui.actionRepeatDefault->setChecked(false);
    ui.actionRepeatPlaylist->setChecked(true);
    ui.actionRepeatTrack->setChecked(false);
}

void MediaPlayer::on_actionRepeatTrack_triggered()
{
    playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);
    ui.actionRepeatDefault->setChecked(false);
    ui.actionRepeatPlaylist->setChecked(false);
    ui.actionRepeatTrack->setChecked(true);
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
    if(this->fileDetails == nullptr) this->fileDetails = new FileMetadataDetails(this->player, this);
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
