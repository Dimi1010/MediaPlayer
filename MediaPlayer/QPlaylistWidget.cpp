#include "QPlaylistWidget.h"

#include <QDragEnterEvent>
#include <QMimeData>

#include <algorithm>

QPlaylistWidget::QPlaylistWidget(QWidget* parent, QMediaPlaylist* playlist)
    : QListWidget(parent), playlist(playlist)
{
    this->setViewMode(QListView::ListMode);
    this->setIconSize(QSize(55, 55));
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragEnabled(true);
    this->setDefaultDropAction(Qt::CopyAction);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);

    connect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_media_inserted);
    connect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_media_removed);
    connect(this, &QPlaylistWidget::itemDoubleClicked, this, &QPlaylistWidget::on_itemDoubleClicked);
}

QPlaylistWidget::~QPlaylistWidget()
{
}

void QPlaylistWidget::setPlaylist(QMediaPlaylist* playlist)
{
    disconnect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_media_inserted);
    disconnect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_media_removed);
    this->playlist = playlist;
    connect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_media_inserted);
    connect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_media_removed);
    emit playlistChanged(*playlist);
}

void QPlaylistWidget::dragMoveEvent(QDragMoveEvent* e)
{
}

void QPlaylistWidget::dropEvent(QDropEvent* event)
{
    event->accept();
    event->setDropAction(Qt::CopyAction);
    auto urls = event->mimeData()->urls();
    QList<QMediaContent> content;
    std::transform(urls.begin(), urls.end(), std::back_inserter(content), [&](QUrl url) { return QMediaContent(url); });
    this->playlist->addMedia(content);
}

void QPlaylistWidget::startDrag(Qt::DropActions supportedActions)
{
}

void QPlaylistWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept();
}

Qt::DropActions QPlaylistWidget::supportedDropActions() const
{
    return Qt::CopyAction;
}

void QPlaylistWidget::on_media_inserted(int start, int end) {
    QList<QMediaContent> content;
    for (int i = start; i <= end; i++) {
        content.append(this->playlist->media(i));
    }
    QStringList names;
    std::transform(content.begin(), content.end(), std::back_inserter(names), [&](QMediaContent item) { return item.request().url().fileName(); });

    this->insertItems(start, names);
}

void QPlaylistWidget::on_media_removed(int start, int end)
{
    for (int i = end; i >= start; i--) {
        auto item = this->takeItem(i);
        delete item;
    }
}

void QPlaylistWidget::on_itemDoubleClicked(QListWidgetItem* item)
{
    playlist->setCurrentIndex(this->row(item));
}
