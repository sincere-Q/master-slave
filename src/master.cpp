/*
 * master.cpp
 *
 *  Created on: Aug 27, 2015
 *      Author: santosh
 */

//Master node should guide the master robot autonomously without bumping into
//objects and the slave should subscribe for master position
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"
#include <sstream>

#define PI 3.14

double lrange=0,crange=0,rrange=0;
double lintensity=0, cintensity=0, rintensity=0;

ros::Publisher velocity_publisher;

//Function declerations of move and rotate
void move(double speed, double distance, bool isForward);
void rotate (double angular_speed, double relative_angle, bool clockwise);

void laserCallBack(const sensor_msgs::LaserScan::ConstPtr & laser_msg);

//Function declearations for equilidian distance and degrees to radians conversion
double getDistance(double x1, double y1, double x2, double y2);
double degrees2radians(double angle_in_degrees);

//Wanader without bumping into obstacles 
void wander(void);

int main(int argc, char **argv)
{
 
  ros::init(argc, argv, "master");

 
  ros::NodeHandle masterNode;
  //ros::NodeHandle n;

 // ros::Publisher master_pub = masterNode.advertise<std_msgs::String>("myPose", 1000);
  velocity_publisher = masterNode.advertise<geometry_msgs::Twist>("/robot_0/cmd_vel", 1000);
    ros::Subscriber laser = masterNode.subscribe("/robot_0/base_scan", 1, laserCallBack);
  ros::Rate loop_rate(100000);


// just for testing 
  while (ros::ok())
  {
	//move (1.0, 1.0, 1);
	//rotate (1.0, 0.047, 1);
//loop_rate.sleep();
wander();
  }
 
  ros::spinOnce();

  return 0;
}

/**
 *  makes the robot turn with a certain linear velocity, for 
 *  a certain distance either forward or backward  
 */
void move(double speed, double distance, bool isForward){
   geometry_msgs::Twist vel_msg;
   //set a random linear velocity in the x-axis
   if (isForward)
	   vel_msg.linear.x =abs(speed);
   else
	   vel_msg.linear.x =-abs(speed);
   vel_msg.linear.y =0;
   vel_msg.linear.z =0;
   //set a random angular velocity in the y-axis
   vel_msg.angular.x = 0;
   vel_msg.angular.y = 0;
   vel_msg.angular.z =0;

   double t0 = ros::Time::now().toSec();
   double current_distance = 0.0;
   ros::Rate loop_rate(100);
   do{
	   velocity_publisher.publish(vel_msg);
	   double t1 = ros::Time::now().toSec();
	   current_distance = speed * (t1-t0);
	   ros::spinOnce();
	   loop_rate.sleep();
   }while(current_distance<distance);
   vel_msg.linear.x =0;
   velocity_publisher.publish(vel_msg);

}

/**
 *  makes the robot turn with a certain angular velocity, for 
 *  a certain distance in either clockwise or counter-clockwise direction  
 */
void rotate (double angular_speed, double relative_angle, bool clockwise){
//angular_speed = degrees2radians(angular_speed);
//relative_angle = degrees2radians(relative_angle);
	geometry_msgs::Twist vel_msg;
	   //set a random linear velocity in the x-axis
	   vel_msg.linear.x =0;
	   vel_msg.linear.y =0;
	   vel_msg.linear.z =0;
	   //set a random angular velocity in the y-axis
	   vel_msg.angular.x = 0;
	   vel_msg.angular.y = 0;
	   if (clockwise)
	   		   vel_msg.angular.z =-abs(angular_speed);
	   	   else
	   		   vel_msg.angular.z =abs(angular_speed);

	   double t0 = ros::Time::now().toSec();
	   double current_angle = 0.0;
	   ros::Rate loop_rate(1000);
	   do{
		   velocity_publisher.publish(vel_msg);
		   double t1 = ros::Time::now().toSec();
		   current_angle = angular_speed * (t1-t0);
		   ros::spinOnce();
		   loop_rate.sleep();
		   //cout<<(t1-t0)<<", "<<current_angle <<", "<<relative_angle<<endl;
	   }while(current_angle<relative_angle);
	   vel_msg.angular.z =0;
	   velocity_publisher.publish(vel_msg);
}

/**
 *  converts angles from degree to radians  
 */

double degrees2radians(double angle_in_degrees){
	return angle_in_degrees *PI /180.0;
}

/*
 * get the euclidian distance between two points 
 */
double getDistance(double x1, double y1, double x2, double y2){
	return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

/*
 * Call back implementation to read and process laser data  
 */
void laserCallBack(const sensor_msgs::LaserScan::ConstPtr & laser_msg)
{
ROS_INFO("I am in: [%s]", "laser call back");
lrange = laser_msg->ranges[2];
crange = laser_msg->ranges[1];
rrange = laser_msg->ranges[0];
ROS_INFO("Ranges: left->[%f], center->[%f], right[%f]", lrange, crange, rrange);
lintensity = laser_msg->intensities[2];
cintensity = laser_msg->intensities[1];
rintensity = laser_msg->intensities[0];
ROS_INFO("Intensities: left->[%f], center->[%f], right[%f]", lintensity, cintensity, rintensity);
}

/*
 *"Wander without hiting" anything implementation
 */
void wander(void)
{
ROS_INFO("I am [%s]", "wandering");
if (cintensity == 1)//obstacle in front
{
if (lrange >= rrange)
{
do
rotate (1.0, 0.5, 0);
while((cintensity !=0) && (rintensity !=0));
}
else if (lrange <= rrange)
{
do
rotate (1.0, 0.5, 1);
while ((cintensity !=0) && (lintensity !=0));
}
}
else
{
move (1.0, 1.0, 1);
}
}

