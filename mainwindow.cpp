#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // 读写器命令集初始化
    readCmd=new ReaderCmd;

    // mysql初始化
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("student_card");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1234");

    // 控件绑定
    textBrowser=ui->textBrowser;
    display=ui->edSum;

    // 窗口设置
    this->setWindowTitle(QStringLiteral("校园卡终端"));
    this->setWindowIcon(QIcon(":/new/ico/windowIco"));

    // 认证对话框初始化
    mngDlg=new authenticateDialog(this);
    mngDlg->lbAccount->setText(QStringLiteral("工 号:"));
    mngDlg->setWindowTitle(QStringLiteral("管理登录"));
    mngDlg->setWindowModality(Qt::ApplicationModal);
    consumDlg=new authenticateDialog(this);
    consumDlg->lbAccount->setText(QStringLiteral("设备号:"));
    consumDlg->setWindowTitle(QStringLiteral("消费登录"));
    consumDlg->setWindowModality(Qt::ApplicationModal);

    connect(consumDlg,SIGNAL(accepted()),this,SLOT(verifyConsum()));
    connect(mngDlg,SIGNAL(accepted()),this,SLOT(verifyMng()));


    // 设置action与右侧按钮的相应
    connect(ui->acHome,SIGNAL(triggered(bool)),this,SLOT(showHomePage()));
    connect(ui->acManage,SIGNAL(triggered(bool)),mngDlg,SLOT(show()));
    connect(ui->acConsum,SIGNAL(triggered(bool)),consumDlg,SLOT(show()));
    connect(ui->acInquire,SIGNAL(triggered(bool)),this,SLOT(showInquirePage()));
    connect(ui->acExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->acAbout,SIGNAL(triggered(bool)),this,SLOT(showAboutDialog()));
    connect(ui->btHome,SIGNAL(clicked(bool)),this,SLOT(showHomePage()));
    connect(ui->btManage,SIGNAL(clicked(bool)),mngDlg,SLOT(show()));
    connect(ui->btConsum,SIGNAL(clicked(bool)),consumDlg,SLOT(show()));
    connect(ui->btInquire,SIGNAL(clicked(bool)),this,SLOT(showInquirePage()));
    connect(ui->btRefresh,SIGNAL(clicked(bool)),this,SLOT(refresh()));

    // 左侧按钮
    connect(ui->btActivate,SIGNAL(clicked(bool)),this,SLOT(activateCard()));    // 发卡
    connect(ui->btLoss,SIGNAL(clicked(bool)),this,SLOT(reportLoss()));          // 挂失
    connect(ui->btFill,SIGNAL(clicked(bool)),this,SLOT(fillCard()));            // 补卡
    connect(ui->btDestroy,SIGNAL(clicked(bool)),this,SLOT(destroyCard()));      // 销卡
    connect(ui->btConfirm,SIGNAL(clicked(bool)),this,SLOT(recharge()));          // 充值
    connect(ui->btInquireConf,SIGNAL(clicked(bool)),this,SLOT(inquire()));      // 查询
    connect(ui->btEnter,SIGNAL(clicked(bool)),this,SLOT(enterClick()));                // 扣款
    connect(ui->bt0,SIGNAL(clicked(bool)),this,SLOT(digistClick()));            // 数字键
    connect(ui->bt1,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt2,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt3,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt4,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt5,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt6,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt7,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt8,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->bt9,SIGNAL(clicked(bool)),this,SLOT(digistClick()));
    connect(ui->btPoint,SIGNAL(clicked(bool)),this,SLOT(pointClick()));         // 小数点
    connect(ui->btPlus,SIGNAL(clicked(bool)),this,SLOT(plusClick()));           // 加号
    connect(ui->btClear,SIGNAL(clicked(bool)),this,SLOT(clearClick()));         // 清空

    // 主页界面
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/new/png/cover")));
    ui->pgWidget->setAutoFillBackground(true);
    ui->pgWidget->setPalette(palette);

    showHomePage();         // 显示主页

    initBackground();       // 进入主页时的后台操作
}

/**
 * @brief MainWindow::showHomePage 进入主页的操作
 */
