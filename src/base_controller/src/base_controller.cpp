/******************************************************************
    基于串口通信的ROS小车基础控制器，功能如下：
    1.实现ros控制数据通过固定的格式和串口通信，从而达到控制小车的移动
    2.订阅了/cmd_vel主题，只要向该主题发布消息，就能实现对控制小车的移动
    3.发布里程计主题/odm

    串口通信说明：
    1.写入串口
    （1）内容：左右轮速度，单位为
    （2）格式：
    2.读取串口
    （1）内容：小车x,y坐标，方向，线速度，角速度，单位依次为：
    （2）格式：
*******************************************************************/
#include "ros/ros.h"  //ros需要的头文件
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
//以下为串口通讯需要的头文件
#include <string>        
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <math.h>
#include "serial/serial.h"
/****************************************************************************/
using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
/*****************************************************************************/
float ratio = 1000.0f ;   //转速转换比例，执行速度调整比例
float D = 0.525f ;    //两轮间距，单位是m
char const_num=2;
float pi=3.14159f;
float const_=180.0f;
float linear_temp=0,angular_temp=0;//转化后得到的线速度和角速度
/****************************************************/
unsigned char data_terminal0=0x0d;	//“/r"字符
unsigned char data_terminal1=0x0a;	//“/n"字符
unsigned char speed_data[14]={0};	//要发给串口的数据

string rec_buffer;	//串口数据接收变量

//发送给下位机的左右轮速度，里程计的坐标和方向
union floatData	//union的作用为实现char数组和float之间的转换
{
	float d;
	unsigned char data[4];
}right_speed_data,left_speed_data,position_x,position_y,oriention,vel_linear,vel_angular;
/************************************************************/
/*
rostopic pub -1 /cmd_vel geometry_msgs/Twist -- '[2.0, 0.0, 0.0]' '[0.0, 0.0, 1.8]' 
*/


void callback(const geometry_msgs::Twist & cmd_input)//订阅/cmd_vel主题回调函数
{
	string port("/dev/ttyUSB0");	//小车串口号
	unsigned long baud = 115200;	//小车串口波特率
	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));	//配置串口

    ROS_INFO("Received a /cmd_vel message!");  
    ROS_INFO("Linear Components:[%f,%f,%f]",cmd_input.linear.x,cmd_input.linear.y,cmd_input.linear.z);  
    ROS_INFO("Angular Components:[%f,%f,%f]",cmd_input.angular.x,cmd_input.angular.y,cmd_input.angular.z);

  		printf("Hello world USB0_A !\n"); 

	angular_temp = 0.5f*cmd_input.angular.z*D ;//获取/cmd_vel的角速度并转化为小车所规定的速度
	 ROS_INFO("angular_temp:[%f]",angular_temp);
	linear_temp =cmd_input.linear.x ;//获取/cmd_vel的线速度并转化为小车所规定的速度
		 ROS_INFO("linear_temp:[%f]",linear_temp);
	// linear_temp = ratio*limit_vel_speed ;
	//将转换好的小车速度分量为左右轮速度
	left_speed_data.d = linear_temp-angular_temp ;
		 ROS_INFO("left_speed_data.d :[%f]",left_speed_data.d );
	right_speed_data.d = linear_temp+angular_temp ;
		 ROS_INFO("right_speed_data.d:[%f]",right_speed_data.d);
	//存入数据到要发布的左右轮速度消息
	left_speed_data.d*=ratio;
	right_speed_data.d*=ratio;
    for(int i=0;i<4;i++)	//将左右轮速度存入数组中发送给串口
    {
        speed_data[i+4]=right_speed_data.data[i];
        speed_data[i+8]=left_speed_data.data[i];
    }
    		speed_data[0]=0xAA;
	speed_data[1]=0x55;
		speed_data[2]=0x09;
	speed_data[3]=0x01;
	

	//在写入串口的左右轮速度数据后加入”/r/n“
	speed_data[12]=0x66;;
	speed_data[13]=0x33;
	//写入数据到串口	
		my_serial.write(speed_data,14);

    ROS_INFO("Send a /cmd_vel message!");  
    ROS_INFO("right_speed_data:[%c,%c,%c,%c]",speed_data[8],speed_data[9],speed_data[10],speed_data[11]);  
    ROS_INFO("left_speed_data:[%c,%c,%c,%c]",speed_data[4],speed_data[5],speed_data[6],speed_data[7]);


	
}

