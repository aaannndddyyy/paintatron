/*
  Paintatron
  Copyright (C) 2013 Bob Mottram
  bob@sluggish.dyndns.org

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup();
    detectSourceImages();
    this->show();
    nextGeneration();
}

MainWindow::~MainWindow()
{
    delete [] source;
    free(full_img);
    delete painter;
    delete ui;
}

// returns the graphicsView object for the given preview index
QGraphicsView * MainWindow::getPreview(int index)
{
    QGraphicsView *g = ui->graphicsView;

    switch(index) {
    case 0: { g = ui->graphicsView0; break; }
    case 1: { g = ui->graphicsView1; break; }
    case 2: { g = ui->graphicsView2; break; }
    case 3: { g = ui->graphicsView3; break; }
    case 4: { g = ui->graphicsView4; break; }
    case 5: { g = ui->graphicsView5; break; }
    case 6: { g = ui->graphicsView6; break; }
    case 7: { g = ui->graphicsView7; break; }
    case 8: { g = ui->graphicsView8; break; }
    case 9: { g = ui->graphicsView9; break; }
    case 10: { g = ui->graphicsView10; break; }
    case 11: { g = ui->graphicsView11; break; }
    case 12: { g = ui->graphicsView12; break; }
    case 13: { g = ui->graphicsView13; break; }
    case 14: { g = ui->graphicsView14; break; }
    case 15: { g = ui->graphicsView15; break; }
    }
    return g;
}

void MainWindow::setup()
{
    QGraphicsView *g;
    QString command;

    painter = new paintatron(NO_OF_IMAGES);

    // create a directory where images will be stored
    dataDirectory = QDir::homePath() + QString(PATH_SEPARATOR) + ".paintatron";
    command = QString(MAKE_DIR) + " " + dataDirectory;
    (void)system(command.toStdString().c_str());

    source = new QImage[MAX_SOURCE_IMAGES];

    full_img_width = 400;
    full_img_height = 400;
    full_img = (unsigned char*)malloc(full_img_width*full_img_height*3);

    for (int index = 0; index < NO_OF_IMAGES; index++) {
        image_scene[index] = new QGraphicsScene();
        image_item[index] = new QGraphicsPixmapItem(QPixmap::fromImage(image[index]));
        image_scene[index]->setSceneRect(image[index].rect());

        g = getPreview(index);
        g->setScene(image_scene[index]);
        g->scene()->addItem(image_item[index]);
        g->fitInView(image[index].rect()); //, Qt::KeepAspectRatio);

        // Event for detecting mouse click on preview image
        g->installEventFilter(this);
    }
    // Event for detecting mouse click on full image
    ui->graphicsView->installEventFilter(this);
    selected_preview_index = -1;
    no_of_source_images = 0;
    reloadSourceImages = true;

    // Save the current image
    connect(ui->actionSave,SIGNAL(triggered()),
            this,SLOT(saveImageAs()));
    // Exit the application
    connect(ui->actionExit,SIGNAL(triggered()),
            this,SLOT(close()));
    // load a source image
    connect(ui->actionLoadImage,SIGNAL(triggered()),
            this,SLOT(loadSourceImage()));
    // clears source images
    connect(ui->actionClearSources,SIGNAL(triggered()),
            this,SLOT(clearSourceImages()));
    // open the about dialog
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(openAbout()));
}

// show the about dialog
void MainWindow::openAbout()
{
    DialogAbout * about = new DialogAbout(this);
    about->exec();
    delete about;
}

// saves the currently selected image
int MainWindow::saveImageAs()
{
    DialogSaveImage * save = new DialogSaveImage(this,selected_preview_index,painter,source,no_of_source_images);
    save->exec();
    delete save;
    return 0;
}

// loads a source image
int MainWindow::loadSourceImage()
{
    int retval = 0;

    if (no_of_source_images >= MAX_SOURCE_IMAGES) return 0;

    // get the filename
    QString filename =
        QFileDialog::getOpenFileName(this,
            tr("Enter a filename to load"),
            QDir::homePath(),
            tr("Images (*.png *.jpg)"));

    // if no filename was given
    if (filename.length()==0) return -1;

    QString command = QString(COPY_FILE) + " " + filename + " " +
                dataDirectory + QString(PATH_SEPARATOR) + "image" +
                QString::number(no_of_source_images);
    if (filename.toLower().endsWith(".png")) {
        command += ".png";
    }
    if (filename.toLower().endsWith(".jpg")) {
        command += ".jpg";
    }
    qDebug("filename %s",filename.toStdString().c_str());
    qDebug("command %s",command.toStdString().c_str());
    retval = system(command.toStdString().c_str());
    QMessageBox::information(this, "Load Source Image","Image loaded");
    no_of_source_images++;
    reloadSourceImages = true;
    return retval;
}

// clears all the source images
int MainWindow::clearSourceImages()
{
    QString filename, command;
    int retval = 0;
    bool deleted;
    QString extensions[] = {
        ".png", ".jpg"
    };

    for (int index = 0; index < no_of_source_images; index++) {
        filename =
            dataDirectory + QString(PATH_SEPARATOR) + "image" +
            QString::number(index);
        deleted = false;
        for (int ext = 0; ext < 2; ext++) {
            command = QString(DELETE_FILE) + " " + filename + extensions[ext];
            retval = system(command.toStdString().c_str());
            if (retval == 0) deleted = true;
        }
        if (!deleted) break;
    }
    no_of_source_images = 0;
    QMessageBox::information(this, "Clear source images","All source images have been cleared");
    return retval;
}

// load a preview image from file
void MainWindow::loadPreviewImage(int index)
{
    char filename[256];

    if (index < NO_OF_IMAGES) {
        sprintf(filename, "%s%spic_%d.png", dataDirectory.toStdString().c_str(), PATH_SEPARATOR, index);
    }
    else {
        sprintf(filename, "%s%sfull.png", dataDirectory.toStdString().c_str(), PATH_SEPARATOR);
    }

    image[index].load(filename);
    if (!image[index].isNull()) {
        QGraphicsView * g = getPreview(index);

        //delete image_scene[index];
        //delete image_item[index];
        image_scene[index] = new QGraphicsScene();
        image_item[index] = new QGraphicsPixmapItem(QPixmap::fromImage(image[index]));
        image_scene[index]->setSceneRect(image[index].rect());

        g = getPreview(index);
        g->setScene(image_scene[index]);
        g->scene()->addItem(image_item[index]);
        g->fitInView(image[index].rect(), Qt::KeepAspectRatio);

        g->show();
    }
}

bool MainWindow::fileExists(QString filename)
{
    FILE * fp;

    fp = fopen(filename.toStdString().c_str(),"r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

// detect how many source images exist
void MainWindow::detectSourceImages()
{
    no_of_source_images = 0;
    for (int i = 0; i < MAX_SOURCE_IMAGES; i++) {
        QString filename = dataDirectory + QString(PATH_SEPARATOR) + "image" +
                    QString::number(no_of_source_images);
        if ((fileExists(filename+".png")) ||
            (fileExists(filename+".jpg"))) {
            no_of_source_images = i+1;
        }
        else {
            break;
        }
    }
}

// reload the source images
void MainWindow::reloadSources()
{
    QString filename;
    bool success;

    reloadSourceImages = false;

    for (int i = 0; i < no_of_source_images; i++) {
        filename = dataDirectory + QString(PATH_SEPARATOR) + "image" +
                QString::number(i);
        success = source[i].load(filename+".png");
        if (!success) success = source[i].load(filename+".jpg");
        if (success) {
            qDebug("Image loaded %dx%d", source[i].width(), source[i].height());
        }
        else {
            qDebug("Couldn't load %d %s", i, filename.toStdString().c_str());
        }
    }
}

// produces the next generation of previews
void MainWindow::nextGeneration()
{
    QString directory = dataDirectory + QString(PATH_SEPARATOR);

    if (reloadSourceImages) {
        reloadSources();
    }

    painter->next_generation();
    painter->produce_population_art((char*)directory.toStdString().c_str(), source, no_of_source_images);
    for (int index = 0; index < NO_OF_IMAGES; index++) {
        loadPreviewImage(index);
    }
}

// Mouse click events
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->graphicsView && event->type() == QEvent::MouseButtonPress)
    {
        nextGeneration();
    }

    for (int index = 0; index < NO_OF_IMAGES; index++) {
        QGraphicsView * g = getPreview(index);
        if(obj == g && event->type() == QEvent::MouseButtonPress)
        {
            selectPreviewImage(index);
        }
    }
    return false;
}

// select a preview image and make it the current full image
void MainWindow::selectPreviewImage(int index)
{
    QString full_image_filename = dataDirectory + QString(PATH_SEPARATOR) + "full.png";

    selected_preview_index = index;

    painter->select_best(index);
    painter->produce_art(index, full_img,
                         full_img_width, full_img_height,
                         source, no_of_source_images,
                         (char*)full_image_filename.toStdString().c_str());
    loadPreviewImage(NO_OF_IMAGES);
}
