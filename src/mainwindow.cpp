#include <QtGui>

#include "rendertriangulation.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{
    renderTriangulation = new RenderTriangulation;
    setCentralWidget(renderTriangulation);

    setWindowTitle(tr("Weighted Triangulation Viewer"));

    createActions();
    createMenus();
}

void MainWindow::openTriangulation()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Weighted Region"), triangulation_path,
        tr("Weighted Triangulation Files (*.txt)"));

    if (!path.isEmpty()) {
        triangulation_path = path;
        renderTriangulation->setTriangulation(path);
    }
}

void MainWindow::saveTriangulationAs()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save Triangulation As"), triangulation_path,
        tr("Weighted Triangulation Files (*.txt)"));

    if (!path.isEmpty()) {
        triangulation_path = path;
        renderTriangulation->save(path);
    }
}

void MainWindow::renderTriangulationEPS()
{
    QString path;
    if (!triangulation_path.isEmpty()) {
        QFileInfo info(triangulation_path);
        path = info.dir().path() + "/" + info.completeBaseName() + ".eps";
    }

    path = QFileDialog::getSaveFileName(
        this, tr("Save EPS"), path,
        tr("Encapsulated Postscript (*.eps)"));

    if (!path.isEmpty())
        renderTriangulation->renderEPS(path);
}

void MainWindow::createActions()
{
    openTriangulationAct = new QAction(tr("Open Triangulation..."), this);
    openTriangulationAct->setStatusTip(tr("Open an existing weighted triangulation file"));
    connect(openTriangulationAct, SIGNAL(triggered()), this, SLOT(openTriangulation()));

    saveTriangulationAsAct = new QAction(tr("Save Triangulation As..."), this);
    saveTriangulationAsAct->setStatusTip(tr("Save Triangulation to a file"));
    saveTriangulationAsAct->setEnabled(false);
    connect(saveTriangulationAsAct, SIGNAL(triggered()), this, SLOT(saveTriangulationAs()));

    renderTriangulationEPSAct = new QAction(tr("&Render EPS..."), this);
    renderTriangulationEPSAct->setShortcut(tr("Ctrl+R"));
    renderTriangulationEPSAct->setStatusTip(tr("Render the map to an EPS file"));
    renderTriangulationEPSAct->setEnabled(false);
    connect(renderTriangulationEPSAct, SIGNAL(triggered()), this, SLOT(renderTriangulationEPS()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openTriangulationAct);
    fileMenu->addAction(saveTriangulationAsAct);
    fileMenu->addAction(renderTriangulationEPSAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}
