#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup();
    detectSourceImages();
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

    full_img_width = 800;
    full_img_height = 800;
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
    connect(ui->actionLoadImage,SIGNAL(triggered()),
            this,SLOT(loadSourceImage()));

}

// saves the currently selected image
int MainWindow::saveImageAs()
{
    int retval = 0;

    // if no image is showing
    if (selected_preview_index == -1) {
        QMessageBox::information(this, "Save Image As","You must first select one of the small preview images");
        return -1;
    }

    // get the filename
    QString filename =
        QFileDialog::getSaveFileName(this,
            tr("Enter a filename to save as"),
            QDir::homePath(),
            tr("PNG Image Files (*.png)"));

    // if no filename was given
    if (filename.length()==0) return -1;

    QString command = QString(COPY_FILE) + " " + dataDirectory + QString(PATH_SEPARATOR) + "full.png " + filename;
    retval = system(command.toStdString().c_str());
    QMessageBox::information(this, "Save Image As","Image saved as " + filename);
    return retval;
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
            tr("PNG Image Files (*.png) | Jpeg Image Files (*.jpg)"));

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
    if (filename.toLower().endsWith(".gif")) {
        command += ".gif";
    }
    qDebug("filename %s",filename.toStdString().c_str());
    qDebug("command %s",command.toStdString().c_str());
    retval = system(command.toStdString().c_str());
    QMessageBox::information(this, "Load Source Image","Image loaded");
    no_of_source_images++;
    reloadSourceImages = true;
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
            (fileExists(filename+".jpg")) ||
            (fileExists(filename+".gif"))) {
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
        if (!success) success = source[i].load(filename+".gif");
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