void MainWindow::showHomePage()
{
    // 右侧按钮的设置
    ui->btManage->setDisabled(false);   // 按钮可用
    ui->btConfirm->setDisabled(false);
    ui->btInquire->setDisabled(false);
    ui->btRefresh->setVisible(false);   // 按钮不可见

    ui->stackedWidget->setCurrentIndex(0);
}

/**
 * @brief MainWindow::showInquirePage 进入查询界面的操作
 */
void MainWindow::showInquirePage()
{
    // 右侧按钮的设定
    ui->btManage->setDisabled(false);
    ui->btConsum->setDisabled(false);
    ui->btInquire->setDisabled(true);
    ui->btRefresh->setVisible(false);

    ui->stackedWidget->setCurrentIndex(3);
    textBrowser->setText(QStringLiteral("进入查询界面成功!\n"));
}

/**
 * @brief MainWindow::verifyMng 进入管理界面的操作
 *
 * 身份认证->读取卡ID
 */
void MainWindow::verifyMng()
{
    // 身份认证
    if(mngDlg!=NULL){
        workerNumber=mngDlg->edAccount->text();
        QString passwd=mngDlg->edKey->text();

        if(workerNumber.compare("1234")==0&&passwd.compare("1234")==0){
            // 右侧按钮设置
            ui->btManage->setDisabled(true);
            ui->btConsum->setDisabled(false);
            ui->btInquire->setDisabled(false);
            ui->btRefresh->setVisible(true);

            // 进入管理界面
            ui->stackedWidget->setCurrentIndex(1);
            textBrowser->setText(QStringLiteral("进入管理界面成功!\n"));

            refresh();
        }else{      // 认证失败
            textBrowser->setText(QStringLiteral("管理认证失败,请检查您的账号和密码."));
        }
    }
}

/**
 * @brief MainWindow::verifyConsum 进入消费界面的操作
 *
 * 身份认证->读卡ID->读取余额
 */
void MainWindow::verifyConsum()
{
    waitingForOperand=true;
    sumSoFar=0.0;
    if(consumDlg!=NULL){
        device=consumDlg->edAccount->text();
        QString passwd=consumDlg->edKey->text();

        if(device.compare("0001")==0&&passwd.compare("1234")==0){
            ui->stackedWidget->setCurrentIndex(2);
            textBrowser->setText(QStringLiteral("进入消费界面成功!\n"));
            ui->btManage->setDisabled(false);
            ui->btConsum->setDisabled(true);
            ui->btInquire->setDisabled(false);
            ui->btRefresh->setVisible(true);

            refresh();
        }else{  // 认证失败
            textBrowser->setText(QStringLiteral("消费认证失败,请检查您的账号和密码."));
        }
    }
}

/**
 * @brief MainWindow::refresh 刷新
 * @return bool
 * 读取卡ID->加载密钥->读取余额
 */
bool MainWindow::refresh()
{
    unsigned char*cardID=readCmd->readCardID();
    if(cardID!=NULL){
        QByteArray tmp=QByteArray::fromRawData((char *)cardID,ReaderCmd::CARD_ID_LENGTH).toHex();
        ui->edCardID->setText(tmp);
        ui->edCardID2->setText(tmp);
        ui->edCardID3->setText(tmp);
        textBrowser->setText(QStringLiteral("读取卡ID成功!\n"));
        if(readCmd->loadKey((unsigned char *)myKeyA)){
            message(QStringLiteral("加载密钥成功!\n"));
            coupon=readCmd->readBlockData(balAddress,keyAFlag);
            if(coupon!=NULL){
                float del=((float)(0x100*coupon[0]+coupon[1]))/100;
                ui->edMoney->setText(QString::number(del,'f',2));
                ui->edMoney2->setText(QString::number(del,'f',2));
                ui->btEnter->setDisabled(false);
                message(QStringLiteral("读取余额成功!\n"));
                return true;
            }else{
                ui->edMoney->clear();
                ui->edMoney2->clear();
                message(QStringLiteral("读取余额失败,请放上有效卡后刷新.\n"));
            }
        } else {
            ui->edMoney->clear();
            ui->edMoney2->clear();
            message(QStringLiteral("加载密钥失败.\n"));
        }
    }else{
        ui->edMoney->clear();
        ui->edMoney2->clear();
        ui->edCardID->setText(QStringLiteral("卡ID号"));
        ui->edCardID2->setText(QStringLiteral("卡ID号"));
        ui->edCardID3->setText(QStringLiteral("卡ID号"));
        textBrowser->setText(QStringLiteral("读取卡ID失败,请放上有效卡后刷新.\n"));
    }
    ui->btEnter->setDisabled(true);

    return false;
}

