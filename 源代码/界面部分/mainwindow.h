#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QMessageBox>

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
    void on_pushButton_clear_clicked();

    void on_pushButton_solve_clicked();

    void readboard();

    void showboard();

    void on_pushButton_generate_clicked();

    void on_pushButton_judge_clicked();

    //void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QButtonGroup *groupButton1;
};
#endif // MAINWINDOW_H
