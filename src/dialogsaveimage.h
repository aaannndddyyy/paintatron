#ifndef DIALOGSAVEIMAGE_H
#define DIALOGSAVEIMAGE_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include "paintatron.h"
#include "mainwindow.h"

namespace Ui {
class DialogSaveImage;
}

class DialogSaveImage : public QDialog
{
    Q_OBJECT

private slots:
    void selectFilename();
    void accept();

public:
    int selected_index;
    paintatron * painter;
    QImage * source_image;
    QString * source_text;
    int no_of_source_images;
    int no_of_source_texts;

    explicit DialogSaveImage(QWidget *parent = 0, int selected_index=0,
                             paintatron * painter=0,
                             QImage * source_image=0,
                             int no_of_source_images=0,
                             QString * source_text=0,
                             int no_of_source_texts=0);
    ~DialogSaveImage();
    
private:
    Ui::DialogSaveImage *ui;
};

#endif // DIALOGSAVEIMAGE_H
