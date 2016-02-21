#include "readercmd.h"
#include <iostream>

ReaderCmd::ReaderCmd(QObject *parent) : QObject(parent)
{
}

ReaderCmd::~ReaderCmd()
{
    if(&myCom != NULL){
        if(myCom->isOpen()){
            myCom->close();
            qDebug() << QStringLiteral("设备已断开.");
        }
        delete &myCom;
    }
}

/**
 * @brief ReaderCmd::connect 连接可用串口
 * @return bool
 */
bool ReaderCmd::connect()
{
    QSerialPortInfo* info = new QSerialPortInfo;
    QList<QSerialPortInfo> * portList = new QList<QSerialPortInfo>;
    *portList = QSerialPortInfo::availablePorts();
    if(!portList->isEmpty()){
        foreach (*info, *portList)
        {
            qDebug() << "Name        : " << info->portName();
            qDebug() << "Description : " << info->description();
            qDebug() << "Manufacturer: " << info->manufacturer();

            myCom=new QextSerialPort(info->portName());
            if (myCom->open(QIODevice::ReadWrite))
            {
                qDebug() << QStringLiteral("设备连接成功！");

                myCom->setBaudRate(BAUD9600);
                myCom->setDataBits(DATA_8);
                myCom->setParity(PAR_NONE);
                myCom->setStopBits(STOP_1);
                myCom->setFlowControl(FLOW_OFF);
                myCom->setTimeout(TIME_OUT);
                return true;
            }
            qDebug()<<QStringLiteral("设备连接失败");
            return false;
        }
    }
    qDebug()<<QStringLiteral("设备连接失败");
    return false;
}

/**
 * @brief ReaderCmd::readMyCom 读取串口数据
 */
unsigned char* ReaderCmd::readMyCom()
{
    QByteArray temp=myCom->readAll();
    if(!temp.isEmpty()){                            // 如果接收报文非空
        qDebug()<<QStringLiteral("接收:")<<temp.toHex();
        // 将报文转为unsigned char暂存tmpData中
        for(int i=0;i<temp.count();i++){
            tmpData[i]=(unsigned char)temp.at(i);
        }

        // 校验报文是否出错
        unsigned check=checkSum(tmpData,temp.count());
        if(check==0x00||check==0x10){

            return tmpData;
        }else{  // 报文出错
            qDebug()<<QStringLiteral("接收报文校验失败.");

            return NULL;
        }
    }
    qDebug()<<QStringLiteral("接收报文为空.");    // 报文为空

    return NULL;
}

/**
 * @brief ReaderCmd::checkSum 生成校验和
 * @param buff 原帧缓冲区
 * @return 校验结果
 *
 * 帧头帧尾不参加校验
 */
unsigned char ReaderCmd::checkSum(unsigned char *buff,int len)
{
    unsigned char sum=0;

    for(int i=2;i<len-2;i++){
        sum+=buff[i];
    }
    sum=(~sum)+1;
    if(sum>0xf0){
        sum-=0x10;
    }

    return sum;
}

/**
 * @brief ReaderCmd::onlineRequest 联机请求
 * @return bool
 *
 * 报文格式:F1 1F FF FF 53 54 CheckSum F2 2F
 */
bool ReaderCmd::onlineRequest()
{
    // 预置的报文格式
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0x53,0x54,0x00,0xf2,0x2f
    };
    buff[6]=checkSum(buff,sizeof(buff));    // 填充校验和
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));

    myCom->write(packet);                   // 发送报文
    qDebug()<<QStringLiteral("联机请求发送:")<<packet.toHex();

    QThread::msleep(150);                   // 停滞0.15秒
    unsigned char *bckPacket=readMyCom();   // 接收数据

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<("联机请求: true");

        return true;                        // 返回联机成功
    }
    qDebug()<<QStringLiteral("联机请求: false");

    return false;                           // 返回联机失败
}

/**
 * @brief ReaderCmd::loadKey 加载密钥
 * @param key 6字节密钥(6B)
 * @return bool
 *
 * 报文格式:F1 1F FF FF C1 C1 06 key CheckSum F2 2F
 */
bool ReaderCmd::loadKey(unsigned char *key)
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc1,0xc1,0x06,
        0xff,0xff,0xff,0xff,0xff,0xff,
        0x00,0xf2,0x2f
    };
    for(int i=0;i<6;i++){
        buff[i+7]=key[i];
    }
    buff[13]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("加载密钥发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("加载密钥: true");

        return true;
    }
    qDebug()<<QStringLiteral("加载密钥: false");

    return false;
}

/**
 * @brief ReaderCmd::readBlockData 读取块数据
 * @param blockNum 块号地址(1B)
 * @param keyFlag 用密钥A或B验证(60/61)
 * @return bool
 *
 * 报文格式:F1 1F FF FF C2 C2 02 bolckNum keyFlag CheckSum F2 2F
 */
