/****************************************************************************
filename:           SendRTP.cpp
Author:             linshufei
Date:               2017/9/13
Discription:

*****************************************************************************/
#include "SendRTP.h"
#include <iostream>

using namespace jrtplib;

#define MAXLEN    (RTP_DEFAULTPACKETSIZE - 100)

SendRTP::SendRTP(void){}
SendRTP::~SendRTP(void){}

void SendRTP::CheckError(int rtpErr){
    if (rtpErr < 0){
        std::cout << "ERROR: " << RTPGetErrorString(rtpErr) << std::endl;
        exit(-1);
    }
}

void SendRTP::SendH264Nalu(uint8_t *h264Buf, int bufLen){
    int status;
    uint8_t *pSendBuf;

    pSendBuf = h264Buf;
    if (bufLen <= MAXLEN)   //dataSize 小于最大长度
    {
        sess.SetDefaultMark(true);
        status = sess.SendPacket((void *)&pSendBuf[0], bufLen);
        CheckError(status);
        printf("send_packt 0 len = %d\n", bufLen);
    }
    else if (bufLen > MAXLEN)   //dataSize 大于最大长度
    {
        int sendPacket = 0;
        int allPacket = 0;
        int overData = 0;
        int iSendLen;

        allPacket = bufLen / MAXLEN;
        overData = bufLen % MAXLEN;
        sendPacket = 0;
        while ((sendPacket < allPacket) || ((sendPacket == allPacket) && (overData > 0)))
        {
            printf("send_packet = %d \n", sendPacket);

            // the first packet or the second last packet 
            if ((0 == sendPacket) || (sendPacket < allPacket && 0 != sendPacket))
            {  
                sess.SetDefaultMark(false);
                status = sess.SendPacket((void *)&pSendBuf[sendPacket * MAXLEN], MAXLEN);
                CheckError(status);
                sendPacket++;
            }

            // the last packet
            else if (((allPacket == sendPacket) && overData>0) || ((sendPacket == (allPacket - 1)) && overData == 0))
            {
                sess.SetDefaultMark(true);

                if (overData > 0)
                {
                    iSendLen = bufLen - sendPacket * MAXLEN;
                }
                else
                {
                    iSendLen = MAXLEN;
                }

                status = sess.SendPacket((void *)&pSendBuf[sendPacket * MAXLEN], iSendLen);
                CheckError(status);
                sendPacket++;
            }
        }
    }
}

void SendRTP::Init(void)
{
#ifdef WIN32  
    WSADATA dat;
    WSAStartup(MAKEWORD(2, 2), &dat);
#endif // WIN32 

    int status;

    //RTPSession sess;
    uint16_t portBase = 6666;  //local portbase
    uint16_t destPort = 8004;
    uint8_t destIP[] = { 192, 168, 179, 129 };  //目标IP地址

    RTPUDPv4TransmissionParams transParams;     //传输参数
    RTPSessionParams sessParams;                //会话参数

    // set h264 param
    sessParams.SetUsePredefinedSSRC(true);              //设置使用预先定义的SSRC
    sessParams.SetOwnTimestampUnit(1.0 / 90000.0);      //设置采样间隔，1.0/9000.0表示一秒采样9000个samples，一定要设置，否则RTCP Sender 会计算出错
    sessParams.SetAcceptOwnPackets(true);               //接收自己发送的数据包

    transParams.SetPortbase(portBase);
    status = sess.Create(sessParams, &transParams);
    CheckError(status);

    RTPIPv4Address addr(destIP, destPort);
    status = sess.AddDestination(addr);
    CheckError(status);

    sess.SetDefaultTimestampIncrement(3600);    //设置时间戳增加间隔
    sess.SetDefaultPayloadType(96);             //96 for h264, 26 for jpeg
    sess.SetDefaultMark(true);                  //重要事件标志
}

void SendRTP::DeInit()
{
    //void BYEDestroy(const RTPTime &maxwaittime, const void *reason,size t reasonlength)
    //发送一个BYE包并且离开绘画。在发送BYE包之前等待maxwaittime(jrtplib::RTPTime(10, 0))，如果超时，会不发送BYE包直接离开，BYE包会包含离开原因reason，reasonlength表示reason长度
    sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0); 
}