/**
 * @brief MainWindow::showAboutDialog 显示关于对话框
 */
void MainWindow::showAboutDialog()
{
    if(!aboutDlg){
        aboutDlg=new aboutDialog(this);
    }
    aboutDlg->setWindowIcon(QIcon(":/new/ico/aboutIco"));
    aboutDlg->setWindowTitle(QStringLiteral("关于"));
    aboutDlg->setStyleSheet("background:#d3e4e1");
    aboutDlg->show();

    // 在主窗口中间显示
    int x =this->x() + (this->width() - aboutDlg->width()) / 2;
    int y =this->y() + (this->height() - aboutDlg->height()) / 2;
    aboutDlg->move(x, y);
}

/**
 * @brief MainWindow::initBackground 启动程序的后台操作
 *
 * 连接读写器->连接服务器->同步读写器时间
 */
void MainWindow::initBackground(){
    if(readCmd->connect()){
        textBrowser->setText(QStringLiteral("读写器连接成功!\n"));
        if(db.open()){
            message(QStringLiteral("数据库连接成功!\n"));
            if(updateReaderTime()){
                message(QStringLiteral("同步读写器时间成功!\n"));
                message(QStringLiteral("启动成功!\n"));
                return;
            }else{
                message(QStringLiteral("同步读写器时间失败,请检查连接后重启应用.\n"));
            }
        } else {
            message(QStringLiteral("数据库连接失败,请检查连接后重启应用.\n"));
        }
    } else {
        textBrowser->setText(QStringLiteral("读写器连接失败,请检查连接后重启应用.\n"));
    }
    message(QStringLiteral("启动失败.\n"));
}

/**
 * @brief MainWindow::updateTime 更新读写器时间
 * @return bool
 *
 * 获取上位机时间->格式转换->写入读写器时间
 */
bool MainWindow::updateReaderTime()
{
    updateTimeBuffer();
    readCmd->writeTime(time);

    return true;
}

/**
 * @brief MainWindow::activateCard 发卡
 * @return bool
 *
 * 卡已填->姓名学号性别已填->写卡->写数据库
 */
