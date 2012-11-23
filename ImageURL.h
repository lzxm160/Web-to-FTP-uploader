#ifndef IMAGEURL_H
#define IMAGEURL_H

#include <QLineEdit>
#include <QDropEvent>
#include <QMimeData>
#include <QString>

class ImageURL : public QLineEdit
{

    public:
        ImageURL (QWidget* parent) : QLineEdit(parent) , textDefault("Insert image URL here...")
        {

            this->setText(this->textDefault);
            this->resize(200,25);
            this->setEnabled(false);

        }
        void Reset(void)
        {

            this->setText(this->textDefault);

        }

    protected:
        virtual void focusInEvent(QFocusEvent *e)
        {

            if ( this->text() == this->textDefault )
            {

                this->clear();

            }

        }
        virtual void focusOutEvent ( QFocusEvent* e )
        {

            if ( this->text() == "")
            {

                this->setText(this->textDefault);

            }

        }
        virtual void dropEvent(QDropEvent* e)
        {

            this->setText(e->mimeData()->text());

        }

    private:
        const QString textDefault;

};

#endif // IMAGEURL_H
