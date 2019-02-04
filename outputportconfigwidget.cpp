#include "outputportconfigwidget.h"
#include "ui_outputportconfigwidget.h"

OutputPortConfigWidget::OutputPortConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputPortConfigWidget)
{
    ui->setupUi(this);
}

OutputPortConfigWidget::~OutputPortConfigWidget()
{
    delete ui;
}
