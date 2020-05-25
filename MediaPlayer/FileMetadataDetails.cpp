#include "FileMetadataDetails.h"

#include <QMediaMetaData>
#include <QTableWidgetItem>
#include <QTime>

FileMetadataDetails::FileMetadataDetails(const QMediaPlayer* player, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    this->player = player;

    if (player->isMetaDataAvailable()) {
        QString keys[] = { QMediaMetaData::Title, QMediaMetaData::Author, QMediaMetaData::Duration };
        for each (auto key in keys)
        {
            auto row_idx = ui.tableWidget->rowCount();
            ui.tableWidget->insertRow(row_idx);
            ui.tableWidget->setItem(row_idx, 0, new QTableWidgetItem(key));
            QString value;
            if (key == QMediaMetaData::Duration) {
                auto data = player->metaData(key);
                qint64 msecs = data.toLongLong();
                value = QTime::fromMSecsSinceStartOfDay(msecs).toString();
            }
            else {
                value = player->metaData(key).toString();
            }
            ui.tableWidget->setItem(row_idx, 1, new QTableWidgetItem(value));
        }
    }
}

FileMetadataDetails::~FileMetadataDetails()
{
}