int main(int argc, char **argv)
{
	string port("/dev/ttyUSB0");//小车串口号
	unsigned long baud = 115200;//小车串口波特率
	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));//配置串口

		printf("Hello world USB0_b !\n"); 

	ros::init(argc, argv, "base_controller");//初始化串口节点
	ros::NodeHandle n;	//定义节点进程句柄

	ros::Subscriber sub = n.subscribe("cmd_vel", 20, callback);	//订阅/cmd_vel主题
	ros::Publisher odom_pub= n.advertise<nav_msgs::Odometry>("odom", 20); //定义要发布/odom主题

	static tf::TransformBroadcaster odom_broadcaster;//定义tf对象
	geometry_msgs::TransformStamped odom_trans;//创建一个tf发布需要使用的TransformStamped类型消息
	nav_msgs::Odometry odom;//定义里程计对象
	geometry_msgs::Quaternion odom_quat; //四元数变量
	//定义covariance矩阵，作用为解决文职和速度的不同测量的不确定性
	float covariance[36] = {0.01,	0, 	  0, 	 0, 	0, 	   0,  // covariance on gps_x
							0,	0.01, 0, 	 0, 	0, 	   0,  // covariance on gps_y
							0, 	0, 	  99999, 0,		0, 	   0,  // covariance on gps_z
							0,	0, 	  0,  	 99999,	0, 	   0,  // large covariance on rot x
							0, 	0, 	  0, 	 0, 	99999, 0,  // large covariance on rot y
							0, 	0,    0, 	 0,     0,     0.01};  // large covariance on rot z 
	//载入covariance矩阵
	for(int i = 0; i < 36; i++)
	{
		odom.pose.covariance[i] = covariance[i];;
	}       

	ros::Rate loop_rate(10);//设置周期休眠时间
		
		printf("Hello world USB0_c !\n"); 		
	while(ros::ok())
	{
		rec_buffer =my_serial.readline(25,"\n");	//获取串口发送来的数据
		const char *receive_data=rec_buffer.data();	//保存串口发送来的数据
		if(rec_buffer.length()==21)	//串口接收的数据长度正确就处理并发布里程计数据消息
		{
			printf("Hello world USB0_d !\n"); 			
			for(int i=0;i<4;i++)//提取X，Y坐标，方向，线速度，角速度
			{
				position_x.data[i]=receive_data[i];
				position_y.data[i]=receive_data[i+4];
				oriention.data[i]=receive_data[i+8];
				vel_linear.data[i]=receive_data[i+12];
				vel_angular.data[i]=receive_data[i+16];
			}
			//将X，Y坐标，线速度缩小1000倍
			position_x.d/=1000;	
			position_y.d/=1000;
			vel_linear.d/=1000;
			
			//里程计的偏航角需要转换成四元数才能发布
            odom_quat = tf::createQuaternionMsgFromYaw(oriention.d);//将偏航角转换成四元数

			//载入坐标（tf）变换时间戳
			odom_trans.header.stamp = ros::Time::now();
			//发布坐标变换的父子坐标系
			odom_trans.header.frame_id = "odom";     
			odom_trans.child_frame_id = "base_footprint";       
			//tf位置数据：x,y,z,方向
			odom_trans.transform.translation.x = position_x.d;
			odom_trans.transform.translation.y = position_y.d;
			odom_trans.transform.translation.z = 0.0;
			odom_trans.transform.rotation = odom_quat;        
			//发布tf坐标变化
			odom_broadcaster.sendTransform(odom_trans);

			//载入里程计时间戳
			odom.header.stamp = ros::Time::now(); 
			//里程计的父子坐标系
			odom.header.frame_id = "odom";
			odom.child_frame_id = "base_footprint";       
			//里程计位置数据：x,y,z,方向
			odom.pose.pose.position.x = position_x.d;     
			odom.pose.pose.position.y = position_y.d;
			odom.pose.pose.position.z = 0.0;
			odom.pose.pose.orientation = odom_quat;       
			//载入线速度和角速度
			odom.twist.twist.linear.x = vel_linear.d;
			//odom.twist.twist.linear.y = odom_vy;
			odom.twist.twist.angular.z = vel_angular.d;    
			//发布里程计
			odom_pub.publish(odom);

			ros::spinOnce();//周期执行
            loop_rate.sleep();//周期休眠
		}
		//程序周期性调用
		//ros::spinOnce();  //callback函数必须处理所有问题时，才可以用到
	}
	return 0;
}