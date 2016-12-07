#include <math.h>
#include <iostream>
#include <fstream>
#include "par.h"
#include <stdlib.h>
#include <string.h>
using namespace std;


float wcb[6][6]={},dfwx[6][6],dfwy[6][6];


//float lambda=10;
float lambda=6;

// sets wcb based on existing dfwy
void cblearn(float dx,float dy)
{
	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++) 
            wcb[i][j]-=lambda*(dx*dfwx[i][j]+dy*dfwy[i][j]);
}


const int na = 100;
//extern float DR_Al_Di[na][n][6];


int main(int argc,char** argv)
{
	float T=1;
	int freq=20;
    float xdot[4];
	int p=0;

	float x[4]={-M_PI/4,M_PI/4,0,0};
	float a=x[0],b=x[1];
//	for(int i=1;i<argc;i++)
//	{
//		if(strcmp(argv[i],"-t1")==0) x[0]=atof(argv[++i]);
//		else if(strcmp(argv[i],"-t2")==0) x[1]=atof(argv[++i]);
//		else if(strcmp(argv[i],"-T")==0) T=atof(argv[++i]);
//		else if(strcmp(argv[i],"-p")==0) p=atoi(argv[++i]);
//		else if(strcmp(argv[i],"-f")==0) freq=atoi(argv[++i]);
//		else if(strcmp(argv[i],"-i")==0) ifstream("ini")>>a>>b;
//		else { cerr<<"unknown option "<<argv[i]<<endl; return 1; }
//	}

	float y[na]={};
//	for(int i=0;i<na;i++) y[i]=.01;

    // activate down action
	y[75]=1;
	
	reach_init();

	float phi0[2]={ -0.832778,	1.16426};
	ifstream("ini")>>phi0[0]>>phi0[1];

	float dw=1;         // size of change in one weight, to compute derivatve wrt this change
	float x0=0,y0=.2;   // target point, not center

    // compute F derivative at zero
	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++)
        {  
            // flush wcb
            for(int k=0;k<6;k++) 
                for(int l=0;l<6;l++) 
                    wcb[k][l]=0;

            // set only current weight nonzero
            wcb[i][j]=dw;

            float phi[4]={};
            phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;
            phi[0] = a; phi[1] = b;   // -pi/4, pi/4
            //x[0]=a; x[1]=b;
            
            // it influences update of DR which is basically some precalc * neuron activity
            reach(phi,y,0, wcb);
            float xcur=(-L1*sin(phi[0])+-L2*sin(phi[1])),ycur=(L1*cos(phi[0])+L2*cos(phi[1]));
            //cout<<wcb[0][0]<<" "<<phi[0]<<" "<<phi[1]<<endl;

            // compute error vector
            dfwx[i][j]=(xcur-x0)/dw; 
            dfwy[i][j]=(ycur-y0)/dw;
        }
	
	x0=.12; y0=.2;    // set different target point

    // flush cb weights 
	for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb[k][l]=0;

    // teach to reach new point
    int trialNum = 10;
	for(int i=0;i<trialNum;i++)
	{
		x[0]=a; x[1]=b;
//		flagout=(i%10==0);

        float phi[4]={};
        phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;
        phi[0] = a; phi[1] = b; 

		reach(phi,y,0, wcb);
		float xcur=(-L1*sin(phi[0])+-L2*sin(phi[1])),ycur=(L1*cos(phi[0])+L2*cos(phi[1]));
		cblearn(xcur-x0,ycur-y0);  // show what we should have reached
            
        cout<<"cb learning errors "<<xcur-x0<<" "<<ycur-y0<<endl;
	}

    ofstream fout("output_cb/arm.dat");
	//flagout=1;
    
    // look where 4 actions reach now, after correction (?)
	for(int action=0;action<100;action+=25)
	{
		for(int k=0;k<na;k++) 
            y[k]=0;
		y[action]=1;

		//x[0]=a; x[1]=b;
        float phi[4]={};
        phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;
        phi[0] = a; phi[1] = b; 

		reach(phi,y,0, wcb);
		float xcur=(-L1*sin(phi[0])+-L2*sin(phi[1])),ycur=(L1*cos(phi[0])+L2*cos(phi[1]));
        fout<<xcur<<"\t"<<xcur<<endl;
	}
	return 0;
}

