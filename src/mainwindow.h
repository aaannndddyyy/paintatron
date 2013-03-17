#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include "paintatron.h"

#define PATH_SEPARATOR  "/"
#define MAKE_DIR        "mkdir"
#define COPY_FILE       "cp"
#define DELETE_FILE     "rm -f"

#define NO_OF_IMAGES      16
#define MAX_SOURCE_IMAGES 128

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    QGraphicsView * getPreview(int index);
    void loadPreviewImage(int index);
    void setup();
    void nextGeneration();
    paintatron * painter;
    int selected_preview_index;
    QString dataDirectory;
    int no_of_source_images;
    bool reloadSourceImages;

    bool eventFilter(QObject *obj, QEvent *event);
    void selectPreviewImage(int index);
    void reloadSources();
    void detectSourceImages();
    bool fileExists(QString filename);

    unsigned char * full_img;
    int full_img_width, full_img_height;

public slots:
    int saveImageAs();
    int loadSourceImage();
    int clearSourceImages();

public:
    QImage * source;
    QImage image[NO_OF_IMAGES+1];
    QGraphicsPixmapItem * image_item[NO_OF_IMAGES+1];
    QGraphicsScene * image_scene[NO_OF_IMAGES+1];

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
