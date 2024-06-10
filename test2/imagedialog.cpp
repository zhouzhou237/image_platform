#include "ImageDialog.h"
#include <QVBoxLayout>

ImageDialog::ImageDialog(QWidget *parent) : QDialog(parent), imageLabel(new QLabel)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    imageLabel->setScaledContents(true);
}

void ImageDialog::setImage(const QPixmap &pixmap)
{
    imageLabel->setPixmap(pixmap);
    imageLabel->adjustSize(); // resize the label
    adjustSize();
}
