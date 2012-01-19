#pragma once

#include "renderpointset.h"

#include <QtGui>

class PointSetEditor : public QWidget
{
    Q_OBJECT

public:
    PointSetEditor(QWidget *parent = 0);

    RenderPointSet *renderPointSet;
    QGroupBox *propertiesWidget;

protected slots:
    void updateLineEdits();
    void boundingBoxEdited();

protected:
    void createPropertiesWidget();

    QLineEdit *lineEdits[4];
};
