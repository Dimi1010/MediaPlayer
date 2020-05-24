#pragma once

#include <array>

#include <QTableWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class QPlaylistWidget : public QTableWidget
{
    Q_OBJECT

public:
    QPlaylistWidget(QWidget *parent, QMediaPlaylist* playlist = nullptr);
    ~QPlaylistWidget();

    void setPlaylist(QMediaPlaylist* playlist);

protected:
    void dragMoveEvent(QDragMoveEvent* e) override;
    void dropEvent(QDropEvent* event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    Qt::DropActions supportedDropActions() const override;
    void updatePlayIndicator();

public slots:
    void on_playerStateChanged(QMediaPlayer::State state);

    void on_currentIndexChanged(int position);
    void on_mediaInserted(int start, int end);
    void on_mediaRemoved(int start, int end);
    void on_itemDoubleClicked(QTableWidgetItem* item);

signals:
    void itemDropped();
    void playlistChanged(const QMediaPlaylist& playlist);

private: 
    static constexpr int nonePlaying = -1;
    const std::array<QIcon, 3> playIcons;

    QMediaPlaylist* playlist;
    QMediaPlayer::State currentState = QMediaPlayer::State::StoppedState;
    int currentlyPlaying = nonePlaying;
};
