#include <math.h>
#include <iostream>
#include <fstream>
#include "par.h"
#include <stdlib.h>
#include <string.h>
using namespace std;
void DF_Inv(float* xdot,float t,float* x,float* DR)
{

   float l[N_Mus], v[N_Mus];
   float Hup[N_Mus], Hdw[N_Mus];

//    % joint specific angles and angular velocities 

	float theta[2]; theta[0]=x[0]; theta[1]=x[1]-x[0];
	float thetaV[2]; thetaV[0]=x[2]; thetaV[1]=x[3]-x[2];

       geometry_block(theta,thetaV,l,v,Hup,Hdw);
    
       float mn[N_Mus]={0,0,0,0,0,0}, f[N_Mus]={0,0,0,0,0,0};
      float fbIa[N_Mus]={0,0,0,0,0,0},fbIb[N_Mus]={0,0,0,0,0,0},fbII[N_Mus]={0,0,0,0,0,0};
 
        calc_feedback(l, v, mn, f, fbIa, fbIb,fbII);
        sp_cord(fbIa, fbIb,  DR,   mn);
        muscles(mn,l,v,f);
      
  

// calculting torques from forces

   float q1m=R[0]*f[0]+ R[1]*f[1]+ R[4]*f[4]+ R[5]*f[5];
   float q2m=R[2]*f[2]+R[3]*f[3]+ R[6]*f[4]+ R[7]*f[5];



//    % specify joint restrictions

	float a1min = M_PI/2-A1max;
	float a1max = M_PI/2-A1min;
	float a2min = M_PI-A2max;
	float a2max = M_PI-A2min;
  
//    %---- calculating total non-conservative forces 
//    % 1. joints viscosity

	float q1v = -BJ*thetaV[0]; 
    float q2v = -BJ*thetaV[1];

//    % 2. joints restrictions (elastic and viscosity components)

    float q1e = 0;
    float q2e = 0;
    if(theta[0] > a1max && thetaV[0] > 0)      // % if shoulder flexion exceeds the limit
        q1e = -KJr*(theta[0]-a1max)-BJr*thetaV[0];
    else if(theta[0] < a1min && thetaV[0] < 0) // % if shoulder extension exceeds the limit
        q1e = -KJr*(theta[0]-a1min)-BJr*thetaV[0];

    if(theta[1] > a2max && thetaV[1] > 0)      // % if elbow flexion exceeds the limit
        q2e = -KJr*(theta[1]-a2max)-BJr*thetaV[1];
    else if(theta[1] < a2min && thetaV[1] < 0) // % if elbow flexion exceeds the limit
        q2e = -KJr*(theta[1]-a2min)-BJr*thetaV[1];

 
    
//    %4. total non-conservative forces 

    float q1 = q1v-q2v+q1e-q2e+q1m-q2m; // %upper joint
    float q2 = q2v+q2e+q2m;             // %lower joint 


//    %---- calculating total conservative forces 

	float f1c = -M2*L1*D2*sin(x[0]-x[1])*x[3]*x[3];
	float f2c = M2*L1*D2*sin(x[0]-x[1])*x[2]*x[2];
	float f1g = M1*g*D1g*sin(x[0])+M2*g*L1g*sin(x[0]);
	float f2g = M2*g*D2g*sin(x[1]);

//    %---- calculating total torques in upper and lower joints;

	float Q1 = f1c-f1g+q1; 
	float Q2 = f2c-f2g+q2;

//    %--- calculating dynamics of the system

	float a1 = I1+M2*L1*L1;
	float a2 = I2+M2*D2*D2;
	float b = M2*L1*D2*cos(x[0]-x[1]);
	float D = a1*a2-b*b;
   
    xdot[0] = x[2]; // % angle in upper joint
    xdot[1] = x[3]; // % angle in lower joint
    xdot[2] = (a2*Q1-b*Q2)/D; // % angular velocity in upper joint
    xdot[3] = (a1*Q2-b*Q1)/D; // % angular velocity in lower joint
}

using namespace std;

const int na=100,n=1000;

float DR_Al_Di[na][n][6]={};
	
void reach_init()
{
    ifstream AllD_CorticalData("AllD_CorticalData");

	for (int d=0;d<na;d++)
	{
		for(int i=0;i<n;i++)
		{
			for(int j=0;j<6;j++) 
			{ 
				
                              AllD_CorticalData >> DR_Al_Di[d][i][j]; 
			}
		}
	}	
}


float wcb[6][6]={},dfwx[6][6],dfwy[6][6];

