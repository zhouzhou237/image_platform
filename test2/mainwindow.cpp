#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Imagedialog.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/stitching.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->brightnessSlider, &QSlider::valueChanged, this, &MainWindow::adjustBrightnessFromSlider);

    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::onApplyButtonClicked);

    connect(ui->brightnessEdit, &QLineEdit::returnPressed, this, &MainWindow::onApplyButtonClicked);

    connect(ui->pushButton_method1_page2, &QPushButton::clicked,this,&MainWindow::scaleImageByPercentage);

    connect(ui->M1Edit_page2, &QLineEdit::returnPressed,this,&MainWindow::scaleImageByPercentage);

    connect(ui->pushButton_method2_page2, &QPushButton::clicked,this,&MainWindow::scaleImageByDimensions);

}

MainWindow::~MainWindow()
{
    delete ui;

}


//Change to page 0
void MainWindow::on_actionDilatation_Erosion_triggered()
{

    ui->stackedWidget->setCurrentWidget(ui->page_0);
}

//Change to page 1
void MainWindow::on_actionLighting_triggered()
{

    ui->stackedWidget->setCurrentWidget(ui->page_1);
}

//Change to page 2
void MainWindow::on_actionResizing_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->page_2);
}

//Change to page 3
void MainWindow::on_actiongraphic_stitching_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->page_3);
}

//Change to page 4
void MainWindow::on_Canny_edge_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->page_4);
}


//Help tools:

void MainWindow::setLabelPixmap(QLabel* label, const QPixmap& pixmap) {
    if (label && !pixmap.isNull()) {
        label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QMessageBox::warning(this, "Error", "No images available to display.");
        return;
    }
}

void MainWindow::showImageDialog(const QPixmap& pixmap) {
    if(pixmap.isNull()) {
        QMessageBox::warning(this, "Error", "No images available to display.");
        return;
    }

    ImageDialog *dialog = new ImageDialog(this);
    dialog->setImage(pixmap);
    dialog->resize(pixmap.size());
    dialog->show();
}

cv::Mat MainWindow::applyMorphologicalTransform(const cv::Mat& src, int operation, int size) {
    cv::Mat result;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                        cv::Size(2 * size + 1, 2 * size + 1),
                        cv::Point(size, size));

    if (operation == MORPH_DILATE) {
        cv::dilate(src, result, element);
    } else if (operation == MORPH_ERODE) {
        cv::erode(src, result, element);
    }

    return result;
}

bool MainWindow::saveImageToFile(const QImage& image, const QString& dialogTitle) {
    if (image.isNull()) {
        QMessageBox::warning(this, "Error", "Image is not valid and cannot be saved.");
        return false;
    }

    QString filter = "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp)";
    QString defaultPath = QDir::homePath() + "/untitled.png";  // Default path and filename
    QString fileName = QFileDialog::getSaveFileName(this, tr(qPrintable(dialogTitle)), defaultPath, tr(qPrintable(filter)));

    if (fileName.isEmpty()) {
        return false;
    }

    return image.save(fileName);
}

cv::Mat MainWindow::QPixmapToCvMat(const QPixmap &pixmap) {
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), static_cast<size_t>(image.bytesPerLine()));
    cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
    return mat.clone();
}

