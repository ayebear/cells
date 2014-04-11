#include "widget.h"

Widget::Widget()
{
    focus = false;
}

void Widget::setFocus(bool mode)
{
    focus = mode;
}

bool Widget::hasFocus() const
{
    return focus;
}
