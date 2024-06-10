#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QLabel>

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent = nullptr);
    void setImage(const QPixmap &pixmap);

private:
    QLabel *imageLabel;
};

#endif // IMAGEDIALOG_H
