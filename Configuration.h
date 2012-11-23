#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class Configuration: public QObject
{

    public:
                    Configuration()
                    {

                    }

                    Configuration   (   QString filePath,
                                        QString comment="#",
                                        QString delimiter="=",
                                        QString encoding="UTF-8"
                                    )
                                    :   filePath(filePath),
                                        comment(comment),
                                        delimiter(delimiter),
                                        encoding(encoding)
                    {

                    }

        bool        load()
                    {

                        QFile config(filePath);

                        if (config.open(QIODevice::ReadOnly|QIODevice::Text))
                        {

                            QTextStream stream(&config);
                            stream.setCodec("UTF-8");

                            while (!stream.atEnd())
                            {

                                QString configLine = stream.readLine();
                                if (configLine != "" && !configLine.startsWith(comment))
                                {

                                    QString parameter(configLine.section(delimiter,0,0));
                                    QVariant value(configLine.section(delimiter,1,1));
                                    setProperty(qPrintable(parameter),value);

                                }

                            }


                            return true;

                        }

                        return false;

                    }

    private:
        QString     filePath;
        QString     comment;
        QString     delimiter;
        QString     encoding;

};

#endif // CONFIGURATION_H
