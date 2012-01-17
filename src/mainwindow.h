#pragma once

#include <QMainWindow>
#include <QStackedLayout>

class RenderPointSet;
class RenderTriangulation;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void newPointSet();
    void openPointSet();
    void savePointSetAs();

    void openTriangulation();
    void saveTriangulationAs();
    void renderTriangulationEPS();

private:
    void createActions();
    void createMenus();
    void enablePointEditor();
    void enableTriangulationEditor();
    void setPointEditorMode(bool);
    void setTriangulationEditorMode(bool);

    // Point Editor
    RenderPointSet *renderPointSet;
    QString point_path;
    QAction *newPointSetAct;
    QAction *openPointSetAct;
    QAction *savePointSetAsAct;

    // Triangulation Editor
    RenderTriangulation *renderTriangulation;
    QString triangulation_path;
    QAction *openTriangulationAct;
    QAction *saveTriangulationAsAct;
    QAction *renderTriangulationEPSAct;

    // Misc
    QStackedLayout *stackedLayout;
    QAction *exitAct;
};
