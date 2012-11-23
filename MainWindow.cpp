#include <MainWindow.h>

            ImageSaver::ImageSaver()
            :   QWidget(NULL,Qt::WindowStaysOnTopHint|Qt::CustomizeWindowHint),
                fileExists(false),
                appLocked(false)
{

    // Trying to load configuration
    configuration = new Configuration("config");

    if (configuration->load())
    {

        //  Setting appearance
        SetAppearance();

        //  Connecting to FTP server
        ConnectToFTP();

        // FTP signals
        QObject::connect(&ftpClient,SIGNAL(stateChanged(int)),this,SLOT(ftpStateChanged(int)));
        QObject::connect(&ftpClient,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(ftpDataTransfer(qint64,qint64)));
        QObject::connect(&ftpClient,SIGNAL(commandFinished(int,bool)),this,SLOT(ftpRequestFinished(int,bool)));
        QObject::connect(&ftpClient,SIGNAL(listInfo(QUrlInfo)),this,SLOT(ftpListInfo(QUrlInfo)));

        //  HTTP signals
        QObject::connect(&httpClient,SIGNAL(requestFinished(int,bool)),this,SLOT(httpRequestFinished(int,bool)));
        QObject::connect(&httpClient,SIGNAL(dataReadProgress(int,int)),this,SLOT(httpDataTransfer(int,int)));
        QObject::connect(&httpClient,SIGNAL(responseHeaderReceived(QHttpResponseHeader)),this,SLOT(httpResponseRecieved(QHttpResponseHeader)));

    }

}

void        ImageSaver::fileSave(void)
{

    //  Locking the form
    FormLock();

    //  Changing the status
    statusBar->setText("Downloading...");

    //  If the URL is valid
    QUrl url(this->urlField->text());

    if ( url.isValid() )
    {

        //  Saving file name
        fileName = url.path().section('/',-1);

        if ( fileName.contains('.') )
        {

            //  Starting download
            httpClient.setHost(url.host());
            httpClient.get(url.path(),&fileData);
            return;

        }

    }

    //  The URL is not valid
    statusBar->setText("The URL is not valid.");
    FormReset();

}

void        ImageSaver::ftpStateChanged(int state)
{

    switch (state)
    {
        case 0:
            statusBar->setText("FTP connection lost");
            FormLock();
            ConnectToFTP();
            break;
        case 1:
            statusBar->setText("Host name lookup...");
            break;
        case 2:
            statusBar->setText("Connecting to FTP...");
            break;
        case 3:
            this->statusBar->setText("Connected to FTP");
            break;
        case 4:
            statusBar->setText("Ready");
            FormUnlock();
            break;
        case 5:
            statusBar->setText("FTP connection closing...");
            break;
    }

}

void        ImageSaver::ftpDataTransfer(qint64 bytesTransfered, qint64 bytesTotal)
{

    progressBar->setMaximum(bytesTotal);
    progressBar->setValue(bytesTotal - bytesTransfered);

}