bool MainWindow::activateCard()
{
    QString cardID=ui->edCardID->text();
    if(!cardID.isEmpty()){
        QString name=ui->edName->text();
        QString stuID=ui->edStuID->text();
        QString sex="";
        if(ui->rbtMan->isChecked())
            sex=QStringLiteral("男");
        if(ui->rbtGirl->isChecked())
            sex=QStringLiteral("女");
        if(!(name.isEmpty()||stuID.isEmpty()||sex.isEmpty())){
            if(readCmd->loadKey((unsigned char *)dftKeyA)){
                unsigned char data[]={0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,
                                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
                // 写卡
                bool ok=readCmd->writeBlockData(balAddress,keyAFlag,data)
                        &&readCmd->modifyKey(0x01,keyAFlag,keyAFlag,(unsigned char*)myKeyA)
                        &&readCmd->modifyKey(0x02,keyAFlag,keyAFlag,(unsigned char*)myKeyA);
                if(ok){
                    message(QStringLiteral("写卡成功!\n"));
                    // 写数据库
                    QSqlQuery *query1=new QSqlQuery(db);
                    QSqlQuery *query2=new QSqlQuery(db);

                    query1->prepare("INSERT INTO student (student_number, student_name, student_sex) VALUES (:student_number, :student_name, :student_sex)");
                    query1->bindValue(":student_number",stuID);
                    query1->bindValue(":student_name",name);
                    query1->bindValue(":student_sex",sex);

                    query2->prepare("INSERT INTO card_basic (card_number, student_number, card_time, card_validity, equipment_number, worker_number, balance) VALUES (:card_number, :student_number, :card_time, :card_validity, :equipment_number, :worker_number, :balance)");
                    query2->bindValue(":card_number",ui->edCardID->text());
                    query2->bindValue(":student_number",stuID);

                    QDateTime time=QDateTime::currentDateTime();
                    QString cardTime=time.toString("yyyyMMdd");
                    int val=(cardTime.toInt(0,10)/10000+4)*10000+630;
                    QString cardVal=QString::number(val,10);
                    query2->bindValue(":card_time",cardTime);

                    query2->bindValue(":card_validity",cardVal);
                    query2->bindValue(":equipment_number",workerNumber);
                    query2->bindValue(":worker_number",workerNumber);
                    query2->bindValue(":balance","0.00");

                    bool ok2 = query1->exec()&&query2->exec();
                    if(ok2){
                        message(QStringLiteral("写数据库成功!\n"));
                        return true;
                    }else{
                        message(QStringLiteral("写数据库失败.\n"));
                    }
                } else {
                    message(QStringLiteral("写卡失败.\n"));
                }
            }
        } else {
            message(QStringLiteral("请完善基本资料.\n"));
        }
    } else {
        message(QStringLiteral("请放上有效卡后刷新.\n"));
    }
    return false;
}

/**
 * @brief MainWindow::reportLoss 挂失
 * @return bool
 *
 * 姓名学号已填->数据库中能找到->改卡号为全F
 */
bool MainWindow::reportLoss()
{
    QString name=ui->edName->text();
    QString stuID=ui->edStuID->text();
    if(!(name.isEmpty()||stuID.isEmpty())){ // 检查学号姓名是否已填
        QSqlQuery *query=new QSqlQuery(db);

        query->prepare("select * from student where student_number=:studentNumber and student_name=:studentName");
        query->bindValue(":studentNumber",stuID);
        query->bindValue(":studentName",name);
        query->exec();

        if(query->size()>=1){
            QSqlQuery *query2=new QSqlQuery(db);
            query2->prepare("UPDATE card_basic SET card_number = 'ffffffff' WHERE student_number=:studentNum");
            query2->bindValue(":studentNum",stuID);
            if(query2->exec()){
                message(QStringLiteral("挂失成功!\n"));
                return true;
            } else {
                message(QStringLiteral("挂失失败.\n"));
            }
        } else {
            message(QStringLiteral("认证失败.\n"));
        }
    } else {    // 如果未填
        message(QStringLiteral("请填写学号姓名.\n"));
    }

    return false;
}

/**
 * @brief MainWindow::fillCard
 * @return bool
 *
 * 卡ID已填->姓名学号已填->认证成功->写卡->写库
 */
bool MainWindow::fillCard()
{
    QString cardID=ui->edCardID->text();
    QString name=ui->edName->text();
    QString stuID=ui->edStuID->text();
    if(!(cardID.isEmpty()||name.isEmpty()||stuID.isEmpty())){
        QSqlQuery *query=new QSqlQuery(db);
        query->prepare("select * from student where student_number=:studentNumber and student_name=:studentName");
        query->bindValue(":studentName",name);
        query->bindValue(":studentNumber",stuID);
        query->exec();
        if(query->size()>=1){
            QSqlQuery *query2=new QSqlQuery(db);
            query2->prepare("select * from card_basic where student_number=:studentNumber");
            query2->bindValue(":studentNumber",stuID);
            query2->exec();
            query2->next();
            int bal=(int)(query2->value("balance").toFloat()*100);
            unsigned char packet[]{
                0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
            };
            packet[0]=bal/256;
            packet[1]=bal%256;
            bool ok=readCmd->loadKey((unsigned char*)dftKeyA)
                    &&readCmd->writeBlockData(balAddress,keyAFlag,packet)
                    &&readCmd->modifyKey(0x01,keyAFlag,keyAFlag,(unsigned char*)myKeyA)
                    &&readCmd->modifyKey(0x02,keyAFlag,keyAFlag,(unsigned char*)myKeyA);
            if(ok){
                message(QStringLiteral("写卡成功!\n"));
                QSqlQuery *query3=new QSqlQuery(db);
                query3->prepare("UPDATE card_basic SET card_number = :cardID WHERE student_number=:studentNum");
                query3->bindValue(":studentNum",stuID);
                query3->bindValue(":cardID",cardID);
                if(query3->exec()){
                    message(QStringLiteral("写库成功!\n"));
                }else{
                    message(QStringLiteral("写库失败.\n"));
                }
            }else{
                message(QStringLiteral("写卡失败.\n"));
            }
        }else{
            message(QStringLiteral("未找到学生信息.\n"));
        }
    }else{
        message(QStringLiteral("请填写姓名学号并放上有效卡后刷新.\n"));
    }

    return false;
}

/**
 * @brief MainWindow::destroyCard 销卡
 * @return bool
 *
 * 卡ID已填->写卡清空
 */
bool MainWindow::destroyCard()
{
    QString cardID=ui->edCardID->text();
    if(!(cardID.isEmpty()||ui->edMoney->text().isEmpty())){
        bool ok[8];
        for(int i=4;i<11;i++){          // 数据块数据初始化
            if(i!=7)
                ok[i-4]=readCmd->writeBlockData(i,keyAFlag,(unsigned char *)dftDataBlock);
        }
        ok[3]=readCmd->modifyKey(0x1,keyAFlag,keyAFlag,(unsigned char *)dftKeyA);
        ok[7]=readCmd->modifyKey(0x2,keyAFlag,keyAFlag,(unsigned char *)dftKeyA);
        if(ok[0]&&ok[1]&&ok[2]&&ok[3]&&ok[4]&&ok[5]&&ok[6]&&ok[7]){
            message(QStringLiteral("卡初始化成功!\n"));
            return true;
        }else {
            message(QStringLiteral("卡初始化失败.\n"));
        }
    } else {
        message(QStringLiteral("请放上有效卡后刷新.\n"));
    }
    return false;
}

/**
 * @brief MainWindow::recharge 充值
 * @return bool
 *
 * 卡号已填->余额已填->充值金额正确->库中找到此卡->写卡->写库
 */
bool MainWindow::recharge()
{
    QString cardID=ui->edCardID2->text();
    QString money=ui->edMoney->text();
    if(!(cardID.isEmpty()||money.isEmpty())){
        QString amount=ui->edAmount->text();
        if(!amount.isEmpty()){
            // 写卡
            float amt=amount.toFloat();
            float mny=money.toFloat();
            int bal[2];
            bal[0]=((int)((amt+mny)*100))/256;
            bal[1]=((int)((amt+mny)*100))%256;
            unsigned char packet[]={
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
            };
            packet[0]=bal[0];
            packet[1]=bal[1];
            packet[2]=coupon[2];
            if(readCmd->writeBlockData(balAddress,keyAFlag,packet)){
                message(QStringLiteral("写卡成功!\n"));
                // 写数据库
                QSqlQuery *query=new QSqlQuery(db);
                query->prepare("UPDATE card_basic SET balance=:bal WHERE card_number=:cardNum");
                query->bindValue(":bal",QString::number(amt+mny,'f',2));
                query->bindValue(":cardNum",cardID);
                QSqlQuery *query2=new QSqlQuery(db);
                query2->prepare("INSERT INTO card_recharge (card_number, recharge_amount, recharge_time, equipment_number, worker_number) VALUES (:card_number, :recharge_amount, :recharge_time, :equipment_number, :worker_number)");
                query2->bindValue(":card_number",cardID);
                query2->bindValue(":recharge_amount",QString::number(amt,'f',2));
                QDateTime tmpTime=QDateTime::currentDateTime();
                query2->bindValue(":recharge_time",tmpTime.toString("yyyy-MM-dd-hh-mm"));
                query2->bindValue(":equipment_number",workerNumber);
                query2->bindValue(":worker_number",workerNumber);

                if(query->exec()&&query2->exec()){
                    message(QStringLiteral("写卡数据库成功!\n"));
                    return true;
                }else{
                    message(QStringLiteral("写卡数据库失败.\n"));
                }
            }else{
                message(QStringLiteral("写卡失败.\n"));
            }
        }else{
            message(QStringLiteral("请填写充值金额.\n"));
        }
    }else{
        message(QStringLiteral("请放上有效卡后刷新.\n"));
    }
    return false;
}

/**
 * @brief MainWindow::inquire 查询
 * @return bool
 *
 * 学号姓名已填->数据库认证成功->找到对应表项->生成数据表
 */
bool MainWindow::inquire()
{
    QString name=ui->edName2->text();
    QString stuID=ui->edStuID2->text();
    // 确认学号姓名是否已填
    if(!(name.isEmpty()||stuID.isEmpty())){
        QSqlQuery *query=new QSqlQuery(db);
        query->prepare("select * from student where student_number=:studentNumber and student_name=:studentName");
        query->bindValue(":studentName",name);
        query->bindValue(":studentNumber",stuID);
        query->exec();
        // 在数据库中是否能找到此账号
        if(query->size()>=1){
            message(QStringLiteral("找到记录!\n"));
            QSqlQuery *query2=new QSqlQuery(db);
            query2->prepare("select * from card_basic where student_number=:studentNumber");
            query2->bindValue(":studentNumber",stuID);
            query2->exec();
            if(query2->next()){
                QString cardID=query2->value("card_number").toString();     // 得到卡号
                ui->edCardID4->setText(cardID);
                qDebug()<<QStringLiteral("查询    得到卡号:")<<cardID;
                // 根据卡号找消费记录
                QSqlQueryModel *model=new QSqlQueryModel;
                QSqlQuery *query3=new QSqlQuery(db);
                query3->prepare("select * from student_purchase join equipment on student_purchase.equipment_number = equipment.equipment_number where card_number =:cardNum order by purchase_time desc");
                query3->bindValue(":cardNum",cardID);
                query3->exec();

                model->setQuery(*query3);
                // 移除多余的项
                model->removeColumn(0);
                model->removeColumn(2);
                model->removeColumn(2);
                model->setHeaderData(0,Qt::Horizontal,QStringLiteral("金额(元)"));
                model->setHeaderData(1,Qt::Horizontal,QStringLiteral("时间"));
                model->setHeaderData(2,Qt::Horizontal,QStringLiteral("地点"));

                ui->tableView->setModel(model);
            }else{
                message(QStringLiteral("获取卡号失败.\n"));
            }
        }else{
            message(QStringLiteral("账户信息未找到.\n"));
        }
    }else{
        message(QStringLiteral("请填写姓名学号.\n"));
    }
    return false;
}

/**
 * @brief MainWindow::fee 按下扣款键
 * @return bool
 */
void MainWindow::enterClick()
{
    if(!waitingForOperand){
        sumSoFar+=(float)((int)(display->text().toFloat()*100))/100;
        display->setText(QString::number(sumSoFar));
    }
    sumSoFar=0.0;
    waitingForOperand=true;
    fee();      // 扣款
}

/**
 * @brief MainWindow::fee 扣款
 * @return bool
 *
 * 卡ID已填->余额已填->余额>=0->写卡->写库->更新余额显示
 */
bool MainWindow::fee()
{
    QString cardID=ui->edCardID3->text();
    QString money=ui->edMoney2->text();
    bool ok[2];
    if(!(cardID.isEmpty()||money.isEmpty())){
        QSqlQuery *query3=new QSqlQuery(db);
        query3->prepare("select * from card_basic where card_number=:cardNumber");
        query3->bindValue(":cardNumber",cardID);
        query3->exec();
        if(query3->size()>=1){
            int mny=(int)((money.toFloat()-display->text().toFloat())*100);
            if(mny>=0){
                unsigned char packet[]{
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
                };
                // 写余额
                packet[0]=mny/256;
                packet[1]=mny%256;
                int address=nextAddress(coupon[2]);
                packet[2]=address;
                ok[0]=readCmd->writeBlockData(balAddress,keyAFlag,packet);
                // 写记录
                int account=(int)(display->text().toFloat()*100);
                packet[0]=account/256;      // 消费金额
                packet[1]=account%256;
                updateTimeBuffer();         // 更新时间缓冲区
                for(int i=0;i<7;i++){
                    packet[i+2]=time[i];
                }
                int deviceNo=device.toInt();
                packet[9]=deviceNo/256;
                packet[10]=deviceNo%256;
                ok[1]=readCmd->writeBlockData(address,keyAFlag,packet);
                if(ok[0]&&ok[1]){
                    // 写库
                    QSqlQuery *query=new QSqlQuery(db);
                    query->prepare("UPDATE card_basic SET balance=:bal WHERE card_number=:cardNum");
                    query->bindValue(":bal",QString::number(((float)mny)/100,'f',2));
                    query->bindValue(":cardNum",cardID);

                    QSqlQuery *query2=new QSqlQuery(db);
                    query2->prepare("INSERT INTO student_purchase (card_number, purchase_amount, purchase_time, equipment_number) VALUES (:card_number, :purchase_amount, :purchase_time, :equipment_number)");
                    query2->bindValue(":card_number",cardID);
                    query2->bindValue(":purchase_amount",QString::number(display->text().toFloat(),'f',2));
                    QDateTime tmpTime=QDateTime::currentDateTime();
                    query2->bindValue(":purchase_time",tmpTime.toString("yyyy-MM-dd-hh-mm"));
                    query2->bindValue(":equipment_number",device);

                    if(query->exec()&&query2->exec()){
                        message(QStringLiteral("写库成功!\n"));
                        ui->edMoney2->setText(QString::number((float)mny/100,'f',2));
                        ui->btEnter->setDisabled(true);
                        return true;
                    }else{
                        message(QStringLiteral("写库失败.\n"));
                    }
                }else{
                    message(QStringLiteral("写卡失败.\n"));
                }
            }else{
                message(QStringLiteral("余额不足.\n"));
            }
        }else{
            message(QStringLiteral("卡未激活.\n"));
        }
    }else{
        message(QStringLiteral("请放上有效卡后刷新.\n"));
    }
    return false;
}

/**
 * @brief MainWindow::digistClick 按下数字键
 */
void MainWindow::digistClick()
{
    QPushButton *btnClicked=qobject_cast<QPushButton *>(sender());
    int value=btnClicked->text().toInt();
    if(display->text()=="0"&&value==0)
        return;
    if(waitingForOperand){
        display->clear();
        waitingForOperand=false;
    }
    display->setText(display->text() + QString::number(value));
}

/**
 * @brief MainWindow::pointClick 按下小数点键
 */
void MainWindow::pointClick()
{
    if(waitingForOperand)
        display->setText("0");
    if(!display->text().contains("."))
        display->setText(display->text()+tr("."));
    waitingForOperand = false;
}

/**
 * @brief MainWindow::plusClick 按下加号
 */
void MainWindow::plusClick()
{
    waitingForOperand=true;
    sumSoFar+=(float)((int)(display->text().toFloat()*100))/100;
    display->setText(QString::number(sumSoFar));
}

/**
 * @brief MainWindow::clearClick 按下清空键
 */
void MainWindow::clearClick()
{
    sumSoFar=0.0;
    display->setText("0");
    waitingForOperand=true;
}

/**
 * @brief MainWindow::nextAddress 获取下一个充值记录的地址
 * @param address
 * @return int
 *
 * 5-6-8-9-10
 */
int MainWindow::nextAddress(int address)
{
    if(address==5||address==8||address==9)
        return (address+1);
    if(address==6)
        return 8;

    return 5;
}

/**
 * @brief MainWindow::getCurrentTime 更
 */
void MainWindow::updateTimeBuffer()
{
    QDateTime currentTime=QDateTime::currentDateTime();
    QString str=currentTime.toString("s m h d M dddd yy");
    QStringList timeList=str.split(" ");
    for(int i=0;i<7;i++){
        int tmp=((QString)timeList.at(i)).toInt();
        time[i]=(tmp/10)*16+(tmp%10);       // 转成BCD码
    }
    // 星期转换成功对应的数值
    QString week=(QString)timeList.at(5);
    time[5]=1;
    if(week.compare(QStringLiteral("星期二")))
        time[5]=2;
    if(week.compare(QStringLiteral("星期三")))
        time[5]=3;
    if(week.compare(QStringLiteral("星期四")))
        time[5]=4;
    if(week.compare(QStringLiteral("星期五")))
        time[5]=5;
    if(week.compare(QStringLiteral("星期六")))
        time[5]=6;
    if(week.compare(QStringLiteral("星期日")))
        time[5]=7;
}

/**
 * @brief MainWindow::message 更新提示信息
 */
void MainWindow::message(QString msg)
{
    textBrowser->setText(textBrowser->document()->toPlainText()+msg);
}
