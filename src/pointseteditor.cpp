#include "pointseteditor.h"

PointSetEditor::PointSetEditor(QWidget *parent)
  : QWidget(parent)
{
    renderPointSet = new RenderPointSet;
    createPropertiesWidget();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(renderPointSet);
    layout->addWidget(propertiesWidget);
    setLayout(layout);

    connect(renderPointSet, SIGNAL(boundingBoxChanged()), this, SLOT(updateLineEdits()));
}

void PointSetEditor::createPropertiesWidget()
{
    const char *labels[4] = {"Min X", "Max X", "Min Y", "Max Y"};

    QFontMetrics metrics(QApplication::font());
    int width = metrics.width("888888");

    QFormLayout *formLayout = new QFormLayout;
    for (int i = 0; i < 4; i++) {
        QLineEdit *lineEdit = new QLineEdit;
        formLayout->addRow(new QLabel(tr(labels[i])), lineEdit);
        lineEdit->setFixedWidth(width);
        lineEdit->setValidator(new QDoubleValidator);
        connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(boundingBoxEdited()));
        lineEdits[i] = lineEdit;
    }

    updateLineEdits();

    propertiesWidget = new QGroupBox(tr("Point Set Properties"));
    propertiesWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    propertiesWidget->setLayout(formLayout);
}

void PointSetEditor::updateLineEdits()
{
    lineEdits[0]->setText(QString::number(renderPointSet->xMin()));
    lineEdits[1]->setText(QString::number(renderPointSet->xMax()));
    lineEdits[2]->setText(QString::number(renderPointSet->yMin()));
    lineEdits[3]->setText(QString::number(renderPointSet->yMax()));
}

void PointSetEditor::boundingBoxEdited()
{
    renderPointSet->setXMin(lineEdits[0]->text().toDouble());
    renderPointSet->setXMax(lineEdits[1]->text().toDouble());
    renderPointSet->setYMin(lineEdits[2]->text().toDouble());
    renderPointSet->setYMax(lineEdits[3]->text().toDouble());
    updateLineEdits();
}
