#ifndef INPUTPORTCONFIGWIDGET_H
#define INPUTPORTCONFIGWIDGET_H

#include <QWidget>

#include "portconfig.h"

namespace Ui {
class InputPortConfigWidget;
}

class InputPortConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InputPortConfigWidget(PortConfig::InConfig ** inConfig, QWidget *parent = nullptr);
    ~InputPortConfigWidget() override;
    void showEvent(QShowEvent * event) override;
    void accepted();

private:
    Ui::InputPortConfigWidget *ui;
    PortConfig::InConfig ** inConfig;
};

#endif // INPUTPORTCONFIGWIDGET_H