QPixmap MainWindow::MatToPixmap(const cv::Mat &mat, QImage &qimage) {
    qimage = QImage((const uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    return QPixmap::fromImage(qimage);
}

void MainWindow::SetImageToLabel(QLabel *label, const QPixmap &pixmap) {
    if (label) {
        label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

QImage MainWindow::MatToQImage(const cv::Mat &mat)
{
    switch (mat.type()) {
        case CV_8UC4: {
            return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
        }
        case CV_8UC3: {
            cv::Mat rgb;
            cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
            return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        }
        case CV_8UC1: {
            return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        }
        default:
            break;
    }
    return QImage();
}

//for file-add

//add picture to certain page
void MainWindow::on_add_picture_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("open Image"),
                                                    ".",
                                                    tr("Image Files(*.jpg *.png)"));
    // if don't have image
    if(fileName.isEmpty()){
        return;
    }

    srcImage_page0 = imread(fileName.toLatin1().data());

    cv::cvtColor(srcImage_page0, srcImage_page0, COLOR_BGR2RGB);

    QImage disImage = QImage((const unsigned char*)srcImage_page0.data,
                             srcImage_page0.cols,srcImage_page0.rows,QImage::Format_RGB888);
    originalPixmap= QPixmap::fromImage(disImage);

    QLabel *targetLabel = nullptr;
    if(ui->stackedWidget->currentIndex()==0) {
        originalPixmap_page0=originalPixmap;
        targetLabel = ui->page0_org_img;
    }else if(ui->stackedWidget->currentIndex()==1) {
        originalPixmap_page1=originalPixmap;
        targetLabel = ui->page1_org_img;
    }else if(ui->stackedWidget->currentIndex()==2) {
        originalPixmap_page2=originalPixmap;
        targetLabel = ui->page2_org_img;
    }else if(ui->stackedWidget->currentIndex()==4) {
        qImg_org_page4 = imread(fileName.toStdString());
        originalPixmap_page4=originalPixmap;
        targetLabel = ui->page4_org_img;
    }

    if (targetLabel) {
        setLabelPixmap(targetLabel, originalPixmap);
    }

}



//page0 - dilatation / erosion



//show bigger image by click button "original"
void MainWindow::on_show_org_image_page0_clicked()
{
    showImageDialog(originalPixmap_page0);
}


//show bigger image by click button "changed"
void MainWindow::on_show_cha_image_page0_clicked()
{
    if (dilatedImage_page0.empty() && erodeImage_page0.empty()) {
        QMessageBox::warning(this, "Error", "No images available to display.");
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Choose Image");
    msgBox.setText("Which image would you like to display?");
    QPushButton *dilatedButton = msgBox.addButton("Dilated", QMessageBox::AcceptRole);
    QPushButton *erodedButton = msgBox.addButton("Eroded", QMessageBox::RejectRole);

    msgBox.exec();

    cv::Mat imageToShow;
    if (msgBox.clickedButton() == dilatedButton) {
        if (!dilatedImage_page0.empty()) {
            imageToShow = dilatedImage_page0;
        } else {
            QMessageBox::warning(this, "Error", "Dilated image not available.");
            return;
        }
    } else if(msgBox.clickedButton() == erodedButton){
        if (!erodeImage_page0.empty()) {
            imageToShow = erodeImage_page0;
        } else {
            QMessageBox::warning(this, "Error", "Eroded image not available.");
            return;
        }
    }

    qImg_page0 = QImage((const uchar*)imageToShow.data, imageToShow.cols, imageToShow.rows, imageToShow.step, QImage::Format_RGB888).rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(qImg_page0);
    showImageDialog(pixmap);
}

//dilatation image
void MainWindow::on_Dilatation_button_clicked()
{
    if(srcImage_page0.empty()){
        QMessageBox::warning(this, "Error", "No image loaded");
        return;
    }

    bool ok;
    int dilationSize = QInputDialog::getInt(this, tr("Dilation Size"),
                                            tr("Enter dilation size:(1-20)"), 5, 1, 20, 1, &ok);
    if (!ok) {
        return;
    }

    dilatedImage_page0 = applyMorphologicalTransform(srcImage_page0, MORPH_DILATE, dilationSize);
    QPixmap pixmap = MatToPixmap(dilatedImage_page0,qImg_page0);
    SetImageToLabel(ui->page0_cha_img,pixmap);
}

//erosion image
void MainWindow::on_Erosion_button_clicked()
{
    if(srcImage_page0.empty()){
        QMessageBox::warning(this, "Error", "No image loaded");
        return;
    }

    bool ok;
    int erodeSize = QInputDialog::getInt(this, tr("Erosion Size"),
                                            tr("Enter erosion size:(1-20)"), 5, 1, 20, 1, &ok);
    if (!ok) {
        return;
    }

    erodeImage_page0 = applyMorphologicalTransform(srcImage_page0, MORPH_ERODE, erodeSize);
    QPixmap pixmap = MatToPixmap(erodeImage_page0,qImg_page0);
    SetImageToLabel(ui->page0_cha_img,pixmap);
}

//download button
void MainWindow::on_download_page0_clicked() {
    saveImageToFile(qImg_page0, "Save Processed Image");
}



//page1 - Lighten / darken


//set lighting From Slider
void MainWindow::adjustBrightnessFromSlider(int value) {
    cv::Mat src = QPixmapToCvMat(originalPixmap_page1);
    cv::Mat adjustedImage;
    int brightnessOffset = value;
    src.convertTo(adjustedImage, -1, 1.0, brightnessOffset);  // change lighting

    QPixmap pixmap = MatToPixmap(adjustedImage,qImg_page1);
    changedPixmap_page1 = pixmap;
    SetImageToLabel(ui->page1_cha_img,pixmap);

    ui->brightnessValueLabel->setText(QString::number(value));
}

//set lighting From Input
void MainWindow::onApplyButtonClicked() {

    bool ok;
    int brightnessValue = ui->brightnessEdit->text().toInt(&ok);
    //it is integer?
    if(ok) {
        adjustBrightnessFromInput(brightnessValue);
    }else {
        QMessageBox::warning(this, "Input Error","Please enter a valid number.");
    }
}

//input applybutton
void MainWindow::adjustBrightnessFromInput(int value) {
    cv::Mat src = QPixmapToCvMat(originalPixmap_page1);
    cv::Mat adjustedImage;
    int brightnessOffset = value;
    src.convertTo(adjustedImage, -1, 1.0, brightnessOffset);

    QPixmap pixmap = MatToPixmap(adjustedImage,qImg_page1);
    changedPixmap_page1 = pixmap;
    SetImageToLabel(ui->page1_cha_img,pixmap);

    ui->currentBrightnessLabel->setText(QString::number(value));
}

//show bigger image by click button "original"
void MainWindow::on_show_org_image_page1_clicked()
{
    showImageDialog(originalPixmap_page1);
}

//show bigger image by click button "changed"
void MainWindow::on_show_cha_image_page1_clicked()
{
    showImageDialog(changedPixmap_page1);
}

//download image
void MainWindow::on_download_page1_clicked()
{
    saveImageToFile(qImg_page1, "Save Processed Image");
}



//page2 - resizing


//Method1-resezing by factor
void MainWindow::scaleImageByPercentage() {
    bool ok;
    int percentage = ui->M1Edit_page2->text().toInt(&ok);
    if(!ok||percentage <= 0||percentage > 100) {
        QMessageBox::warning(this,"Error","Invalid percentage. Please enter a positive number.");
        return;
    }

    double factor = percentage / 100.0;

    cv::Mat src = QPixmapToCvMat(originalPixmap_page2);
    if(src.empty()) {
        QMessageBox::warning(this, "Error", "Failed to load image.");
        return;
    }

    cv::Mat adjustedImage;
    cv::resize(src,adjustedImage,cv::Size(),factor,factor,cv::INTER_LINEAR);

    QPixmap pixmap = MatToPixmap(adjustedImage,qImg_page2);
    changedPixmap_page2 = pixmap;

    QMessageBox::information(this,"Success","Image changed success!");
}

//Method2-resezing by demensions
void MainWindow::scaleImageByDimensions() {
    bool ok;
    int width = ui->M1Edit_Width_page2->text().toInt(&ok);
    if(!ok || width <= 0) {
        QMessageBox::warning(this, "Error", "Invalid width.");
        return;
    }
    int height = ui->M1Edit_Height_page2->text().toInt(&ok);
    if(!ok || height <= 0) {
        QMessageBox::warning(this, "Error", "Invalid height.");
        return;
    }

    cv::Mat src = QPixmapToCvMat(originalPixmap_page2);
    if(src.empty()) {
        QMessageBox::warning(this, "Error", "Failed to load image.");
        return;
    }

    cv::Mat adjustedImage;
    cv::resize(src,adjustedImage,cv::Size(width,height),0,0,cv::INTER_LINEAR);

    QPixmap pixmap = MatToPixmap(adjustedImage,qImg_page2);
    changedPixmap_page2 = pixmap;

    QMessageBox::information(this,"Success","Image changed success!");
}

//show original image
void MainWindow::on_show_org_image_page2_clicked()
{
    showImageDialog(originalPixmap_page2);
}

//show changed image
void MainWindow::on_show_cha_image_page2_clicked()
{
    showImageDialog(changedPixmap_page2);
}

//download changed image
void MainWindow::on_download_page2_clicked()
{
    saveImageToFile(qImg_page2, "Save Processed Image");
}


//page3-stitching



void MainWindow::on_stitching_button_clicked() {
    filenames = QFileDialog::getOpenFileNames(this, "Select images for stitching", QString(), "Images (*.png *.jpg *.jpeg *.bmp)");
    if (filenames.size() < 2) {
        QMessageBox::warning(this, "Insufficient Images", "Please select at least two images for stitching.");
        return;
    }

    std::vector<cv::Mat> images;

    if (!filenames.isEmpty()) {
        QString fileNamesText;
        for (const QString &file : qAsConst(filenames)) {
            fileNamesText += file + "\n"; // Add file name to the text
            cv::Mat image = cv::imread(file.toStdString());
            if (image.empty()) {
                QMessageBox::warning(this, "Image Load Error", "Failed to load image: " + file);
                return;
            }
            std::cout << "Loaded file: " << file.toStdString() << std::endl;
            images.push_back(image); // Push the loaded image into the vector
        }

        ui->showpicture_label->setText("Selected images:\n" + fileNamesText); // Set text with file names
        QApplication::processEvents();

        cv::Mat result;
        // Use default Stitcher configuration
        Stitcher sti = Stitcher::createDefault(true);
        cv::Stitcher::Status status = sti.stitch(images, result);

        if (status == cv::Stitcher::OK) {
            QPixmap pixmap = MatToPixmap(result, qImg_page3);
            changedPixmap_page3 = pixmap;
        } else {
            QString errorMsg = "Could not stitch images. Error code: " + QString::number(status);
            QMessageBox::warning(this, "Stitching failed", errorMsg);
        }
    } else {
        QMessageBox::information(this, "No images selected", "Please select images to stitch.");
    }
}

void MainWindow::on_showChangedPicture_clicked()
{
    showImageDialog(changedPixmap_page3);
}

void MainWindow::on_download_page3_clicked()
{
     saveImageToFile(qImg_page3, "Save Processed Image");
}



//page4-Canny edge



void MainWindow::on_show_org_image_page4_clicked()
{
    showImageDialog(originalPixmap_page4);
}

void MainWindow::on_show_cha_image_page4_clicked()
{
    showImageDialog(changedPixmap_page4);
}

void MainWindow::on_download_page2_2_clicked()
{
    saveImageToFile(qImg_page4, "Save Processed Image");
}

void MainWindow::on_Canny_clicked()
{
    if(originalPixmap_page4.isNull()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please load an image first!"));
        return;
    }

    bool ok1,ok2,ok3;
    double threshould1 = ui->lineEdit_ts1->text().toDouble(&ok1);
    double threshould2 = ui->lineEdit_ts2->text().toDouble(&ok2);
    int kernel = ui->lineEdit_kernel->text().toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter valid numbers!"));
        return;
    }


    Mat edges;
    cv::Canny(qImg_org_page4,edges,threshould1,threshould2,kernel);


    qImg_page4 = MatToQImage(edges);
    QPixmap edgePixmap = QPixmap::fromImage(qImg_page4);
    changedPixmap_page4 = edgePixmap;
    SetImageToLabel(ui->page4_cha_img,edgePixmap);

}
