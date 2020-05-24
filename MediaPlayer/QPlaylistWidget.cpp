#include "QPlaylistWidget.h"

#include <QDragEnterEvent>
#include <QMimeData>

#include <algorithm>

QPlaylistWidget::QPlaylistWidget(QWidget* parent, QMediaPlaylist* playlist)
    : QTableWidget(0, 2, parent), playlist(playlist),
    playIcons({ 
        QIcon(":/icons/stop.svg"),
        QIcon(":/icons/play.svg"),
        QIcon(":/icons/pause.svg")
    })
{
    this->setHorizontalHeaderLabels(QStringList({ QString("Playing"), QString("Filename") }));
    this->setIconSize(QSize(20, 20));
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setDragEnabled(true);
    this->setDefaultDropAction(Qt::CopyAction);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);

    if (this->playlist != nullptr) {
        connect(this->playlist, &QMediaPlaylist::currentIndexChanged, this, &QPlaylistWidget::on_currentIndexChanged);
        connect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_mediaInserted);
        connect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_mediaRemoved);
    }
    connect(this, &QPlaylistWidget::itemDoubleClicked, this, &QPlaylistWidget::on_itemDoubleClicked);
}

QPlaylistWidget::~QPlaylistWidget()
{
}

void QPlaylistWidget::setPlaylist(QMediaPlaylist* playlist)
{
    if (this->playlist != nullptr) {
        disconnect(this->playlist, &QMediaPlaylist::currentIndexChanged, this, &QPlaylistWidget::on_currentIndexChanged);
        disconnect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_mediaInserted);
        disconnect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_mediaRemoved);
    }
    this->playlist = playlist;
    connect(this->playlist, &QMediaPlaylist::currentIndexChanged, this, &QPlaylistWidget::on_currentIndexChanged);
    connect(this->playlist, &QMediaPlaylist::mediaInserted, this, &QPlaylistWidget::on_mediaInserted);
    connect(this->playlist, &QMediaPlaylist::mediaRemoved, this, &QPlaylistWidget::on_mediaRemoved);
    emit playlistChanged(*playlist);
}

void QPlaylistWidget::dragMoveEvent(QDragMoveEvent* e)
{
    //TODO: SoonTM
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
    //TODO: SoonTM
}

void QPlaylistWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept();
}

Qt::DropActions QPlaylistWidget::supportedDropActions() const
{
    return Qt::CopyAction;
}

void QPlaylistWidget::updatePlayIndicator()
{
    if (this->currentlyPlaying != this->nonePlaying) {
        this->item(this->currentlyPlaying, 0)->setIcon(this->playIcons[this->currentState]);
    }
}

void QPlaylistWidget::on_playerStateChanged(QMediaPlayer::State state) {
    this->currentState = state;
    this->updatePlayIndicator();
}

void QPlaylistWidget::on_currentIndexChanged(int position) {
    if(this->currentlyPlaying != this->nonePlaying)
        this->item(this->currentlyPlaying, 0)->setIcon(QIcon());
    this->currentlyPlaying = position;
    this->updatePlayIndicator();
}

void QPlaylistWidget::on_mediaInserted(int start, int end) {
    QList<QMediaContent> content;
    for (int i = start; i <= end; i++) {
        content.append(this->playlist->media(i));
    }
    QStringList names;
    std::transform(content.begin(), content.end(), std::back_inserter(names), [&](QMediaContent item) { return item.request().url().fileName(); });

    auto nameIt = names.cbegin();
    for (int row = start; row <= end && nameIt != names.cend(); row++, nameIt++) {
        this->insertRow(row);
        this->setItem(row, 0, new QTableWidgetItem());
        this->setItem(row, 1, new QTableWidgetItem(*nameIt));
    }
}

void QPlaylistWidget::on_mediaRemoved(int start, int end)
{
    for (int row = end; row >= start; row--) {
        this->removeRow(row);
    }
}

void QPlaylistWidget::on_itemDoubleClicked(QTableWidgetItem* item)
{
    playlist->setCurrentIndex(this->row(item));
}