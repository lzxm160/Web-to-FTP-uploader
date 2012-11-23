#ifndef UPLOADPROGRESS_H
#define UPLOADPROGRESS_H

#include <QProgressBar>
#include <QMouseEvent>
#include <QPoint>

class UploadProgress : public QProgressBar
{

    Q_OBJECT

    public:
        UploadProgress(QWidget* parent) : QProgressBar(parent)
        {

            this->move(0,25);
            this->resize(250,10);
            this->setTextVisible(false);
            this->isDragging = false;

        }

        void mousePressEvent(QMouseEvent* event)
        {

            this->isDragging = true;
            this->dragStart = event->pos();

        }

        void mouseMoveEvent(QMouseEvent* event)
        {

            if ( this->isDragging )
            {

                QPoint dragDelta = event->pos() - this->dragStart;

                emit dragged(dragDelta);

            }

        }

        void mouseReleaseEvent(QMouseEvent* event)
        {

            this->isDragging = false;

        }

    protected:
        void enterEvent(QEvent*)
        {

            QCursor cursor(Qt::SizeAllCursor);
            setCursor(cursor);

        }

    private:
        bool isDragging;
        QPoint dragStart;

    signals:
        void dragged(QPoint);

};

#endif // UPLOADPROGRESS_H
