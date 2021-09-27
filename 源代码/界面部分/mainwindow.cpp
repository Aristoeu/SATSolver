#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DPLLsolver.h"
#include "sudoku.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <direct.h>

#define KeepLiveBegin(ignoreevent) {IgnoreEvent* kl_ie =NULL; if(ignoreevent)kl_ie = new IgnoreEvent(); std::condition_variable kl_cv;std::mutex kl_mtx;std::unique_lock <std::mutex> kl_lck(kl_mtx);\
        std::thread kl_thread([&](){kl_mtx.lock(); kl_mtx.unlock();

#define KeepLiveEnd	kl_cv.notify_all();});kl_thread.detach(); \
        while (kl_cv.wait_for(kl_lck, std::chrono::milliseconds(5)) == std::cv_status::timeout){qApp->processEvents();}if(kl_ie!=NULL) delete kl_ie;}

class IgnoreEvent :public QObject
{
public:
    IgnoreEvent(QObject* obj=qApp)
    {
        m_obj = obj;
        m_obj->installEventFilter(this);
    }
    ~IgnoreEvent()
    {
        m_obj->removeEventFilter(this);
    }
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress)
        {
            event->ignore();
            return true;
        }
        return QObject::eventFilter(obj, event);
    }
private:
    QObject* m_obj;
};


