#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/stitching.hpp>

#include <QPixmap>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


    //slide change
    void on_actionDilatation_Erosion_triggered();

    void on_actionLighting_triggered();

    //add picture
    void on_add_picture_triggered();


    //page 0
    void on_show_org_image_page0_clicked();

    void on_Dilatation_button_clicked();

    void on_Erosion_button_clicked();

    void on_show_cha_image_page0_clicked();

    void on_download_page0_clicked();


    //page1
    //set lighting From Slider
    void adjustBrightnessFromSlider(int value);

    //set lighting From Input
    void adjustBrightnessFromInput(int value);

    //input applybutton
    void onApplyButtonClicked();

    void on_show_org_image_page1_clicked();

    void on_show_cha_image_page1_clicked();

    void on_download_page1_clicked();

    void on_actionResizing_triggered();

    void on_actiongraphic_stitching_triggered();

    void scaleImageByPercentage();

    void scaleImageByDimensions();

    void on_show_cha_image_page2_clicked();

    void on_download_page2_clicked();

    void on_show_org_image_page2_clicked();

    void on_stitching_button_clicked();

    void on_showChangedPicture_clicked();

    void on_download_page3_clicked();

    void on_Canny_edge_triggered();

    void on_show_org_image_page4_clicked();

    void on_show_cha_image_page4_clicked();

    void on_download_page2_2_clicked();

    void on_Canny_clicked();

private:
    Ui::MainWindow *ui;
    // show original picture in pixmap
    QPixmap originalPixmap;
    QPixmap originalPixmap_page0;
    QPixmap originalPixmap_page1;
    QPixmap originalPixmap_page2;
    QPixmap originalPixmap_page3;
    QPixmap originalPixmap_page4;
    //page0 image
    cv::Mat srcImage_page0;
    //page0 dilatedImage
    cv::Mat dilatedImage_page0;
    //page0 erodeImage
    cv::Mat erodeImage_page0;
    //the image showed on the label_page0(stock changed image)
    QImage qImg_page0;
    //the image showed on the label_page1(stock changed image)
    QImage qImg_page1;
    //the image showed on the label_page2(stock changed image)
    QImage qImg_page2;
    //the image showed on the label_page3(stock changed image)
    QImage qImg_page3;
    //the image showed on the label_page4(stock changed image)
    QImage qImg_page4;
    cv::Mat qImg_org_page4;

    //changed image of page1 in pixmap
    QPixmap changedPixmap_page1;
    //changed image of page2 in pixmap
    QPixmap changedPixmap_page2;
    //changed image of page3 in pixmap
    QPixmap changedPixmap_page3;
    //changed image of page4 in pixmap
    QPixmap changedPixmap_page4;


    //add pictures to page3
    QStringList filenames;


    //help tools
    void setLabelPixmap(QLabel* label, const QPixmap& pixmap);
    void showImageDialog(const QPixmap& pixmap);
    cv::Mat applyMorphologicalTransform(const cv::Mat& src, int operation, int size);
    cv::Mat QPixmapToCvMat(const QPixmap &pixmap);
    QPixmap MatToPixmap(const cv::Mat &mat, QImage &qimage);
    void SetImageToLabel(QLabel *label, const QPixmap &pixmap);
    QImage MatToQImage(const cv::Mat &mat);



    //save image
    bool saveImageToFile(const QImage& image, const QString& dialogTitle);

};

#endif // MAINWINDOW_H
