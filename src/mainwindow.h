#pragma once

#include <QMainWindow>

class RenderTriangulation;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString filename = QString());

private slots:
    void open();
    void renderEPS();

private:
    void createActions();
    void createMenus();

    RenderTriangulation *renderTriangulation;
    QString filename;
    QAction *openAct;
    QAction *renderEPSAct;
    QAction *exitAct;
};