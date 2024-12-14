#include "infra.h"

extern uint8_t RmtSta;
extern uint32_t RmtRec;
extern uint8_t RmtCnt;

uint8_t Remote_Scan()
{
    uint8_t sta = 0;
    uint8_t t1, t2;
    if (RmtSta & 0x40) // 得到一个按键的所有信息了
    {
        t1 = RmtRec >> 24;                           // 得到地址码
        t2 = (RmtRec >> 16) & 0xff;                  // 得到地址反码
        if ((t1 == (uint8_t)~t2) && t1 == REMOTE_ID) // 检验遥控识别码(ID)及地址
        {
            t1 = RmtRec >> 8;
            t2 = RmtRec;
            if (t1 == (uint8_t)~t2)
                sta = t1; // 键值正确
        }
        if ((sta == 0) || ((RmtSta & 0X80) == 0)) // 按键数据错误/遥控已经没有按下了
        {
            RmtSta &= ~0x40; // 清除接收到有效按键标识
            RmtCnt = 0;          // 清除按键次数计数器
        }
    }
    return sta;
}

uint8_t Remote_Count(){
    return (RmtSta & 0x40) ? RmtCnt : 0;
}