void cbnet(float *DR)
{
	float cb[6]={};
	for(int i=0;i<6;i++) for(int j=0;j<6;j++) cb[i]+=wcb[i][j]*DR[j];
	for(int i=0;i<6;i++) DR[i]+=cb[i];
}

float lambda=10;

void cblearn(float dx,float dy)
{
	for(int i=0;i<6;i++) for(int j=0;j<6;j++) wcb[i][j]-=lambda*(dx*dfwx[i][j]+dy*dfwy[i][j]);
}

int flagout=0;
void reach(float* x,float* y,float dt)
{
	float xdot[4];
    for(int i=0;i<n;i++)
	{
		float DR[6]={};
		for(int k=0;k<6;k++)
		{
			for(int j=0;j<na;j++) DR[k]+=y[j]*(DR_Al_Di[j][i][k]-DR_Al_Di[0][0][k]);
		}
		cbnet(DR);
		for(int k=0;k<6;k++) DR[k]+=DR_Al_Di[0][0][k];

		float t=i*dt;

		DF_Inv(xdot,t,x,DR);

		for(int j=0;j<4;j++) x[j]+=xdot[j]*dt;	

		int freq=20;
		if(flagout && i%freq==0)
		{
//			cout<<"set arrow 1 from 0,0 to "<<(-L1*sin(x[0]))<<","<<(L1*cos(x[0]))<<" nohead"<<endl;
//			cout<<"set arrow 2 from "<<(-L1*sin(x[0])+-L2*sin(x[1]))<<","<<(L1*cos(x[0])+L2*cos(x[1]))<<" to "<<
//				(-L1*sin(x[0]))<<","<<(L1*cos(x[0]))<<" nohead"<<endl;
//			cout<<"set object 1 circle at "<<(-L1*sin(x[0])+-L2*sin(x[1]))<<","<<(L1*cos(x[0])+L2*cos(x[1]))<<
//				" size .02"<<endl;
//			cout<<"plot 0"<<endl<<"pause "<<(dt*freq)<<endl;
	 	}
	}
}


int main(int argc,char** argv)
{
	float T=1;
	int freq=20;
	float x[4]={-M_PI/4,M_PI/4,0,0},xdot[4];
	int p=0;
	float a=x[0],b=x[1];
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-t1")==0) x[0]=atof(argv[++i]);
		else if(strcmp(argv[i],"-t2")==0) x[1]=atof(argv[++i]);
		else if(strcmp(argv[i],"-T")==0) T=atof(argv[++i]);
		else if(strcmp(argv[i],"-p")==0) p=atoi(argv[++i]);
		else if(strcmp(argv[i],"-f")==0) freq=atoi(argv[++i]);
		else if(strcmp(argv[i],"-i")==0) ifstream("ini")>>a>>b;
		else { cerr<<"unknown option "<<argv[i]<<endl; return 1; }
	}

	float dt=T/n;
	
	float y[na]={};
//	for(int i=0;i<na;i++) y[i]=.01;
	y[75]=1;
	
	reach_init();

	float dw=1;
	float x0=0,y0=.2;
	for(int i=0;i<6;i++) for(int j=0;j<6;j++)
	{
		for(int k=0;k<6;k++) for(int l=0;l<6;l++) wcb[k][l]=0;
		wcb[i][j]=dw;
		x[0]=a; x[1]=b;
		reach(x,y,dt);
		float xcur=(-L1*sin(x[0])+-L2*sin(x[1])),ycur=(L1*cos(x[0])+L2*cos(x[1]));
        cout<<x[0]<<" "<<x[1]<<endl;
		dfwx[i][j]=(xcur-x0)/dw; 
		dfwy[i][j]=(ycur-y0)/dw;
	}
	
	x0=.05; y0=.2;
	for(int k=0;k<6;k++) for(int l=0;l<6;l++) wcb[k][l]=0;
	for(int i=0;i<10;i++)
	{
		x[0]=a; x[1]=b;
//		flagout=(i%10==0);
		reach(x,y,dt);
		float xcur=(-L1*sin(x[0])+-L2*sin(x[1])),ycur=(L1*cos(x[0])+L2*cos(x[1]));
		cblearn(xcur-x0,ycur-y0);
	}

	flagout=1;
	for(int i=0;i<100;i+=25)
	{
		for(int k=0;k<na;k++) y[k]=0;
		y[i]=1;
		x[0]=a; x[1]=b;
		reach(x,y,dt);
	}
	return 0;
}