void        ImageSaver::ftpRequestFinished(int command, bool error )
{

    QFtp::Command commandFinished = ftpClient.currentCommand();

    if (commandFinished == QFtp::Put)
    {

        if ( !error )
        {

            statusBar->setText("Saved");
            fileData.open(QIODevice::Truncate);
            fileData.close();
            FormReset();

        }
        else
        {

            QString errorMessage("Upload error #");
            QFtp::Error errorCode = ftpClient.error();
            errorMessage.append(QString::number(errorCode));
            errorMessage.append('.');
            statusBar->setText(errorMessage);
            FormReset();

        }

    }

    else if (commandFinished == QFtp::List)
    {

        if (fileExists)
        {

            fileExists = false;
            FileNameIncrease();
            ftpClient.list();

        }

        else
        {

            statusBar->setText("Uploading...");
            fileData.open(QIODevice::ReadOnly);
            ftpClient.put(&fileData,fileName);
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

    progressBar->setMaximum(bytesTotal);
    progressBar->setValue(bytesTransfered);
    //qDebug() << "download" << bytesTotal << bytesTransfered ;

}

void        ImageSaver::httpRequestFinished(int request, bool error)
{

    //  If GET request comeplete
    if ( httpClient.currentRequest().method() == "GET" )
    {

        //  If no errors occured
        if ( !error )
        {

            ftpClient.list();

        }

        //  If an error occured while downloading file
        else
        {

            QString errorMessage("Download error #");
            QHttp::Error errorCode = httpClient.error();
            errorMessage.append(QString::number(errorCode));
            errorMessage.append('.');
            statusBar->setText(errorMessage);
            FormReset();

        }

    }

}

void        ImageSaver::httpResponseRecieved(QHttpResponseHeader response)
{

    qDebug() << response.contentType();

}

void        ImageSaver::draged(QPoint delta)
{

    MoveTo(pos() + delta);

}

void        ImageSaver::resized(void)
 {

     //MoveToCorner();

 }

void        ImageSaver::closeEvent(QCloseEvent* event)
{

    appLocked
    ?   event->ignore()
    :   event->accept();

}

void        ImageSaver::moveEvent(QMoveEvent* event)
{

    //MoveTo(event->pos());

}

void        ImageSaver::resizeEvent(QResizeEvent* event)
{

  MoveToCorner();

}



void        ImageSaver::SetAppearance(void)
{

    //  Applying window settings
    this->setFixedSize(250,50);
    this->setWindowTitle("Save files from Web to FTP");

    //  Applying fonts
    QFont urlFont("Century Gothic");
    this->setFont(urlFont);

    //  File URL
    urlField = new ImageURL(this);

    //  Progress bar
    progressBar = new UploadProgress(this);
    QObject::connect(progressBar,SIGNAL(dragged(QPoint)),this,SLOT(draged(QPoint)));

    //  Status bar
    statusBar = new StatusBar(this);
    QObject::connect(statusBar,SIGNAL(dragged(QPoint)),this,SLOT(draged(QPoint)));

    //  Save button
    if (configuration->property("INTERFACE_TYPE").toInt() == 1)
    {

        saveButton = new SaveButton(this);
        QObject::connect(saveButton,SIGNAL(clicked()),this,SLOT(fileSave()));

    }

    else
    {

        urlField->resize(250,25);
        QObject::connect(urlField,SIGNAL(textChanged()),this,SLOT(fileSave()));

    }

}

void        ImageSaver::ConnectToFTP(void)
{

    QString     ftpHost = configuration->property("FTP_HOST").toString();
    int         ftpPort = configuration->property("FTP_PORT").toInt();
    QString     ftpUser = configuration->property("FTP_USER").toString();
    QString     ftpPassword = configuration->property("FTP_PASWORD").toString();

    ftpClient.connectToHost(ftpHost,ftpPort);
    ftpClient.login(ftpUser,ftpPassword);

}

void        ImageSaver::FormReset(void)
{

    /*

        Reset the application

    */



    FormUnlock();
    urlField->Reset();
    progressBar->setValue(0);

}

void        ImageSaver::FormLock(void)
{

    /*

        Lock the application

    */



    urlField->setEnabled(false);
    appLocked = true;

    if (configuration->property("INTERFACE_TYPE").toInt() == 1)
    {

        saveButton->QPushButton::setEnabled(false);

    }
}

void        ImageSaver::FormUnlock(void)
{

    /*

        Unlock the application

    */


    urlField->setEnabled(true);
    appLocked = false;

    if (configuration->property("INTERFACE_TYPE").toInt() == 1)
    {

        saveButton->QPushButton::setEnabled(true);

    }

}

void        ImageSaver::MoveToCorner(void)
{

    QString positionInitial = configuration->property("POSITION_INITIAL").toString();
    QSize   screenSize = QApplication::desktop()->size();
    QSize   windowSize = frameGeometry().size();
    QPoint  windowPosition(0,0);

    if (positionInitial == "TR")
    {

        windowPosition.setX(screenSize.width()-windowSize.width());

    }
    else if (positionInitial == "BL")
    {

        windowPosition.setY(screenSize.height()-windowSize.height());

    }
    else if (positionInitial == "BR")
    {

        windowPosition.setX(screenSize.width()-windowSize.width());
        windowPosition.setY(screenSize.height()-windowSize.height());

    }

    move(windowPosition);

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

void        ImageSaver::MagnetApply()
{

    QPoint windowPosition(pos());
    QSize windowSize = frameGeometry().size();
    QSize screenSize = QApplication::desktop()->size();
    int magnetPower = property("MAGNET_POWER").toInt();

    int dLeft = pos().x();
    int dRight = screenSize.width() - pos().x() - windowSize.width();
    int dTop = pos().y();
    int dBottom = screenSize.height() - pos().y() - windowSize.height();

    if (dLeft <= magnetPower)
        windowPosition.setX(0);
    else if (dRight <= magnetPower)
        windowPosition.setX(screenSize.width()-windowSize.width());

    if (dTop <= magnetPower)
        windowPosition.setY(0);
    else if (dBottom <= magnetPower)
        windowPosition.setY(screenSize.height()-windowSize.height());

    move(windowPosition);

}

void        ImageSaver::MoveTo(QPoint position)
{

    QSize   windowSize = frameGeometry().size();
    QSize   screenSize = QApplication::desktop()->size();
    int     magnetPower = configuration->property("MAGNET_POWER").toInt();

    int dLeft = position.x();
    int dRight = screenSize.width() - position.x() - windowSize.width();
    int dTop = position.y();
    int dBottom = screenSize.height() - position.y() - windowSize.height();

    if (dLeft <= magnetPower)
    {

        position.setX(0);

    }

    else if (dRight <= magnetPower)
    {

        position.setX(screenSize.width()-windowSize.width());

    }

    if (dTop <= magnetPower)
    {

        position.setY(0);

    }

    else if (dBottom <= magnetPower)
    {

        position.setY(screenSize.height()-windowSize.height());

    }

    move(position);

}