unsigned char* ReaderCmd::readBlockData(unsigned char blockNum,unsigned char keyFlag)
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc2,0xc2,0x02,
        0x00,0x60,
        0x00,0xf2,0x2f
    };
    buff[7]=blockNum;
    buff[8]=keyFlag;
    buff[9]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("读取块数据发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckpacket=readMyCom();

    if(bckpacket!=NULL&&bckpacket[6]==0x01){
        qDebug()<<QStringLiteral("读取块数据: 成功");

        return bckpacket+8;
    }
    qDebug()<<QStringLiteral("读取块数据: 失败");

    return NULL;
}

/**
 * @brief ReaderCmd::writeBlockData 写入块数据
 * @param blockNum 块号地址(1B)
 * @param keyFlag 用密钥A或B验证(60/61)
 * @param data 数据(16B)
 * @return bool
 *
 * 报文格式:F1 1F FF FF C3 C3 12 blockNum keyFlag data CheckSum F2 2F
 */
bool ReaderCmd::writeBlockData(unsigned char blockNum,unsigned char keyFlag,unsigned char *data)
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc3,0xc3,0x12,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0xf2,0x2f
    };
    buff[7]=blockNum;
    buff[8]=keyFlag;
    for(int i=0;i<16;i++){
        buff[i+9]=data[i];
    }
    buff[25]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("写入块数据发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("写入块数据: true");

        return true;
    }
    qDebug()<<QStringLiteral("写入块数据: false");

    return true;
}

/**
 * @brief ReaderCmd::modifyKey 修改密钥
 * @param sector 块号地址(1B)
 * @param keyFlag 用密钥A或B验证(60/61)
 * @param mKeyFlag 要修改的密钥(60/61)
 * @param newKey 新密钥(6B)
 * @return bool
 *
 * 报文格式:F1 1F FF FF C4 C4 09 sector keyFlag mKeyFlag newKey CheckSum F2 2F
 */
bool ReaderCmd::modifyKey(unsigned char sector,unsigned char keyFlag,unsigned char mKeyFlag,unsigned char *newKey)
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc4,0xc4,0x09,0x00,0x60,0x60,
        0xff,0xff,0xff,0xff,0xff,0xff,
        0x00,0xf2,0x2f
    };
    buff[7]=sector;
    buff[8]=keyFlag;
    buff[9]=mKeyFlag;
    for(int i=0;i<6;i++){
        buff[i+10]=newKey[i];
    }
    buff[16]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("修改密钥发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("修改密钥: true");

        return true;
    }
    qDebug()<<QStringLiteral("修改密钥: false");

    return false;
}

/**
 * @brief ReaderCmd::readTime 读取时间
 * @return bool
 *
 * 报文格式:F1 1F FF FF C6 C6 CheckSum F2 2F
 */
unsigned char* ReaderCmd::readTime()
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc6,0xc6,0x00,0xf2,0x2f
    };
    buff[6]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("读取时间发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("读取时间: 成功");

        return bckPacket+8;
    }
    qDebug()<<QStringLiteral("读取时间: 失败");

    return NULL;
}

/**
 * @brief ReaderCmd::writeTime 写入时间
 * @param time BCD码表示的时间(7B)
 * @return bool
 *
 * 报文格式:F1 1F FF FF C5 C5 07 秒 分 时 日 月 周 年 CheckSum F2 F
 */
bool ReaderCmd::writeTime(unsigned char *time)
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc5,0xc5,0x07,
        0x00,0x40,0x15,0x10,0x12,0x04,0x15,
        0x00,0xf2,0x2f
    };
    for(int i=0;i<7;i++){
        buff[i+7]=time[i];
    }
    buff[14]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("写入时间发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("写入时间: true");

        return true;
    }
    qDebug()<<QStringLiteral("写入时间: false");

    return false;
}

/**
 * @brief ReaderCmd::readCardID 读取卡ID
 * @return bool
 *
 * 报文格式:F1 1F FF FF C7 C7 CheckSum F2 2F
 */
unsigned char* ReaderCmd::readCardID()
{
    unsigned char buff[]={
        0xf1,0x1f,0xff,0xff,0xc7,0xc7,0x00,0xf2,0x2f
    };
    buff[6]=checkSum(buff,sizeof(buff));
    QByteArray packet=QByteArray::fromRawData((char *)buff,sizeof(buff));
    qDebug()<<QStringLiteral("读取卡ID发送:")<<packet.toHex();

    myCom->write(packet);
    QThread::msleep(150);
    unsigned char *bckPacket=readMyCom();

    if(bckPacket!=NULL&&bckPacket[6]==0x01){
        qDebug()<<QStringLiteral("读取卡ID: 成功");

        return bckPacket+8;
    }
    qDebug()<<QStringLiteral("读取卡ID: 失败");

    return NULL;
}
