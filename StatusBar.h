#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

class StatusBar : public QLabel
{

    Q_OBJECT

    public:

        StatusBar(QWidget* parent) : QLabel(parent)
        {

            this->move(0,35);
            this->resize(250,15);
            this->isDragging = false;

        }

        virtual void mousePressEvent(QMouseEvent* event)
        {

            this->isDragging = true;
            this->dragStart = event->pos();

        }

        virtual void mouseMoveEvent(QMouseEvent* event)
        {

            if ( this->isDragging )
            {

                QPoint dragDelta = event->pos() - this->dragStart;

                emit dragged(dragDelta);

            }

        }

        virtual void mouseReleaseEvent(QMouseEvent* event)
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

#endif // STATUSBAR_H
