#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <QtNetwork/QFtp>
#include <QUrl>
#include <QUrlInfo>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QFile>
#include <QDialog>
#include <QRegExp>
#include <QMouseEvent>
#include "ImageURL.h"
#include "SaveButton.h"
#include "UploadProgress.h"
#include "StatusBar.h"
#include <iostream>
#include <QApplication>
#include <QDesktopWidget>
#include "EventFilter.h"
#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <QCursor>
#include <QTextStream>
#include <QDir>
#include <QVariant>

class ImageSaver : public QWidget
{

    Q_OBJECT

    public:
                        ImageSaver ();

    public slots:
        void            fileSave(void);
        void            ftpStateChanged(int);
        void            ftpDataTransfer(qint64,qint64);
        void            ftpRequestFinished(int, bool);
        void            ftpListInfo(QUrlInfo);
        void            httpDataTransfer(int,int);
        void            httpRequestFinished(int,bool);
        void            draged(QPoint);
        void            resized(void);

    protected:
        void            closeEvent(QCloseEvent*);

    private:
        ImageURL*       url;
        SaveButton*     save;
        UploadProgress* progress;
        StatusBar*      status;
        EventFilter*    eventFilter;
        QFtp            ftp;
        QHttp           http;
        bool            fileExists;
        bool            appLocked;
        QString         fileURL;
        QString         fileName;
        QBuffer         fileData;
        QCursor         cursor;

        void            SetAppearance(void);
        void            ConnectToFTP(void);
        void            AppReset(void);
        void            AppLock(void);
        void            AppUnlock(void);
        void            MoveToCorner(void);
        void            FileNameIncrease(void);
        bool            FileExists(void);
        bool            ConfigLoad(void);

};


#endif // IMAGESAVER_H
