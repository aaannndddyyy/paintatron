#include "dialogsaveimage.h"
#include "ui_dialogsaveimage.h"

DialogSaveImage::DialogSaveImage(QWidget *parent,
                                 int selected_index,
                                 paintatron * painter,
                                 QImage * source,
                                 int no_of_source_images) :
    QDialog(parent),
    ui(new Ui::DialogSaveImage)
{
    ui->setupUi(this);
    this->selected_index = selected_index;
    this->painter = painter;
    this->source = source;
    this->no_of_source_images = no_of_source_images;
    ui->spinBoxWidth->setValue(800);
    ui->spinBoxHeight->setValue(800);
    ui->lineEditFilename->setText("");
    ui->spinBoxWidth->setFocus();

    connect(ui->pushButtonFilename,SIGNAL(clicked()),this,SLOT(selectFilename()));
}

DialogSaveImage::~DialogSaveImage()
{
    delete ui;
}

void DialogSaveImage::selectFilename()
{
    // get the filename
    QString filename =
        QFileDialog::getSaveFileName(this,
            tr("Enter a filename to save as"),
            QDir::homePath(),
            tr("PNG Image Files (*.png)"));

    // if no filename was given
    if (filename.length()==0) return;

    ui->lineEditFilename->setText(filename);
}

void DialogSaveImage::accept()
{
    if (ui->lineEditFilename->text().length() == 0) {
        ui->lineEditFilename->setFocus();
        QMessageBox::information(this, "Save Image As","No filename given");
        return;
    }

    unsigned char * img =
                (unsigned char*)malloc(ui->spinBoxWidth->value() *
                                       ui->spinBoxHeight->value() * 3);

    painter->select_best(selected_index);
    painter->produce_art(selected_index, img,
                         ui->spinBoxWidth->value(),
                         ui->spinBoxHeight->value(),
                         source, no_of_source_images,
                         (char*)ui->lineEditFilename->text().toStdString().c_str());
    free(img);
    this->close();
}