using std::string;
int board[8][8];
int cntnum=0;
char buffer[1000];
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for(int i =0;i<8;i++)
        for(int j=0;j<8;j++)
            board[i][j]=-1;
    getcwd(buffer,1000);

        for(int i = 0;i<1000;i++){
            if(buffer[i]=='\0'){
                buffer[i]='\\';
                buffer[i+1]='1';
                buffer[i+2]='.';
                buffer[i+3]='c';
                buffer[i+4]='n';
                buffer[i+5]='f';
                buffer[i+6]='\0';
                break;
            }
        }
    groupButton1=new QButtonGroup(this);
    groupButton1->addButton(ui->radioButton_1,0);
    groupButton1->addButton(ui->radioButton_2,1);
    groupButton1->addButton(ui->radioButton_3,2);
    ui->radioButton_1->setChecked(true);
    QMessageBox::about(NULL,"提示","求解过程中程序可能会短暂的无响应，等待结果即可");
    connect(ui->pushButton, &QPushButton::clicked, [this](){

        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose CNF File"), tr("<Enter File Name>"), tr("*.cnf"));
        //KeepLiveBegin(true)
        qApp->processEvents();
        QByteArray ba = fileName.toLocal8Bit();
        char *filename = ba.data();
        //ui->pushButton->setText("正在求解中……请稍等……求解完会弹出提示！");
        int res;
        long start, stop;
        long long time;
        Solver S;
        //int res;
        int literalnum = S.read(filename);
        if (literalnum == -1){res=-2;S.write(0, literalnum, filename);}
        else{


                start = clock();
                res=S.solve();
                stop=clock();
                time = stop - start;
                S.write(time, literalnum, filename);


        }

        //ui->pushButton->setText("读取cnf文件进行求解");
        switch(res){
        case 1:
            QMessageBox::about(NULL, "有解", "当前文件有解,已经输出结果到与cnf文件同路径的res文件！\nlits:"+QString::number(S.model.size())+"  cla:"+QString::number(S.constrs.size())+"  time:"+QString::number(time));
            break;
        case 0:
        case -1:
            QMessageBox::critical(NULL, "错误", "当前文件无解！");
            break;
        case -2:
            QMessageBox::critical(NULL, "错误", "文件打开错误！");
            break;
        }
        //KeepLiveEnd
    });

}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::readboard()
{
    cntnum=0;
    for(int i =0;i<8;i++)
        for(int j=0;j<8;j++)
            board[i][j]=-1;
    if(ui->lineEdit_1->text().operator==(""))board[0][0]=-1;else if(ui->lineEdit_1->text().toInt()==0){board[0][0]=0;cntnum++;}else if(ui->lineEdit_1->text().toInt()==1){board[0][0]=1;cntnum++;}
    if(ui->lineEdit_2->text().operator==(""))board[0][1]=-1;else if(ui->lineEdit_2->text().toInt()==0){board[0][1]=0;cntnum++;}else if(ui->lineEdit_2->text().toInt()==1){board[0][1]=1;cntnum++;}
    if(ui->lineEdit_3->text().operator==(""))board[0][2]=-1;else if(ui->lineEdit_3->text().toInt()==0){board[0][2]=0;cntnum++;}else if(ui->lineEdit_3->text().toInt()==1){board[0][2]=1;cntnum++;}
    if(ui->lineEdit_4->text().operator==(""))board[0][3]=-1;else if(ui->lineEdit_4->text().toInt()==0){board[0][3]=0;cntnum++;}else if(ui->lineEdit_4->text().toInt()==1){board[0][3]=1;cntnum++;}
    if(ui->lineEdit_5->text().operator==(""))board[0][4]=-1;else if(ui->lineEdit_5->text().toInt()==0){board[0][4]=0;cntnum++;}else if(ui->lineEdit_5->text().toInt()==1){board[0][4]=1;cntnum++;}
    if(ui->lineEdit_6->text().operator==(""))board[0][5]=-1;else if(ui->lineEdit_6->text().toInt()==0){board[0][5]=0;cntnum++;}else if(ui->lineEdit_6->text().toInt()==1){board[0][5]=1;cntnum++;}
    if(ui->lineEdit_7->text().operator==(""))board[0][6]=-1;else if(ui->lineEdit_7->text().toInt()==0){board[0][6]=0;cntnum++;}else if(ui->lineEdit_7->text().toInt()==1){board[0][6]=1;cntnum++;}
    if(ui->lineEdit_8->text().operator==(""))board[0][7]=-1;else if(ui->lineEdit_8->text().toInt()==0){board[0][7]=0;cntnum++;}else if(ui->lineEdit_8->text().toInt()==1){board[0][7]=1;cntnum++;}
    if(ui->lineEdit_9->text().operator==(""))board[1][0]=-1;else if(ui->lineEdit_9->text().toInt()==0){board[1][0]=0;cntnum++;}else if(ui->lineEdit_9->text().toInt()==1){board[1][0]=1;cntnum++;}
    if(ui->lineEdit_10->text().operator==(""))board[1][1]=-1;else if(ui->lineEdit_10->text().toInt()==0){board[1][1]=0;cntnum++;}else if(ui->lineEdit_10->text().toInt()==1){board[1][1]=1;cntnum++;}
    if(ui->lineEdit_11->text().operator==(""))board[1][2]=-1;else if(ui->lineEdit_11->text().toInt()==0){board[1][2]=0;cntnum++;}else if(ui->lineEdit_11->text().toInt()==1){board[1][2]=1;cntnum++;}
    if(ui->lineEdit_12->text().operator==(""))board[1][3]=-1;else if(ui->lineEdit_12->text().toInt()==0){board[1][3]=0;cntnum++;}else if(ui->lineEdit_12->text().toInt()==1){board[1][3]=1;cntnum++;}
    if(ui->lineEdit_13->text().operator==(""))board[1][4]=-1;else if(ui->lineEdit_13->text().toInt()==0){board[1][4]=0;cntnum++;}else if(ui->lineEdit_13->text().toInt()==1){board[1][4]=1;cntnum++;}
    if(ui->lineEdit_14->text().operator==(""))board[1][5]=-1;else if(ui->lineEdit_14->text().toInt()==0){board[1][5]=0;cntnum++;}else if(ui->lineEdit_14->text().toInt()==1){board[1][5]=1;cntnum++;}
    if(ui->lineEdit_15->text().operator==(""))board[1][6]=-1;else if(ui->lineEdit_15->text().toInt()==0){board[1][6]=0;cntnum++;}else if(ui->lineEdit_15->text().toInt()==1){board[1][6]=1;cntnum++;}
    if(ui->lineEdit_16->text().operator==(""))board[1][7]=-1;else if(ui->lineEdit_16->text().toInt()==0){board[1][7]=0;cntnum++;}else if(ui->lineEdit_16->text().toInt()==1){board[1][7]=1;cntnum++;}
    if(ui->lineEdit_17->text().operator==(""))board[2][0]=-1;else if(ui->lineEdit_17->text().toInt()==0){board[2][0]=0;cntnum++;}else if(ui->lineEdit_17->text().toInt()==1){board[2][0]=1;cntnum++;}
    if(ui->lineEdit_18->text().operator==(""))board[2][1]=-1;else if(ui->lineEdit_18->text().toInt()==0){board[2][1]=0;cntnum++;}else if(ui->lineEdit_18->text().toInt()==1){board[2][1]=1;cntnum++;}
    if(ui->lineEdit_19->text().operator==(""))board[2][2]=-1;else if(ui->lineEdit_19->text().toInt()==0){board[2][2]=0;cntnum++;}else if(ui->lineEdit_19->text().toInt()==1){board[2][2]=1;cntnum++;}
    if(ui->lineEdit_20->text().operator==(""))board[2][3]=-1;else if(ui->lineEdit_20->text().toInt()==0){board[2][3]=0;cntnum++;}else if(ui->lineEdit_20->text().toInt()==1){board[2][3]=1;cntnum++;}
    if(ui->lineEdit_21->text().operator==(""))board[2][4]=-1;else if(ui->lineEdit_21->text().toInt()==0){board[2][4]=0;cntnum++;}else if(ui->lineEdit_21->text().toInt()==1){board[2][4]=1;cntnum++;}
    if(ui->lineEdit_22->text().operator==(""))board[2][5]=-1;else if(ui->lineEdit_22->text().toInt()==0){board[2][5]=0;cntnum++;}else if(ui->lineEdit_22->text().toInt()==1){board[2][5]=1;cntnum++;}
    if(ui->lineEdit_23->text().operator==(""))board[2][6]=-1;else if(ui->lineEdit_23->text().toInt()==0){board[2][6]=0;cntnum++;}else if(ui->lineEdit_23->text().toInt()==1){board[2][6]=1;cntnum++;}
    if(ui->lineEdit_24->text().operator==(""))board[2][7]=-1;else if(ui->lineEdit_24->text().toInt()==0){board[2][7]=0;cntnum++;}else if(ui->lineEdit_24->text().toInt()==1){board[2][7]=1;cntnum++;}
    if(ui->lineEdit_25->text().operator==(""))board[3][0]=-1;else if(ui->lineEdit_25->text().toInt()==0){board[3][0]=0;cntnum++;}else if(ui->lineEdit_25->text().toInt()==1){board[3][0]=1;cntnum++;}
    if(ui->lineEdit_26->text().operator==(""))board[3][1]=-1;else if(ui->lineEdit_26->text().toInt()==0){board[3][1]=0;cntnum++;}else if(ui->lineEdit_26->text().toInt()==1){board[3][1]=1;cntnum++;}
    if(ui->lineEdit_27->text().operator==(""))board[3][2]=-1;else if(ui->lineEdit_27->text().toInt()==0){board[3][2]=0;cntnum++;}else if(ui->lineEdit_27->text().toInt()==1){board[3][2]=1;cntnum++;}
    if(ui->lineEdit_28->text().operator==(""))board[3][3]=-1;else if(ui->lineEdit_28->text().toInt()==0){board[3][3]=0;cntnum++;}else if(ui->lineEdit_28->text().toInt()==1){board[3][3]=1;cntnum++;}
    if(ui->lineEdit_29->text().operator==(""))board[3][4]=-1;else if(ui->lineEdit_29->text().toInt()==0){board[3][4]=0;cntnum++;}else if(ui->lineEdit_29->text().toInt()==1){board[3][4]=1;cntnum++;}
    if(ui->lineEdit_30->text().operator==(""))board[3][5]=-1;else if(ui->lineEdit_30->text().toInt()==0){board[3][5]=0;cntnum++;}else if(ui->lineEdit_30->text().toInt()==1){board[3][5]=1;cntnum++;}
    if(ui->lineEdit_31->text().operator==(""))board[3][6]=-1;else if(ui->lineEdit_31->text().toInt()==0){board[3][6]=0;cntnum++;}else if(ui->lineEdit_31->text().toInt()==1){board[3][6]=1;cntnum++;}
    if(ui->lineEdit_32->text().operator==(""))board[3][7]=-1;else if(ui->lineEdit_32->text().toInt()==0){board[3][7]=0;cntnum++;}else if(ui->lineEdit_32->text().toInt()==1){board[3][7]=1;cntnum++;}
    if(ui->lineEdit_33->text().operator==(""))board[4][0]=-1;else if(ui->lineEdit_33->text().toInt()==0){board[4][0]=0;cntnum++;}else if(ui->lineEdit_33->text().toInt()==1){board[4][0]=1;cntnum++;}
    if(ui->lineEdit_34->text().operator==(""))board[4][1]=-1;else if(ui->lineEdit_34->text().toInt()==0){board[4][1]=0;cntnum++;}else if(ui->lineEdit_34->text().toInt()==1){board[4][1]=1;cntnum++;}
    if(ui->lineEdit_35->text().operator==(""))board[4][2]=-1;else if(ui->lineEdit_35->text().toInt()==0){board[4][2]=0;cntnum++;}else if(ui->lineEdit_35->text().toInt()==1){board[4][2]=1;cntnum++;}
    if(ui->lineEdit_36->text().operator==(""))board[4][3]=-1;else if(ui->lineEdit_36->text().toInt()==0){board[4][3]=0;cntnum++;}else if(ui->lineEdit_36->text().toInt()==1){board[4][3]=1;cntnum++;}
    if(ui->lineEdit_37->text().operator==(""))board[4][4]=-1;else if(ui->lineEdit_37->text().toInt()==0){board[4][4]=0;cntnum++;}else if(ui->lineEdit_37->text().toInt()==1){board[4][4]=1;cntnum++;}
    if(ui->lineEdit_38->text().operator==(""))board[4][5]=-1;else if(ui->lineEdit_38->text().toInt()==0){board[4][5]=0;cntnum++;}else if(ui->lineEdit_38->text().toInt()==1){board[4][5]=1;cntnum++;}
    if(ui->lineEdit_39->text().operator==(""))board[4][6]=-1;else if(ui->lineEdit_39->text().toInt()==0){board[4][6]=0;cntnum++;}else if(ui->lineEdit_39->text().toInt()==1){board[4][6]=1;cntnum++;}
    if(ui->lineEdit_40->text().operator==(""))board[4][7]=-1;else if(ui->lineEdit_40->text().toInt()==0){board[4][7]=0;cntnum++;}else if(ui->lineEdit_40->text().toInt()==1){board[4][7]=1;cntnum++;}
    if(ui->lineEdit_41->text().operator==(""))board[5][0]=-1;else if(ui->lineEdit_41->text().toInt()==0){board[5][0]=0;cntnum++;}else if(ui->lineEdit_41->text().toInt()==1){board[5][0]=1;cntnum++;}
    if(ui->lineEdit_42->text().operator==(""))board[5][1]=-1;else if(ui->lineEdit_42->text().toInt()==0){board[5][1]=0;cntnum++;}else if(ui->lineEdit_42->text().toInt()==1){board[5][1]=1;cntnum++;}
    if(ui->lineEdit_43->text().operator==(""))board[5][2]=-1;else if(ui->lineEdit_43->text().toInt()==0){board[5][2]=0;cntnum++;}else if(ui->lineEdit_43->text().toInt()==1){board[5][2]=1;cntnum++;}
    if(ui->lineEdit_44->text().operator==(""))board[5][3]=-1;else if(ui->lineEdit_44->text().toInt()==0){board[5][3]=0;cntnum++;}else if(ui->lineEdit_44->text().toInt()==1){board[5][3]=1;cntnum++;}
    if(ui->lineEdit_45->text().operator==(""))board[5][4]=-1;else if(ui->lineEdit_45->text().toInt()==0){board[5][4]=0;cntnum++;}else if(ui->lineEdit_45->text().toInt()==1){board[5][4]=1;cntnum++;}
    if(ui->lineEdit_46->text().operator==(""))board[5][5]=-1;else if(ui->lineEdit_46->text().toInt()==0){board[5][5]=0;cntnum++;}else if(ui->lineEdit_46->text().toInt()==1){board[5][5]=1;cntnum++;}
    if(ui->lineEdit_47->text().operator==(""))board[5][6]=-1;else if(ui->lineEdit_47->text().toInt()==0){board[5][6]=0;cntnum++;}else if(ui->lineEdit_47->text().toInt()==1){board[5][6]=1;cntnum++;}
    if(ui->lineEdit_48->text().operator==(""))board[5][7]=-1;else if(ui->lineEdit_48->text().toInt()==0){board[5][7]=0;cntnum++;}else if(ui->lineEdit_48->text().toInt()==1){board[5][7]=1;cntnum++;}
    if(ui->lineEdit_49->text().operator==(""))board[6][0]=-1;else if(ui->lineEdit_49->text().toInt()==0){board[6][0]=0;cntnum++;}else if(ui->lineEdit_49->text().toInt()==1){board[6][0]=1;cntnum++;}
    if(ui->lineEdit_50->text().operator==(""))board[6][1]=-1;else if(ui->lineEdit_50->text().toInt()==0){board[6][1]=0;cntnum++;}else if(ui->lineEdit_50->text().toInt()==1){board[6][1]=1;cntnum++;}
    if(ui->lineEdit_51->text().operator==(""))board[6][2]=-1;else if(ui->lineEdit_51->text().toInt()==0){board[6][2]=0;cntnum++;}else if(ui->lineEdit_51->text().toInt()==1){board[6][2]=1;cntnum++;}
    if(ui->lineEdit_52->text().operator==(""))board[6][3]=-1;else if(ui->lineEdit_52->text().toInt()==0){board[6][3]=0;cntnum++;}else if(ui->lineEdit_52->text().toInt()==1){board[6][3]=1;cntnum++;}
    if(ui->lineEdit_53->text().operator==(""))board[6][4]=-1;else if(ui->lineEdit_53->text().toInt()==0){board[6][4]=0;cntnum++;}else if(ui->lineEdit_53->text().toInt()==1){board[6][4]=1;cntnum++;}
    if(ui->lineEdit_54->text().operator==(""))board[6][5]=-1;else if(ui->lineEdit_54->text().toInt()==0){board[6][5]=0;cntnum++;}else if(ui->lineEdit_54->text().toInt()==1){board[6][5]=1;cntnum++;}
    if(ui->lineEdit_55->text().operator==(""))board[6][6]=-1;else if(ui->lineEdit_55->text().toInt()==0){board[6][6]=0;cntnum++;}else if(ui->lineEdit_55->text().toInt()==1){board[6][6]=1;cntnum++;}
    if(ui->lineEdit_56->text().operator==(""))board[6][7]=-1;else if(ui->lineEdit_56->text().toInt()==0){board[6][7]=0;cntnum++;}else if(ui->lineEdit_56->text().toInt()==1){board[6][7]=1;cntnum++;}
    if(ui->lineEdit_57->text().operator==(""))board[7][0]=-1;else if(ui->lineEdit_57->text().toInt()==0){board[7][0]=0;cntnum++;}else if(ui->lineEdit_57->text().toInt()==1){board[7][0]=1;cntnum++;}
    if(ui->lineEdit_58->text().operator==(""))board[7][1]=-1;else if(ui->lineEdit_58->text().toInt()==0){board[7][1]=0;cntnum++;}else if(ui->lineEdit_58->text().toInt()==1){board[7][1]=1;cntnum++;}
    if(ui->lineEdit_59->text().operator==(""))board[7][2]=-1;else if(ui->lineEdit_59->text().toInt()==0){board[7][2]=0;cntnum++;}else if(ui->lineEdit_59->text().toInt()==1){board[7][2]=1;cntnum++;}
    if(ui->lineEdit_60->text().operator==(""))board[7][3]=-1;else if(ui->lineEdit_60->text().toInt()==0){board[7][3]=0;cntnum++;}else if(ui->lineEdit_60->text().toInt()==1){board[7][3]=1;cntnum++;}
    if(ui->lineEdit_61->text().operator==(""))board[7][4]=-1;else if(ui->lineEdit_61->text().toInt()==0){board[7][4]=0;cntnum++;}else if(ui->lineEdit_61->text().toInt()==1){board[7][4]=1;cntnum++;}
    if(ui->lineEdit_62->text().operator==(""))board[7][5]=-1;else if(ui->lineEdit_62->text().toInt()==0){board[7][5]=0;cntnum++;}else if(ui->lineEdit_62->text().toInt()==1){board[7][5]=1;cntnum++;}
    if(ui->lineEdit_63->text().operator==(""))board[7][6]=-1;else if(ui->lineEdit_63->text().toInt()==0){board[7][6]=0;cntnum++;}else if(ui->lineEdit_63->text().toInt()==1){board[7][6]=1;cntnum++;}
    if(ui->lineEdit_64->text().operator==(""))board[7][7]=-1;else if(ui->lineEdit_64->text().toInt()==0){board[7][7]=0;cntnum++;}else if(ui->lineEdit_64->text().toInt()==1){board[7][7]=1;cntnum++;}
}
void MainWindow::showboard()
{
    if(board[0][0]==0)ui->lineEdit_1->setText("0");else if(board[0][0]==1)ui->lineEdit_1->setText("1");else ui->lineEdit_1->setText("");
    if(board[0][1]==0)ui->lineEdit_2->setText("0");else if(board[0][1]==1)ui->lineEdit_2->setText("1");else ui->lineEdit_2->setText("");
    if(board[0][2]==0)ui->lineEdit_3->setText("0");else if(board[0][2]==1)ui->lineEdit_3->setText("1");else ui->lineEdit_3->setText("");
    if(board[0][3]==0)ui->lineEdit_4->setText("0");else if(board[0][3]==1)ui->lineEdit_4->setText("1");else ui->lineEdit_4->setText("");
    if(board[0][4]==0)ui->lineEdit_5->setText("0");else if(board[0][4]==1)ui->lineEdit_5->setText("1");else ui->lineEdit_5->setText("");
    if(board[0][5]==0)ui->lineEdit_6->setText("0");else if(board[0][5]==1)ui->lineEdit_6->setText("1");else ui->lineEdit_6->setText("");
    if(board[0][6]==0)ui->lineEdit_7->setText("0");else if(board[0][6]==1)ui->lineEdit_7->setText("1");else ui->lineEdit_7->setText("");
    if(board[0][7]==0)ui->lineEdit_8->setText("0");else if(board[0][7]==1)ui->lineEdit_8->setText("1");else ui->lineEdit_8->setText("");
    if(board[1][0]==0)ui->lineEdit_9->setText("0");else if(board[1][0]==1)ui->lineEdit_9->setText("1");else ui->lineEdit_9->setText("");
    if(board[1][1]==0)ui->lineEdit_10->setText("0");else if(board[1][1]==1)ui->lineEdit_10->setText("1");else ui->lineEdit_10->setText("");
    if(board[1][2]==0)ui->lineEdit_11->setText("0");else if(board[1][2]==1)ui->lineEdit_11->setText("1");else ui->lineEdit_11->setText("");
    if(board[1][3]==0)ui->lineEdit_12->setText("0");else if(board[1][3]==1)ui->lineEdit_12->setText("1");else ui->lineEdit_12->setText("");
    if(board[1][4]==0)ui->lineEdit_13->setText("0");else if(board[1][4]==1)ui->lineEdit_13->setText("1");else ui->lineEdit_13->setText("");
    if(board[1][5]==0)ui->lineEdit_14->setText("0");else if(board[1][5]==1)ui->lineEdit_14->setText("1");else ui->lineEdit_14->setText("");
    if(board[1][6]==0)ui->lineEdit_15->setText("0");else if(board[1][6]==1)ui->lineEdit_15->setText("1");else ui->lineEdit_15->setText("");
    if(board[1][7]==0)ui->lineEdit_16->setText("0");else if(board[1][7]==1)ui->lineEdit_16->setText("1");else ui->lineEdit_16->setText("");
    if(board[2][0]==0)ui->lineEdit_17->setText("0");else if(board[2][0]==1)ui->lineEdit_17->setText("1");else ui->lineEdit_17->setText("");
    if(board[2][1]==0)ui->lineEdit_18->setText("0");else if(board[2][1]==1)ui->lineEdit_18->setText("1");else ui->lineEdit_18->setText("");
    if(board[2][2]==0)ui->lineEdit_19->setText("0");else if(board[2][2]==1)ui->lineEdit_19->setText("1");else ui->lineEdit_19->setText("");
    if(board[2][3]==0)ui->lineEdit_20->setText("0");else if(board[2][3]==1)ui->lineEdit_20->setText("1");else ui->lineEdit_20->setText("");
    if(board[2][4]==0)ui->lineEdit_21->setText("0");else if(board[2][4]==1)ui->lineEdit_21->setText("1");else ui->lineEdit_21->setText("");
    if(board[2][5]==0)ui->lineEdit_22->setText("0");else if(board[2][5]==1)ui->lineEdit_22->setText("1");else ui->lineEdit_22->setText("");
    if(board[2][6]==0)ui->lineEdit_23->setText("0");else if(board[2][6]==1)ui->lineEdit_23->setText("1");else ui->lineEdit_23->setText("");
    if(board[2][7]==0)ui->lineEdit_24->setText("0");else if(board[2][7]==1)ui->lineEdit_24->setText("1");else ui->lineEdit_24->setText("");
    if(board[3][0]==0)ui->lineEdit_25->setText("0");else if(board[3][0]==1)ui->lineEdit_25->setText("1");else ui->lineEdit_25->setText("");
    if(board[3][1]==0)ui->lineEdit_26->setText("0");else if(board[3][1]==1)ui->lineEdit_26->setText("1");else ui->lineEdit_26->setText("");
    if(board[3][2]==0)ui->lineEdit_27->setText("0");else if(board[3][2]==1)ui->lineEdit_27->setText("1");else ui->lineEdit_27->setText("");
    if(board[3][3]==0)ui->lineEdit_28->setText("0");else if(board[3][3]==1)ui->lineEdit_28->setText("1");else ui->lineEdit_28->setText("");
    if(board[3][4]==0)ui->lineEdit_29->setText("0");else if(board[3][4]==1)ui->lineEdit_29->setText("1");else ui->lineEdit_29->setText("");
    if(board[3][5]==0)ui->lineEdit_30->setText("0");else if(board[3][5]==1)ui->lineEdit_30->setText("1");else ui->lineEdit_30->setText("");
    if(board[3][6]==0)ui->lineEdit_31->setText("0");else if(board[3][6]==1)ui->lineEdit_31->setText("1");else ui->lineEdit_31->setText("");
    if(board[3][7]==0)ui->lineEdit_32->setText("0");else if(board[3][7]==1)ui->lineEdit_32->setText("1");else ui->lineEdit_32->setText("");
    if(board[4][0]==0)ui->lineEdit_33->setText("0");else if(board[4][0]==1)ui->lineEdit_33->setText("1");else ui->lineEdit_33->setText("");
    if(board[4][1]==0)ui->lineEdit_34->setText("0");else if(board[4][1]==1)ui->lineEdit_34->setText("1");else ui->lineEdit_34->setText("");
    if(board[4][2]==0)ui->lineEdit_35->setText("0");else if(board[4][2]==1)ui->lineEdit_35->setText("1");else ui->lineEdit_35->setText("");
    if(board[4][3]==0)ui->lineEdit_36->setText("0");else if(board[4][3]==1)ui->lineEdit_36->setText("1");else ui->lineEdit_36->setText("");
    if(board[4][4]==0)ui->lineEdit_37->setText("0");else if(board[4][4]==1)ui->lineEdit_37->setText("1");else ui->lineEdit_37->setText("");
    if(board[4][5]==0)ui->lineEdit_38->setText("0");else if(board[4][5]==1)ui->lineEdit_38->setText("1");else ui->lineEdit_38->setText("");
    if(board[4][6]==0)ui->lineEdit_39->setText("0");else if(board[4][6]==1)ui->lineEdit_39->setText("1");else ui->lineEdit_39->setText("");
    if(board[4][7]==0)ui->lineEdit_40->setText("0");else if(board[4][7]==1)ui->lineEdit_40->setText("1");else ui->lineEdit_40->setText("");
    if(board[5][0]==0)ui->lineEdit_41->setText("0");else if(board[5][0]==1)ui->lineEdit_41->setText("1");else ui->lineEdit_41->setText("");
    if(board[5][1]==0)ui->lineEdit_42->setText("0");else if(board[5][1]==1)ui->lineEdit_42->setText("1");else ui->lineEdit_42->setText("");
    if(board[5][2]==0)ui->lineEdit_43->setText("0");else if(board[5][2]==1)ui->lineEdit_43->setText("1");else ui->lineEdit_43->setText("");
    if(board[5][3]==0)ui->lineEdit_44->setText("0");else if(board[5][3]==1)ui->lineEdit_44->setText("1");else ui->lineEdit_44->setText("");
    if(board[5][4]==0)ui->lineEdit_45->setText("0");else if(board[5][4]==1)ui->lineEdit_45->setText("1");else ui->lineEdit_45->setText("");
    if(board[5][5]==0)ui->lineEdit_46->setText("0");else if(board[5][5]==1)ui->lineEdit_46->setText("1");else ui->lineEdit_46->setText("");
    if(board[5][6]==0)ui->lineEdit_47->setText("0");else if(board[5][6]==1)ui->lineEdit_47->setText("1");else ui->lineEdit_47->setText("");
    if(board[5][7]==0)ui->lineEdit_48->setText("0");else if(board[5][7]==1)ui->lineEdit_48->setText("1");else ui->lineEdit_48->setText("");
    if(board[6][0]==0)ui->lineEdit_49->setText("0");else if(board[6][0]==1)ui->lineEdit_49->setText("1");else ui->lineEdit_49->setText("");
    if(board[6][1]==0)ui->lineEdit_50->setText("0");else if(board[6][1]==1)ui->lineEdit_50->setText("1");else ui->lineEdit_50->setText("");
    if(board[6][2]==0)ui->lineEdit_51->setText("0");else if(board[6][2]==1)ui->lineEdit_51->setText("1");else ui->lineEdit_51->setText("");
    if(board[6][3]==0)ui->lineEdit_52->setText("0");else if(board[6][3]==1)ui->lineEdit_52->setText("1");else ui->lineEdit_52->setText("");
    if(board[6][4]==0)ui->lineEdit_53->setText("0");else if(board[6][4]==1)ui->lineEdit_53->setText("1");else ui->lineEdit_53->setText("");
    if(board[6][5]==0)ui->lineEdit_54->setText("0");else if(board[6][5]==1)ui->lineEdit_54->setText("1");else ui->lineEdit_54->setText("");
    if(board[6][6]==0)ui->lineEdit_55->setText("0");else if(board[6][6]==1)ui->lineEdit_55->setText("1");else ui->lineEdit_55->setText("");
    if(board[6][7]==0)ui->lineEdit_56->setText("0");else if(board[6][7]==1)ui->lineEdit_56->setText("1");else ui->lineEdit_56->setText("");
    if(board[7][0]==0)ui->lineEdit_57->setText("0");else if(board[7][0]==1)ui->lineEdit_57->setText("1");else ui->lineEdit_57->setText("");
    if(board[7][1]==0)ui->lineEdit_58->setText("0");else if(board[7][1]==1)ui->lineEdit_58->setText("1");else ui->lineEdit_58->setText("");
    if(board[7][2]==0)ui->lineEdit_59->setText("0");else if(board[7][2]==1)ui->lineEdit_59->setText("1");else ui->lineEdit_59->setText("");
    if(board[7][3]==0)ui->lineEdit_60->setText("0");else if(board[7][3]==1)ui->lineEdit_60->setText("1");else ui->lineEdit_60->setText("");
    if(board[7][4]==0)ui->lineEdit_61->setText("0");else if(board[7][4]==1)ui->lineEdit_61->setText("1");else ui->lineEdit_61->setText("");
    if(board[7][5]==0)ui->lineEdit_62->setText("0");else if(board[7][5]==1)ui->lineEdit_62->setText("1");else ui->lineEdit_62->setText("");
    if(board[7][6]==0)ui->lineEdit_63->setText("0");else if(board[7][6]==1)ui->lineEdit_63->setText("1");else ui->lineEdit_63->setText("");
    if(board[7][7]==0)ui->lineEdit_64->setText("0");else if(board[7][7]==1)ui->lineEdit_64->setText("1");else ui->lineEdit_64->setText("");
}
void MainWindow::on_pushButton_clear_clicked()
{
 if(!ui->lineEdit_1->text().operator==("")){ui->lineEdit_1->setText("");board[0][0]=-1;}
 if(!ui->lineEdit_2->text().operator==("")){ui->lineEdit_2->setText("");board[0][1]=-1;}
 if(!ui->lineEdit_3->text().operator==("")){ui->lineEdit_3->setText("");board[0][2]=-1;}
 if(!ui->lineEdit_4->text().operator==("")){ui->lineEdit_4->setText("");board[0][3]=-1;}
 if(!ui->lineEdit_5->text().operator==("")){ui->lineEdit_5->setText("");board[0][4]=-1;}
 if(!ui->lineEdit_6->text().operator==("")){ui->lineEdit_6->setText("");board[0][5]=-1;}
 if(!ui->lineEdit_7->text().operator==("")){ui->lineEdit_7->setText("");board[0][6]=-1;}
 if(!ui->lineEdit_8->text().operator==("")){ui->lineEdit_8->setText("");board[0][7]=-1;}
 if(!ui->lineEdit_9->text().operator==("")){ui->lineEdit_9->setText("");board[1][0]=-1;}
 if(!ui->lineEdit_10->text().operator==("")){ui->lineEdit_10->setText("");board[1][1]=-1;}
 if(!ui->lineEdit_11->text().operator==("")){ui->lineEdit_11->setText("");board[1][2]=-1;}
 if(!ui->lineEdit_12->text().operator==("")){ui->lineEdit_12->setText("");board[1][3]=-1;}
 if(!ui->lineEdit_13->text().operator==("")){ui->lineEdit_13->setText("");board[1][4]=-1;}
 if(!ui->lineEdit_14->text().operator==("")){ui->lineEdit_14->setText("");board[1][5]=-1;}
 if(!ui->lineEdit_15->text().operator==("")){ui->lineEdit_15->setText("");board[1][6]=-1;}
 if(!ui->lineEdit_16->text().operator==("")){ui->lineEdit_16->setText("");board[1][7]=-1;}
 if(!ui->lineEdit_17->text().operator==("")){ui->lineEdit_17->setText("");board[2][0]=-1;}
 if(!ui->lineEdit_18->text().operator==("")){ui->lineEdit_18->setText("");board[2][1]=-1;}
 if(!ui->lineEdit_19->text().operator==("")){ui->lineEdit_19->setText("");board[2][2]=-1;}
 if(!ui->lineEdit_20->text().operator==("")){ui->lineEdit_20->setText("");board[2][3]=-1;}
 if(!ui->lineEdit_21->text().operator==("")){ui->lineEdit_21->setText("");board[2][4]=-1;}
 if(!ui->lineEdit_22->text().operator==("")){ui->lineEdit_22->setText("");board[2][5]=-1;}
 if(!ui->lineEdit_23->text().operator==("")){ui->lineEdit_23->setText("");board[2][6]=-1;}
 if(!ui->lineEdit_24->text().operator==("")){ui->lineEdit_24->setText("");board[2][7]=-1;}
 if(!ui->lineEdit_25->text().operator==("")){ui->lineEdit_25->setText("");board[3][0]=-1;}
 if(!ui->lineEdit_26->text().operator==("")){ui->lineEdit_26->setText("");board[3][1]=-1;}
 if(!ui->lineEdit_27->text().operator==("")){ui->lineEdit_27->setText("");board[3][2]=-1;}
 if(!ui->lineEdit_28->text().operator==("")){ui->lineEdit_28->setText("");board[3][3]=-1;}
 if(!ui->lineEdit_29->text().operator==("")){ui->lineEdit_29->setText("");board[3][4]=-1;}
 if(!ui->lineEdit_30->text().operator==("")){ui->lineEdit_30->setText("");board[3][5]=-1;}
 if(!ui->lineEdit_31->text().operator==("")){ui->lineEdit_31->setText("");board[3][6]=-1;}
 if(!ui->lineEdit_32->text().operator==("")){ui->lineEdit_32->setText("");board[3][7]=-1;}
 if(!ui->lineEdit_33->text().operator==("")){ui->lineEdit_33->setText("");board[4][0]=-1;}
 if(!ui->lineEdit_34->text().operator==("")){ui->lineEdit_34->setText("");board[4][1]=-1;}
 if(!ui->lineEdit_35->text().operator==("")){ui->lineEdit_35->setText("");board[4][2]=-1;}
 if(!ui->lineEdit_36->text().operator==("")){ui->lineEdit_36->setText("");board[4][3]=-1;}
 if(!ui->lineEdit_37->text().operator==("")){ui->lineEdit_37->setText("");board[4][4]=-1;}
 if(!ui->lineEdit_38->text().operator==("")){ui->lineEdit_38->setText("");board[4][5]=-1;}
 if(!ui->lineEdit_39->text().operator==("")){ui->lineEdit_39->setText("");board[4][6]=-1;}
 if(!ui->lineEdit_40->text().operator==("")){ui->lineEdit_40->setText("");board[4][7]=-1;}
 if(!ui->lineEdit_41->text().operator==("")){ui->lineEdit_41->setText("");board[5][0]=-1;}
 if(!ui->lineEdit_42->text().operator==("")){ui->lineEdit_42->setText("");board[5][1]=-1;}
 if(!ui->lineEdit_43->text().operator==("")){ui->lineEdit_43->setText("");board[5][2]=-1;}
 if(!ui->lineEdit_44->text().operator==("")){ui->lineEdit_44->setText("");board[5][3]=-1;}
 if(!ui->lineEdit_45->text().operator==("")){ui->lineEdit_45->setText("");board[5][4]=-1;}
 if(!ui->lineEdit_46->text().operator==("")){ui->lineEdit_46->setText("");board[5][5]=-1;}
 if(!ui->lineEdit_47->text().operator==("")){ui->lineEdit_47->setText("");board[5][6]=-1;}
 if(!ui->lineEdit_48->text().operator==("")){ui->lineEdit_48->setText("");board[5][7]=-1;}
 if(!ui->lineEdit_49->text().operator==("")){ui->lineEdit_49->setText("");board[6][0]=-1;}
 if(!ui->lineEdit_50->text().operator==("")){ui->lineEdit_50->setText("");board[6][1]=-1;}
 if(!ui->lineEdit_51->text().operator==("")){ui->lineEdit_51->setText("");board[6][2]=-1;}
 if(!ui->lineEdit_52->text().operator==("")){ui->lineEdit_52->setText("");board[6][3]=-1;}
 if(!ui->lineEdit_53->text().operator==("")){ui->lineEdit_53->setText("");board[6][4]=-1;}
 if(!ui->lineEdit_54->text().operator==("")){ui->lineEdit_54->setText("");board[6][5]=-1;}
 if(!ui->lineEdit_55->text().operator==("")){ui->lineEdit_55->setText("");board[6][6]=-1;}
 if(!ui->lineEdit_56->text().operator==("")){ui->lineEdit_56->setText("");board[6][7]=-1;}
 if(!ui->lineEdit_57->text().operator==("")){ui->lineEdit_57->setText("");board[7][0]=-1;}
 if(!ui->lineEdit_58->text().operator==("")){ui->lineEdit_58->setText("");board[7][1]=-1;}
 if(!ui->lineEdit_59->text().operator==("")){ui->lineEdit_59->setText("");board[7][2]=-1;}
 if(!ui->lineEdit_60->text().operator==("")){ui->lineEdit_60->setText("");board[7][3]=-1;}
 if(!ui->lineEdit_61->text().operator==("")){ui->lineEdit_61->setText("");board[7][4]=-1;}
 if(!ui->lineEdit_62->text().operator==("")){ui->lineEdit_62->setText("");board[7][5]=-1;}
 if(!ui->lineEdit_63->text().operator==("")){ui->lineEdit_63->setText("");board[7][6]=-1;}
 if(!ui->lineEdit_64->text().operator==("")){ui->lineEdit_64->setText("");board[7][7]=-1;}
}

