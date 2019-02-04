#include "inputportconfigwidget.h"
#include "ui_inputportconfigwidget.h"

InputPortConfigWidget::InputPortConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputPortConfigWidget)
{
    ui->setupUi(this);
}

InputPortConfigWidget::~InputPortConfigWidget()
{
    delete ui;
}
