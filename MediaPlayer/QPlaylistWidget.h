#pragma once

#include <QListWidget>
#include <QMediaPlaylist>

class QPlaylistWidget : public QListWidget
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
public slots:
    void on_media_inserted(int start, int end);
    void on_media_removed(int start, int end);
    void on_itemDoubleClicked(QListWidgetItem* item);

signals:
    void itemDropped();
    void playlistChanged(const QMediaPlaylist& playlist);

private: 
    QMediaPlaylist* playlist;
};
