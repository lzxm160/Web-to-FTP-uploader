#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <QtNetwork/QFtp>
#include <QHttpResponseHeader>
#include <QUrl>
#include <QHttp>
#include <QDialog>
#include <QRegExp>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QByteArray>
#include <QBuffer>
#include <QCursor>
#include <QTextStream>
#include <QVariant>

#include <Configuration.h>
#include <URLField.h>
#include <SaveButton.h>
#include <ProgressBar.h>
#include <StatusBar.h>
#include <EventFilter.h>


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
        void            httpResponseRecieved(QHttpResponseHeader);
        void            draged(QPoint);
        void            resized(void);

    protected:
        void            resizeEvent(QResizeEvent*);
        void            moveEvent(QMoveEvent*);
        void            closeEvent(QCloseEvent*);

    private:
        Configuration*  configuration;
        ImageURL*       urlField;
        SaveButton*     saveButton;
        UploadProgress* progressBar;
        StatusBar*      statusBar;
        EventFilter*    eventFilter;
        QFtp            ftpClient;
        QHttp           httpClient;
        QString         fileURL;
        QString         fileName;
        QBuffer         fileData;
        bool            fileExists;
        bool            appLocked;

        void            SetAppearance(void);
        void            ConnectToFTP(void);
        void            FormReset(void);
        void            FormLock(void);
        void            FormUnlock(void);
        void            MoveToCorner(void);
        void            FileNameIncrease(void);
        bool            FileExists(void);
        void            MagnetApply(void);
        void            MoveTo(QPoint);

};

#endif // IMAGESAVER_H
