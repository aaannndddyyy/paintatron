/*
  Paintatron
  Copyright (C) 2013 Bob Mottram
  bob@robotics.uk.to

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
#include "dialogsaveimage.h"
#include "dialogabout.h"

#define PATH_SEPARATOR  "/"
#define MAKE_DIR        "mkdir"
#define COPY_FILE       "cp"
#define DELETE_FILE     "rm -f"

#define NO_OF_IMAGES      16
#define MAX_SOURCE_IMAGES 128
#define MAX_SOURCE_TEXTS  128

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
    int no_of_source_texts;
    bool reloadSourceImages;
    bool reloadSourceTexts;

    bool eventFilter(QObject *obj, QEvent *event);
    void selectPreviewImage(int index);
    void reloadSources();
    void detectSourceImages();
    void detectSourceTexts();
    bool fileExists(QString filename);
    bool loadText(QString filename, int index);

    unsigned char * full_img;
    int full_img_width, full_img_height;

public slots:
    int saveImageAs();
    int loadSourceImage();
    int loadSourceText();
    int clearSourceImages();
    int clearSourceTexts();
    int clearSources();
    void openAbout();

public:
    QImage * source_image;
    QString * source_text;
    QImage image[NO_OF_IMAGES+1];
    QGraphicsPixmapItem * image_item[NO_OF_IMAGES+1];
    QGraphicsScene * image_scene[NO_OF_IMAGES+1];

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
