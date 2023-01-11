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
    test = new QColorDialog(this);
    connect(test, SIGNAL(currentColorChanged(QColor)), this, SLOT(getColor(QColor)));
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
    qDebug() << actor;
    actor->GetProperty()->SetDiffuseColor(color.redF(), color.greenF(), color.blueF()); 
    qDebug() << color.redF() << color.greenF() << color.blueF();

    ui->openGLWidget->GetRenderWindow()->Render();
}

void MainWindow::on_color_triggered()
{
    test->show();
}
