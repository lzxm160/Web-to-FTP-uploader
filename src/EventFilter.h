#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include <QEvent>

class EventFilter : public QObject
{

    Q_OBJECT

    protected:
        bool eventFilter(QObject* object, QEvent* event)
        {

            QEvent::Type eventType = event->type();

            if (eventType == QEvent::Resize)
            {

                emit resized();

            }

            return false;

        }

    signals:
        void resized();

};

#endif // EVENTFILTER_H
