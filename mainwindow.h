#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTextBrowser>
#include "aboutdialog.h"
#include "authenticatedialog.h"
#include "readercmd.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>
#include <QDateTime>

// 初始数据块数据
const unsigned char dftDataBlock[]{
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

const int keyAFlag=0x60;        // 密钥B的标志
const unsigned char dftKeyA[]={0x0,0x0,0x0,0x0,0x0,0x0};        // 初始密钥A
const unsigned char myKeyA[]={0xaa,0xbb,0xcc,0xdd,0xee,0xff};   // 修改后的密钥A
const int rcdAddress[]={5,6,8,9,10};    // 充值记录的块地址
const int balAddress=4;                 // 余额的块地址

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    aboutDialog *aboutDlg=NULL;             // 关于对话框
    authenticateDialog *mngDlg=NULL;        // 管理认证对话框
    authenticateDialog *consumDlg=NULL;     // 消费认证对话框

    ReaderCmd *readCmd;             // 读写器命令集
    QSqlDatabase db;                 // 数据库

    unsigned char* coupon;          // 余额-记录地址缓冲区
    QString workerNumber;          // 工作号缓冲区
    QString device;                 // 设备号
    unsigned char time[7];          // 时间缓冲区

    QTextBrowser *textBrowser;     // 控件
    QLineEdit *display;

    bool waitingForOperand;         // 等待操作数
    float sumSoFar;

    void init();
    void initBackground();
    bool updateReaderTime();
    int nextAddress(int address);
    bool fee();                 // 扣款
    void updateTimeBuffer();
    void message(QString msg);

private slots:
    bool refresh();             // 刷新
    void showHomePage();        // 显示主页
    void showInquirePage();     // 显示查询界面
    void showAboutDialog();     // 显示关于对话框
    void verifyMng();           // 管理者登录认证
    void verifyConsum();        // 消费机登录认证
    bool activateCard();        // 发卡
    bool reportLoss();          // 挂失
    bool fillCard();            // 补卡
    bool destroyCard();         // 销卡
    bool recharge();            // 充值
    bool inquire();             // 查询
    void digistClick();         // 输入数字
    void pointClick();          // 输入小数点
    void plusClick();           // 输入加号
    void clearClick();          // 输入清除
    void enterClick();          // 输入扣款
};

#endif // MAINWINDOW_H
