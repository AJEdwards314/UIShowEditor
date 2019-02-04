#ifndef OUTPUTPORTCONFIGWIDGET_H
#define OUTPUTPORTCONFIGWIDGET_H

#include <QWidget>

namespace Ui {
class OutputPortConfigWidget;
}

class OutputPortConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPortConfigWidget(QWidget *parent = nullptr);
    ~OutputPortConfigWidget();

private:
    Ui::OutputPortConfigWidget *ui;
};

#endif // OUTPUTPORTCONFIGWIDGET_H
