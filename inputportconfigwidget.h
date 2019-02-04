#ifndef INPUTPORTCONFIGWIDGET_H
#define INPUTPORTCONFIGWIDGET_H

#include <QWidget>

namespace Ui {
class InputPortConfigWidget;
}

class InputPortConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InputPortConfigWidget(QWidget *parent = nullptr);
    ~InputPortConfigWidget();

private:
    Ui::InputPortConfigWidget *ui;
};

#endif // INPUTPORTCONFIGWIDGET_H
