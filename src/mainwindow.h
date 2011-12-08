#pragma once

#include <QMainWindow>

class RenderTriangulation;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openTriangulation();
    void saveTriangulationAs();
    void renderTriangulationEPS();

private:
    void createActions();
    void createMenus();

    // Triangulation Editor
    RenderTriangulation *renderTriangulation;
    QString triangulation_path;
    QAction *openTriangulationAct;
    QAction *saveTriangulationAsAct;
    QAction *renderTriangulationEPSAct;

    // Misc
    QAction *exitAct;
};
