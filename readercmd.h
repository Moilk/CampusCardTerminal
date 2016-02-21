#ifndef READERCMD_H
#define READERCMD_H

#include <QSerialPortInfo>
#include <qextserial/qextserialport.h>
#include <QtDebug>
#include <QThread>

//延时，TIME_OUT是串口读写的延时
#define TIME_OUT 10

class ReaderCmd : public QObject
{
    Q_OBJECT
public:
    enum myConstant{
        BLOCK_LENGTH=16,        // S50卡数据块的长度
        KEY_LENGTH=6,           // 密钥的长度
        TIME_LENGTH=7,          // 时间的长度
        CTRL_WORLD_LENGTH=4,    // 控制字的长度
        CARD_ID_LENGTH=4        // 卡ID的长度
    };

    explicit ReaderCmd(QObject *parent = 0);
    ~ReaderCmd();

    bool connect();                     // 连接设备
    bool onlineRequest();               // 联机请求
    bool loadKey(unsigned char *key);       // 加载密钥
    unsigned char *readBlockData(unsigned char blockNum, unsigned char keyFlag);                       // 读取块数据
    bool writeBlockData(unsigned char blockNum, unsigned char keyFlag, unsigned char *data);      // 写入块数据
    bool modifyKey(unsigned char sector, unsigned char keyFlag, unsigned char mKeyFlag, unsigned char *newKey);  // 修改密钥
    unsigned char *readTime();                        // 读取时间
    bool writeTime(unsigned char *time);    // 写入时间
    unsigned char *readCardID();                      // 读取卡ID

private:
    QextSerialPort *myCom;
    unsigned char tmpData[30];

    unsigned char checkSum(unsigned char *buff, int len);         // 生成校验和

private slots:
    unsigned char *readMyCom();
};

#endif // READERCMD_H
