#ifndef SAVEBUTTON_H
#define SAVEBUTTON_H

#include <QPushButton>

class SaveButton : public QPushButton
{

    public:

        SaveButton (QWidget* parent) : QPushButton(parent)
        {

            setText("Save");
            move(200,0);
            resize(50,25);
            setEnabled(false);

        }

};

#endif // SAVEBUTTON_H