void MainWindow::on_pushButton_solve_clicked()
{
    readboard();
    char filename[1000];// = "D:\\1.cnf";
    strcpy(filename,buffer);
    FILE *out = fopen(filename, "wb");
    SudokuToSAT(board, out, cntnum);
    fclose(out);
    Solver S;
    int res;
    int literalnum = S.read(filename);
    if (literalnum == -1)res=-1;
    else res=S.solve();
    if(res==1){
        for(int i=0;i<8;i++)
            for(int j = 0;j<8;j++)
                board[i][j]=S.model[index(i,j)];
    }
    showboard();
}

void MainWindow::on_pushButton_generate_clicked()
{
    int show[8][8];
    for(int i =0;i<8;i++)
        for(int j=0;j<8;j++)
        {show[i][j]=-1;board[i][j]=-1;}
    generateBoard(show,6);
    char filename[1000];// = "D:\\1.cnf";
    strcpy(filename,buffer);
    FILE *out = fopen(filename, "wb");
    SudokuToSAT(show, out, 6);
    fclose(out);
    Solver S;
    int res;
    int literalnum = S.read(filename);
    if (literalnum == -1)res=-1;
    else res=S.solve();
    if(res==1){
        for(int i=0;i<8;i++)
            for(int j = 0;j<8;j++)
                show[i][j]=S.model[index(i,j)];
    }
    int nums=15;
    switch(groupButton1->checkedId())
    {
    case 0:
        nums=15;break;
    case 1:
        nums=25;break;
    case 2:
        nums=35;break;
    }
    time_t seed;
    srand(time(&seed));
    while (nums>0) {
        int m = rand() % M, n = rand() % M;
        if(board[m][n]==-1){
            board[m][n]=show[m][n];
            nums--;
        }
    }
    showboard();
}

void MainWindow::on_pushButton_judge_clicked()
{
    readboard();
    char filename[1000];// = "D:\\1.cnf";
    strcpy(filename,buffer);
    FILE *out = fopen(filename, "wb");
    SudokuToSAT(board, out, cntnum);
    fclose(out);
    Solver S;
    int res;
    int literalnum = S.read(filename);
    if (literalnum == -1)res=-2;
    else res=S.solve();
    switch(res){
    case 1:
        QMessageBox::about(NULL, "有解", "当前数独有解！");
        break;
    case 0:
    case -1:
        QMessageBox::critical(NULL, "错误", "当前数独无解！");
        break;
    case -2:
        QMessageBox::critical(NULL, "错误", "文件打开错误！");
        break;
    }
}

