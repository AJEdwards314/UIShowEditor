#ifndef OUTPUTPORTCONFIGWIDGET_H
#define OUTPUTPORTCONFIGWIDGET_H

#include <QWidget>

#include "portconfig.h"

namespace Ui {
class OutputPortConfigWidget;
}

class OutputPortConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPortConfigWidget(PortConfig::OutConfigBase ** outConfig, QWidget *parent = nullptr);
    ~OutputPortConfigWidget() override;
    void showEvent(QShowEvent * event) override;
    void accepted();

private slots:
    void cbChanged(const QString &text);

private:
    Ui::OutputPortConfigWidget * ui;
    PortConfig::OutConfigBase ** outConfig;
};

#endif // OUTPUTPORTCONFIGWIDGET_H
