#include <QtGui>

#include "mainwindow.h"
#include "pointseteditor.h"
#include "rendertriangulation.h"

MainWindow::MainWindow()
{
    pointSetEditor = new PointSetEditor;
    renderTriangulation = new RenderTriangulation;

    stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(pointSetEditor);
    stackedLayout->addWidget(renderTriangulation);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(stackedLayout);
    setCentralWidget(centralWidget);

    setWindowTitle(tr("Weighted Triangulation Editor"));

    createActions();
    createMenus();

    setPointEditorMode(true);
    setTriangulationEditorMode(false);

    QStringList argv = qApp->arguments();
    if (argv.size() >= 2) {
        QString path = argv.last();
        if (path.endsWith(".txt", Qt::CaseInsensitive)) {
            enableTriangulationEditor();
            triangulation_path = path;
            renderTriangulation->setTriangulation(path);
        } else if (path.endsWith(".node", Qt::CaseInsensitive)) {
            enablePointEditor();
            point_path = path;
            pointSetEditor->renderPointSet->open(path);
        } else {
            qWarning() << "File with unrecognized extension passed on command line." << path;
        }
    }
}

void MainWindow::newPointSet()
{
    pointSetEditor->renderPointSet->clear();
    enablePointEditor();
}

void MainWindow::openPointSet()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Point Set"), point_path,
        tr("Point Set Files (*.node)"));

    if (!path.isEmpty()) {
        enablePointEditor();
        point_path = path;
        pointSetEditor->renderPointSet->open(path);
    }
}

void MainWindow::savePointSetAs()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save Point Set"), point_path,
        tr("Point Set Files (*.node)"));

    if (!path.isEmpty()) {
        point_path = path;
        pointSetEditor->renderPointSet->save(path);
    }
}

void MainWindow::addPointSetGrid()
{
    bool ok;
    int rows = QInputDialog::getInt(this, "Add Grid", "Rows:", 10, 1, 10000, 1, &ok);
    if (!ok)
        return;
    int cols = QInputDialog::getInt(this, "Add Grid", "Columns:", 10, 1, 10000, 1, &ok);
    if (!ok)
        return;

    pointSetEditor->renderPointSet->addGrid(rows, cols);
}

void MainWindow::openTriangulation()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Weighted Region"), triangulation_path,
        tr("Weighted Triangulation Files (*.txt)"));

    if (!path.isEmpty()) {
        enableTriangulationEditor();
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
    newPointSetAct = new QAction(tr("New Point Set..."), this);
    newPointSetAct->setStatusTip(tr("Create a new point set"));
    connect(newPointSetAct, SIGNAL(triggered()), this, SLOT(newPointSet()));

    openPointSetAct = new QAction(tr("Open Point Set..."), this);
    openPointSetAct->setStatusTip(tr("Open an existing point set file"));
    connect(openPointSetAct, SIGNAL(triggered()), this, SLOT(openPointSet()));

    savePointSetAsAct = new QAction(tr("Save Point Set As..."), this);
    savePointSetAsAct->setStatusTip(tr("Save Point Set to a file"));
    connect(savePointSetAsAct, SIGNAL(triggered()), this, SLOT(savePointSetAs()));

    addPointSetGridAct = new QAction(tr("Add Point Set Grid..."), this);
    addPointSetGridAct->setStatusTip(tr("Add a regular grid of points to the point set"));
    connect(addPointSetGridAct, SIGNAL(triggered()), this, SLOT(addPointSetGrid()));

    openTriangulationAct = new QAction(tr("Open Triangulation..."), this);
    openTriangulationAct->setStatusTip(tr("Open an existing weighted triangulation file"));
    connect(openTriangulationAct, SIGNAL(triggered()), this, SLOT(openTriangulation()));

    saveTriangulationAsAct = new QAction(tr("Save Triangulation As..."), this);
    saveTriangulationAsAct->setStatusTip(tr("Save Triangulation to a file"));
    connect(saveTriangulationAsAct, SIGNAL(triggered()), this, SLOT(saveTriangulationAs()));

    renderTriangulationEPSAct = new QAction(tr("&Render EPS..."), this);
    renderTriangulationEPSAct->setShortcut(tr("Ctrl+R"));
    renderTriangulationEPSAct->setStatusTip(tr("Render the map to an EPS file"));
    connect(renderTriangulationEPSAct, SIGNAL(triggered()), this, SLOT(renderTriangulationEPS()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newPointSetAct);
    fileMenu->addAction(openPointSetAct);
    fileMenu->addAction(savePointSetAsAct);
    fileMenu->addAction(addPointSetGridAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openTriangulationAct);
    fileMenu->addAction(saveTriangulationAsAct);
    fileMenu->addAction(renderTriangulationEPSAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

void MainWindow::enablePointEditor()
{
    setTriangulationEditorMode(false);
    setPointEditorMode(true);
}

void MainWindow::enableTriangulationEditor()
{
    setPointEditorMode(false);
    setTriangulationEditorMode(true);
}

void MainWindow::setPointEditorMode(bool enabled)
{
    savePointSetAsAct->setEnabled(enabled);
    addPointSetGridAct->setEnabled(enabled);
    if (enabled)
        stackedLayout->setCurrentWidget(pointSetEditor);
}

void MainWindow::setTriangulationEditorMode(bool enabled)
{
    saveTriangulationAsAct->setEnabled(enabled);
    renderTriangulationEPSAct->setEnabled(enabled);
    if (enabled)
        stackedLayout->setCurrentWidget(renderTriangulation);
}
