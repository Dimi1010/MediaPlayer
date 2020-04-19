#pragma once

#include <QDialog>
#include <QMediaPlayer>

#include "ui_FileMetadataDetails.h"

class FileMetadataDetails : public QDialog
{
    Q_OBJECT

public:
    FileMetadataDetails(const QMediaPlayer* player, QWidget *parent = Q_NULLPTR);
    ~FileMetadataDetails();

private:
    Ui::FileMetadataDetails ui;

    const QMediaPlayer* player;
};
