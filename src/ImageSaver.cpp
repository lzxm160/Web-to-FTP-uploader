#include "ImageSaver.h"

            ImageSaver::ImageSaver()
                :   QWidget(NULL,Qt::WindowStaysOnTopHint|Qt::CustomizeWindowHint),
                    fileExists(false),
                    appLocked(false)
{

    // Loading the config file
    if (ConfigLoad())
    {
        //  URL field
        url = new ImageURL(this);

        //  Save button
        save = new SaveButton(this);
        QObject::connect(save,SIGNAL(clicked()),this,SLOT(fileSave()));
        QObject::connect(save,SIGNAL(dragged(QPoint)),this,SLOT(draged(QPoint)));

        //  Progress bar
        progress = new UploadProgress(this);
        QObject::connect(progress,SIGNAL(dragged(QPoint)),this,SLOT(draged(QPoint)));

        //  Status bar
        status = new StatusBar(this);
        QObject::connect(status,SIGNAL(dragged(QPoint)),this,SLOT(draged(QPoint)));

        //  Installing event filter
        eventFilter = new EventFilter();
        installEventFilter(eventFilter);
        QObject::connect(eventFilter,SIGNAL(resized()),this,SLOT(resized()));

        //  Setting appearance
        SetAppearance();

        //  Connecting to FTP
        ConnectToFTP();

        // FTP signals
        QObject::connect(&ftp,SIGNAL(stateChanged(int)),this,SLOT(ftpStateChanged(int)));
        QObject::connect(&ftp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(ftpDataTransfer(qint64,qint64)));
        QObject::connect(&ftp,SIGNAL(commandFinished(int,bool)),this,SLOT(ftpRequestFinished(int,bool)));
        QObject::connect(&ftp,SIGNAL(listInfo(QUrlInfo)),this,SLOT(ftpListInfo(QUrlInfo)));

        //  HTTP signals
        QObject::connect(&http,SIGNAL(requestFinished(int,bool)),this,SLOT(httpRequestFinished(int,bool)));
        QObject::connect(&http,SIGNAL(dataReadProgress(int,int)),this,SLOT(httpDataTransfer(int,int)));

    }

}



void        ImageSaver::fileSave(void)
{

    //  Locking the form
    AppLock();

    //  Changing the status
    status->setText("Downloading...");

    //  If the URL is valid
    QUrl url(this->url->text());

    if ( url.isValid() )
    {

        //  Saving file name
        fileName = url.path().section('/',-1);
        if ( ! fileName.contains('.') )
        {
            fileName.append(".jpg");
        }

        //  Starting download
        http.setHost(url.host());
        http.get(url.path(),&fileData);

    }

    //  The URL is not valid
    else
    {

        status->setText("The URL is not valid.");
        AppReset();

    }

}

void        ImageSaver::ftpStateChanged(int state)
{

    switch (state)
    {
        case 0:
            status->setText("FTP connection lost");
            AppLock();
            ConnectToFTP();
            break;
        case 1:
            status->setText("Host name lookup...");
            break;
        case 2:
            status->setText("Connecting to FTP...");
            break;
        case 3:
            this->status->setText("Connected to FTP");
            break;
        case 4:
            status->setText("Ready");
            AppUnlock();
            break;
        case 5:
            status->setText("FTP connection closing...");
            break;
    }

}

void        ImageSaver::ftpDataTransfer(qint64 bytesTransfered, qint64 bytesTotal)
{

    progress->setMaximum(bytesTotal);
    progress->setValue(bytesTotal - bytesTransfered);
    //qDebug() << "upload" << bytesTotal << bytesTransfered ;

}

void        ImageSaver::ftpRequestFinished(int command, bool error )
{

    QFtp::Command commandFinished = ftp.currentCommand();

    if (commandFinished == QFtp::Put)
    {

        if ( !error )
        {

            status->setText("Saved");
            fileData.open(QIODevice::Truncate);
            fileData.close();
            AppReset();

        }
        else
        {

            QString errorMessage("Upload error #");
            QFtp::Error errorCode = ftp.error();
            errorMessage.append(QString::number(errorCode));
            errorMessage.append('.');
            status->setText(errorMessage);
            AppReset();

        }

    }

    else if (commandFinished == QFtp::List)
    {

        if (fileExists)
        {

            fileExists = false;
            FileNameIncrease();
            ftp.list();

        }

        else
        {

            status->setText("Uploading...");
            fileData.open(QIODevice::ReadOnly);
            ftp.put(&fileData,fileName);
            fileData.close();

        }

    }

}

void        ImageSaver::ftpListInfo(QUrlInfo urlInfo)
{

    if (urlInfo.name() == fileName)
    {

        fileExists = true;

    }

}

void        ImageSaver::httpDataTransfer(int bytesTransfered, int bytesTotal)
{

    progress->setMaximum(bytesTotal);
    progress->setValue(bytesTransfered);
    //qDebug() << "download" << bytesTotal << bytesTransfered ;

}

