// Mavlink_test2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "Serial.h"		// a reliant library, you can replace by yours
#include "mavlink.h" 	// point the correct path in /Mavlink/common
#include "iostream"
using namespace std;

unsigned char senddata[sizeof(mavlink_message_t)];	// send buffer 
CSerialPort mySerialPort;  // create a Serial port object


DWORD WINAPI receive(LPVOID lpParamter)	// this function will be called regularly 
{
	mavlink_heartbeat_t recv_heartbeat;	// some kinds of messages define here 
	mavlink_command_ack_t recv_setmode;
	mavlink_mission_ack_t recv_mission;
	mavlink_statustext_t recv_text;
	mavlink_status_t recvstatus;
	mavlink_message_t recvdata;
	mavlink_gps_raw_int_t gps;

	char a;
	while(1) 
	{
		if(mySerialPort.GetBytesInCOM()>0)
		{
			mySerialPort.ReadChar(a);		// read a char from Serial 
			if(mavlink_parse_char(MAVLINK_COMM_0,a,&recvdata,&recvstatus))		//use this parse function in Mavlink, it can parse massage automatically, we will use this one all the time
			{
				printf(" message id %d \n" ,recvdata.msgid);					// if parse complete and right, message is in recvdata, otherwise it cannot pass if()
				if(recvdata.msgid==0)											// according to the unique message_id, we divide them in different message
				{
					mavlink_msg_heartbeat_decode(&recvdata,&recv_heartbeat);	// using specific decode function, we can access the data in message.
					printf("%d\n",recv_heartbeat.system_status);
				}
				if(recvdata.msgid==77)
				{
					mavlink_msg_command_ack_decode(&recvdata,&recv_setmode);
					//printf("recv_setmode.result:%d\n",recv_setmode.result);
				}
				if(recvdata.msgid==47)
				{
					mavlink_msg_mission_ack_decode(&recvdata,&recv_mission);
					//printf("recv_mission.type:%d\n",recv_mission.type);
				}
				if(recvdata.msgid==253)
				{
					mavlink_msg_statustext_decode(&recvdata,&recv_text);
					//printf("recv_text.text:%s\n",recv_text.text);
				}

				if(recvdata.msgid==24)
				{
					mavlink_msg_gps_raw_int_decode(&recvdata,&gps);
					//printf("%s\n",gps.lat);
				}
			}
		}
		else 
		{
			Sleep(10);
			continue;
		}
	}
}

int main(void)
{

	if (!mySerialPort.InitPort(2))		// open the Serial port, LOOK OUT the port number
	{  
		std::cout << "initPort fail !" << std::endl;  
	}  
	else 
	{  
		std::cout << "initPort success !" << std::endl;  
	}  
	
	mavlink_message_t data;
	mavlink_msg_request_data_stream_pack(3,0,&data,1,1,2,1,0);				//a message for requesting the data stream
	memcpy(senddata,&data,sizeof(mavlink_message_t));						//copy to the send buffer
	if(mySerialPort.WriteData(senddata,sizeof(senddata)))					// send through Serial port
		cout<<"send ok"<<endl;
	CreateThread(NULL,0,receive,NULL,0,NULL);								// create a thread, tread function is receive() above 
	Sleep(1000);
	short i=0;
	while(1)
	{
		if(i==1)
		{	
			mavlink_msg_set_mode_pack(1,0,&data,1,1,4); // to change the mode in Guided, 4 represent Guided
			//0:Stabilize����,1:Acro�ؼ�,2:AltHold����,3:Auto�Զ�4:Guided����,5:Loiter���,6:RTL����,7:Circle��Ȧ9:Land����,11:DriftƮ��,13:Sport�˶�,14:Flip��ת15:AutoTune�Զ�����,16:PosHold����,17:Brake��ͣ
			memcpy(senddata,&data,sizeof(mavlink_message_t));
			if(mySerialPort.WriteData(senddata,sizeof(senddata)))
				cout<<"send set mode ok"<<endl;
		}
		if(i==3)
		{
			mavlink_msg_command_long_pack(3,0,&data,1,1,MAV_CMD_COMPONENT_ARM_DISARM,0,1,0,0,0,0,0,0);	// to arm the copter
			memcpy(senddata,&data,sizeof(mavlink_message_t));
			if(mySerialPort.WriteData(senddata,sizeof(senddata)))
				cout<<"send ARMED ok"<<endl;
		}

		if(i==6)
		{
			
			mavlink_msg_command_long_pack(3,0,&data,1,1,MAV_CMD_NAV_TAKEOFF,1,0,0,0,0,0,0,5); // to make copter take off at 5m 
			memcpy(senddata,&data,sizeof(mavlink_message_t));
			if(mySerialPort.WriteData(senddata,sizeof(senddata)))
				cout<<"send takeoff ok"<<endl;
		}

		if(i==10)
		{
			
			mavlink_msg_command_long_pack(3,0,&data,1,1,MAV_CMD_NAV_LAND,1,0,0,0,0,0,0,0);	// to make the copter land
			memcpy(senddata,&data,sizeof(mavlink_message_t));
			if(mySerialPort.WriteData(senddata,sizeof(senddata)))
				cout<<"send land ok"<<endl;
		}

		Sleep(1000);
		i++;
	}
}

