#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
//#include <QAbstractSlider>
//#include <QSlider>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //void on_pushButton_load_clicked();
    void on_pushButton_3_clicked(); // reverse

    void on_pushButton_4_clicked(); // amplify

    //void handleButton();
    //void onButtonClicked();
    void on_Button_save_clicked();

    std::string on_Button_directory_clicked(); // wpisanie directory i nazwy pliku
    //std::string on_Button_fileName_clicked();

    void on_horizontalSlider_valueChanged(int value); //suwak

    void on_checkBox_toggled(bool arg);
    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

  //  void on_doubleSpinBox_textChanged(const QString &arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_6_clicked();

    //void on_Button_save_play_clicked();

    void on_pushButton_fadeIN_clicked(); // fade in

    void on_pushButton_fadeOUT_clicked(); // fade out


    //void on_lineEdit_play_cursorPositionChanged(int arg1, int arg2);

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;
    //QPushButton *m_button;
    //QPushButton *button;

    //QSlider *m_slider;
};
#endif // MAINWINDOW_H