void        ImageSaver::httpRequestFinished(int request, bool error)
{

    //  If GET request comeplete
    if ( http.currentRequest().method() == "GET" )
    {

        //  If no errors occured
        if ( !error )
        {

            ftp.list();

        }

        //  If an error occured while downloading file
        else
        {

            QString errorMessage("Download error #");
            QHttp::Error errorCode = http.error();
            errorMessage.append(QString::number(errorCode));
            errorMessage.append('.');
            status->setText(errorMessage);
            AppReset();

        }

    }

}

void        ImageSaver::draged(QPoint delta)
{

    QPoint position = this->pos() + delta;
    int x = position.x();
    int y = position.y();
    QSize size = frameGeometry().size();
    QPoint corner = QApplication::desktop()->geometry().bottomRight();
    int magnetPower = property("MAGNET_POWER").toInt();

    int deltaLeft = x;
    int deltaRight = corner.x() - x - size.width();
    int deltaTop = y;
    int deltaBottom = corner.y() - y - size.height();

    if (deltaLeft <= magnetPower)
    {

        x = 0;

    }

    else if (deltaRight <= magnetPower)
    {

        x = corner.x() - size.width();

    }

    if (deltaTop <= magnetPower)
    {

        y = 0;

    }

    else if (deltaBottom <= magnetPower )
    {

        y = corner.y() - size.height();

    }

    this->move(x,y);

}

void        ImageSaver::resized(void)
 {

     MoveToCorner();

 }

void        ImageSaver::closeEvent(QCloseEvent* event)
{

    if (appLocked)
    {

        event->ignore();

    }

    else
    {

        event->accept();

    }

}



void        ImageSaver::SetAppearance(void)
{

    //  Applying fonts
    QFont urlFont("Century Gothic");
    this->setFont(urlFont);

    //  Applying window settings
    this->setFixedSize(250,50);
    this->setWindowTitle("Save files from Web to FTP");

}

void        ImageSaver::ConnectToFTP(void)
{

    ftp.connectToHost(property("FTP_HOST").toString(),property("FTP_PORT").toInt());
    ftp.login(property("FTP_USER").toString(),property("FTP_PASWORD").toString());

}

void        ImageSaver::AppReset(void)
{

    /*

        Reset the application

    */



    AppUnlock();
    url->Reset();
    progress->setValue(0);
    this->

}

void        ImageSaver::AppLock(void)
{

    /*

        Lock the application

    */



    save->QPushButton::setEnabled(false);
    url->setEnabled(false);
    appLocked = true;

}

void        ImageSaver::AppUnlock(void)
{

    /*

        Unlock the application

    */



    save->QPushButton::setEnabled(true);
    url->setEnabled(true);
    appLocked = false;

}

void        ImageSaver::MoveToCorner(void)
 {

     QSize screenSize = QApplication::desktop()->size();
     QSize appSize = frameGeometry().size();
     QPoint posNew(screenSize.width()-appSize.width(),screenSize.height()-appSize.height());

     move(posNew);

 }

void        ImageSaver::FileNameIncrease(void)
{

    QString fileNameCopy(fileName);

    QRegExp fileNumberRE("\\([0-9]*\\)\\..*$");
    int numberPosition = fileNameCopy.lastIndexOf(fileNumberRE);

    if (numberPosition >= 0)
    {

        //  Getting file number
        QRegExp beginOfFile("^[^.]*\\(");
        QRegExp endOfFile("\\)\\..*$");
        fileNameCopy.replace(beginOfFile,"");
        fileNameCopy.replace(endOfFile,"");
        int fileNumber = fileNameCopy.toInt() + 1;

        //  Getting file extention
        fileNameCopy = fileName;
        QRegExp fileBaseRE("^[^\\.]*\\.");
        QString fileExtention = fileNameCopy.replace(fileBaseRE,"");

        //  Creating new file name
        QString fileEndOldSTR;
        QString fileEndNewSTR;
        fileEndOldSTR.append("(").append(QString::number(fileNumber-1)).append(").").append(fileExtention);
        fileEndNewSTR.append("(").append(QString::number(fileNumber)).append(").").append(fileExtention);

        fileName.replace(fileEndOldSTR,fileEndNewSTR);

    }

    else
    {

        //  Getting new file base
        QRegExp fileExtentionRE("\\..*$");
        QString fileBase = fileNameCopy.replace(fileExtentionRE,"");
        QString fileBaseNew(fileBase);
        fileBaseNew = fileBaseNew.append("(0)");

        fileName.replace(fileBase,fileBaseNew);

    }

}

bool        ImageSaver::ConfigLoad()
{

    QFile config("config");

    if (config.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream stream(&config);
        stream.setCodec("UTF-8");

        while ( ! stream.atEnd() )
        {

            QString configLine = stream.readLine();
            if (configLine != "" && configLine[0] != '#')
            {

                QString parameter(configLine.section(' ',0,0));
                QVariant value(configLine.section(' ',1,1));
                setProperty(qPrintable(parameter),value);

            }

        }

        return true;

    }

    return false;

}
