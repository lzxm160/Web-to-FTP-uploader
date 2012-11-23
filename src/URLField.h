#ifndef IMAGEURL_H
#define IMAGEURL_H

#include <QLineEdit>
#include <QDropEvent>
#include <QMimeData>
#include <QString>
#include <QDebug>

class ImageURL : public QLineEdit
{

    Q_OBJECT

    public:
        ImageURL (QWidget* parent) : QLineEdit(parent) , textDefault("Drag'n'drop file URL here...")
        {

            this->setText(this->textDefault);
            this->resize(200,25);
            this->setEnabled(false);
            QObject::connect(this,SIGNAL(textChanged(QString)),this,SLOT(changed(QString)));

        }
        void Reset(void)
        {

            this->setText(this->textDefault);

        }

    protected:
        virtual void focusInEvent(QFocusEvent* event)
        {

            if ( this->text() == this->textDefault )
            {

                this->clear();

            }

        }
        virtual void focusOutEvent ( QFocusEvent* event )
        {

            if ( this->text() == "")
            {

                this->setText(this->textDefault);

            }

        }
        virtual void dropEvent(QDropEvent* event)
        {

            this->setText(event->mimeData()->text());

        }

    private:
        const QString textDefault;

    private slots:
        void changed(const QString)
        {

            if (text() != textDefault)
            {

                emit textChanged();

            }

        }

    signals:
        void textChanged();

};

#endif // IMAGEURL_H
