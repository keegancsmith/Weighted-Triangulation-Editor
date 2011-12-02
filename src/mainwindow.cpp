#include <QtGui>

#include "rendertriangulation.h"
#include "mainwindow.h"

MainWindow::MainWindow(QString filename)
    : filename(filename)
{
    renderTriangulation = new RenderTriangulation;
    setCentralWidget(renderTriangulation);

    setWindowTitle(tr("Weighted Triangulation Viewer"));

    createActions();
    createMenus();

    if (!filename.isEmpty())
        renderTriangulation->setTriangulation(filename);
}

void MainWindow::open()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Weighted Region"), filename,
        tr("Weighted Triangulation Files (*.txt)"));

    if (!path.isEmpty()) {
        filename = path;
        renderTriangulation->setTriangulation(filename);
    }
}

void MainWindow::saveAs()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save EPS"), filename,
        tr("Encapsulated Postscript (*.eps)"));

    if (!path.isEmpty()) {
        filename = path;
        renderTriangulation->save(path);
    }
}

void MainWindow::renderEPS()
{
    QString path;
    if (!filename.isEmpty()) {
        QFileInfo info(filename);
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
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAsAct = new QAction(tr("&Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::Save);
    saveAsAct->setStatusTip(tr("Save to a file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    renderEPSAct = new QAction(tr("&Render EPS..."), this);
    renderEPSAct->setShortcut(tr("Ctrl+R"));
    renderEPSAct->setStatusTip(tr("Render the map to an EPS file"));
    connect(renderEPSAct, SIGNAL(triggered()), this, SLOT(renderEPS()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(renderEPSAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}
