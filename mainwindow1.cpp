#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QDebug>
#include <vtkRenderWindow.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    id = 1;
    test = new QColorDialog(this);
    //style = new CustomInteractorStyle;
    connect(test, SIGNAL(currentColorChanged(QColor)), this, SLOT(getColor(QColor)));
    connect(this, SIGNAL(send_key(int)), this, SLOT(get_key(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_triggered()
{
    QString selfilter = tr("STL(*.stl)");
    QString title, directory;
    vtkSmartPointer<vtkPolyData> file = vtkSmartPointer<vtkPolyData>::New();
    QString fileName = QFileDialog::getOpenFileName(
        this,
        title,
        directory,
        tr("All files (*.*);;STL(*.stl)"),
        &selfilter
    );
    file = loadStl(fileName);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(file);

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    hash[id] = actor;
    id++;
    ui->openGLWidget->addActor(actor);

    qDebug() << actor;

}

vtkSmartPointer<vtkPolyData> MainWindow::loadStl(QString filename)
{
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(filename.toStdString().c_str());
    reader->Update();
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy(reader->GetOutput());

    return polyData;
}

void MainWindow::getColor(QColor color)
{
    hash[key1]->GetProperty()->SetDiffuseColor(color.redF(), color.greenF(), color.blueF());
    qDebug() << color.redF() << color.greenF() << color.blueF();

    ui->openGLWidget->GetRenderWindow()->Render();
}

void MainWindow::on_color_triggered()
{
    test->show();
}

void MainWindow::get_key(int key)
{
    key1 = key;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    QString text = event->key();
    int key;
    switch (event->key())
    {
    case Qt::Key_1:
        key = 1;
        emit send_key(key);
        break;
    case Qt::Key_2:
        key = 2;
        emit send_key(key);
        break;
    case Qt::Key_3:
        key = 3;
        emit send_key(key);
        break;
    }
}