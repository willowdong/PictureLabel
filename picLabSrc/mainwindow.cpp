#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    /*连接函数与动作*/
    connect(ui->open_action,&QAction::triggered,ui->label,&MyLabel::OnSelectImage);
    connect(ui->save_action,&QAction::triggered,ui->label,&MyLabel::OnSave);
    connect(ui->clear_action,&QAction::triggered,ui->label,&MyLabel::OnClearLab);
    connect(ui->reset_action,&QAction::triggered,ui->label,&MyLabel::OnPresetImage);
    connect(ui->out_action,&QAction::triggered,ui->label,&MyLabel::OnZoomOutImage);
    connect(ui->in_action,&QAction::triggered,ui->label,&MyLabel::OnZoomInImage);
    connect(ui->dir_action,&QAction::triggered,ui->label,&MyLabel::OnOpenDir);
    connect(ui->next_action,&QAction::triggered,ui->label,&MyLabel::OnNext);
    connect(ui->pre_action,&QAction::triggered,ui->label,&MyLabel::OnPre);
    connect(ui->load_action,&QAction::triggered,ui->label,&MyLabel::OnLoad);
    connect(ui->actionType,&QAction::triggered,ui->label,&MyLabel::OnChange);

    QLabel *status=new QLabel(this);                                                    //状态栏信息更新
    QString info ="就绪";
    status->setText(info);
    QFont font ("Microsoft YaHei", 10, 75);
    status->setFont(font);
    ui->statusBar->addWidget(status);
    connect(ui->label,&MyLabel::_status,this,[=](){
        status->setText(ui->label->infor);
    });

    connect(ui->about_action,&QAction::triggered,this,[=](){                            //软件信息
        QMessageBox::about(this,"软件信息","作者:董威龙\n用于标注图片并添加标签\n");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

