#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "par.h"
#include "suppl.h"

int genCortcalData(parmap & params) //int argc,char** argv)
{
  //float x1=.4, y1=-.6;
	float x2=-.5, y2=-.2;
	
	float T=1;
	int n=1000;
  int N =100;
	
	float  TqRatio=0.7;    // %  parameter to control  the ratio for bifurnction and shoulder.
  float  BaseL_Activation=0.1; // % parameter to control the Tonic or base line activity.

  float minAngDeg = 0;        // count from EAST ccw
  float maxAngDeg = 360;

  string key;
  parmap::iterator iter;

  key = string("minActionAngDeg");
  iter = params.find(key);
  if(iter!=params.end())
  {
    minAngDeg = stof(iter->second);   
  }
  else
  {
    cout<<"key "<<key<<" not found"<<endl;
  }

  key = string("maxActionAngDeg");
  iter = params.find(key);
  if(iter!=params.end())
  {
    maxAngDeg = stof(iter->second);   
  }
  else
  {
    cout<<"key "<<key<<" not found"<<endl;
  }

  key = string("na");
  iter = params.find(key);
  if(iter!=params.end())
  {
    N = stoi(iter->second);   
  }
  else
  {
    cout<<"key "<<key<<" not found"<<endl;
  }

  float x1=0, y1=0.4;

//# Target directions
//  td1=25
//  td2=75
//
//  x2=x1+0.2*cos(2*M_PI*td1/N);
//  y2=y1+0.2*sin(2*M_PI*td1/N);

  //set print 'tmp'
  //print "-x1 ",x1," -x2 ",x2," -y1 ",y1," -y2 ",y2
  //set print

	//for(int i=1;i<argc;i++)
	//{
	//	if(strcmp(argv[i],"-x1")==0) x1=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-x2")==0) x2=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-y1")==0) y1=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-y2")==0) y2=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-T")==0) T=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-r")==0) TqRatio=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-b")==0) BaseL_Activation=atof(argv[++i]);
	//	else if(strcmp(argv[i],"-n")==0) n=atoi(argv[++i]);
  //  else if(strcmp(argv[i],"-N")==0) N=atoi(argv[++i]);
  //  else if(strcmp(argv[i],"-minAngDeg")==0) minAngDeg=atof(argv[++i]);
  //  else if(strcmp(argv[i],"-maxAngDeg")==0) maxAngDeg=atof(argv[++i]);
	//	else { cerr<<"unknown option "<<argv[i]<<endl; return 1; }
	//}
	
      // float Di_DR[n][N_Mus]={0};
        //float  DR_Al_Di[N][n][6];
      //  float  DR_Al_Di[n][6];

   //ofstream CorticalData("CorticalData");
   ofstream AllD_CorticalData("AllD_CorticalData");
  

  float minAngRad = minAngDeg / 180 * M_PI;
  float maxAngRad = maxAngDeg / 180 * M_PI;

  string fname="ini";
  key = "armInitAng_fname";
  iter = params.find(key);
  if(iter != params.end())
  { 
    fname = iter->second;
  }

  cout<<"------- recalibrating arm -- minAngDeg="<<minAngDeg<<" maxAngDeg="<<maxAngDeg<<" na="<<N<<endl;

 for (int TD=0;TD<N;TD++)
 {
         
   cout<<" recalibrating action number "<<TD<<endl;
   //  cout << TD << endl;
    x2=x1+0.2*cos(minAngRad + maxAngRad*float(TD)/float(N));
    y2=y1+0.2*sin(minAngRad + maxAngRad*float(TD)/float(N));

    //ofstream CorticalData("CorticalData");

   float t1[n]={0}, t2[n]={0}; // t1= shoulder angle, t2=elbow angle, v- for velocity, a- for accelaration
	float t1v[n]={0}, t2v[n]={0};
	float t1a[n]={0}, t2a[n]={0};
// to calculate Angles, velocities and accelarations 
 AnglesVelocityAcceleration(  x1, x2,  y1, y2,  T, n,  t1,t2,  t1v, t2v, t1a, t2a);
	 ofstream(fname)<<t1[0]<<'\t'<<t2[0]<<endl;

   params["phi_0"] = to_string(t1[0]);
   params["phi_1"] = to_string(t2[0]);

 
	for(int i=0;i<n;i++)
	{
    //cout<<"start time "<<i<<endl;
		
	//    % joint specific angles and angular velocities 
		float theta[2]; theta[0]=t1[i]; theta[1]=t2[i]-t1[i];
		float thetaV[2]; thetaV[0]=t1v[i]; thetaV[1]=t2v[i]-t1v[i];

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

//    %--- calculating dynamics of the system

		float a1 = I1+M2*L1*L1;
		float a2 = I2+M2*D2*D2;
		float b = M2*L1*D2*cos(t1[i]-t2[i]);
		float D = a1*a2-b*b;
   
//    %---- calculating total conservative forces 

		float f1c = -M2*L1*D2*sin(t1[i]-t2[i])*t2v[i]*t2v[i];
		float f2c = M2*L1*D2*sin(t1[i]-t2[i])*t1v[i]*t1v[i];
		float f1g = M1*g*D1g*sin(t1[i])+M2*g*L1g*sin(t1[i]);
		float f2g = M2*g*D2g*sin(t2[i]);

//    %4. total non-conservative forces 

// % Solving for q1 and q2, total non-conservative forces 
 
		float q1=a1*t1a[i] + b*t2a[i] -(f1c-f1g);   // % solving q1
		float q2=a2*t2a[i] + b*t1a[i] -(f2c-f2g);   // % solving q2


//   % Solving for muscle torques:  q1m, q2m 
         float q1m,q2m;
    	 q2m=q2-(q2v+q2e);             // %lower joint  
    	 q1m=q1-(q1v-q2v+q1e-q2e-q2m); // %upper joint


// Solving muscle length and velocity
    	
		float l[N_Mus], v[N_Mus];
		float Hup[N_Mus], Hdw[N_Mus];
	
        geometry_block(theta,thetaV,l,v,Hup,Hdw);

// Calculating Forces and motoneurons signals

             float f[N_Mus]={0,0,0,0,0,0};
		float mn[N_Mus];

        Force_MN(l, v, q1m, q2m,TqRatio,BaseL_Activation,  f, mn);
 
 // Calculating Feedbacks, fbIa, fbIb, fbII


      float fbIa[N_Mus],fbIb[N_Mus],fbII[N_Mus];
      calc_feedback(l, v, mn, f, fbIa, fbIb,fbII);

// ========== Calculating Drives=================================
 
   //  Calculating Weighted input
	float WDRMN [6][6], WDRIa [6][6], WDRIb [6][6], WMNRC [6][6];
        float WRCIa [6][6], WRCMN [6][6], WRCRC [6][6];
	float WIaIa [6][6], WIaMN [6][6], WIbMN [6][6];
	float WfbIaMN [6][6], WfbIaIa [6][6], WfbIbIb [6][6],WMNMN [6][6];
	

	 ParWeights( WDRMN,WDRIa,  WDRIb , WMNRC, WRCIa, WRCMN ,  WRCRC ,
		    WIaIa ,  WIaMN ,  WIbMN , WfbIaMN,  WfbIaIa, WfbIbIb ,  WMNMN);


       float DR [N_Mus],RC [N_Mus], Ia [N_Mus], Ib [N_Mus];
        float Y_MN [N_Mus], Y1 [N_Mus],  Y2 [N_Mus], Y_RC [N_Mus], Y_Ia [N_Mus], Y_Ib [N_Mus];
	float DR1[N_Mus], DR2[N_Mus];
        float Ymn[N_Mus]= {0};
        
       
     

	for(int j=0;j<N_Mus;j++)
	{
           if (i==0){DR[j] = (V12+Slope*log(mn[j]/(1-mn[j]))-B[j])/(Z[j]*WDRMN[j][j]);} //  Trivial solution for DR
            DR2[j]=DR[j];  DR1[j]= 0.1 + DR[j]; 
            Ymn[j]=0.0001;//0.001*rand()/(float)RAND_MAX;  //  randomize the outputs of the neurons (try to avoid bistability) 
            
        } 
             
	  sp_cord(  fbIa, fbIb,  DR1,   Ymn);


	 for(int j=0;j<N_Mus;j++){Y1[j]=Ymn[j]-mn[j];}

	 	
         float resSum=0;
  
	        	      
	do {  sp_cord(  fbIa, fbIb,  DR,   Ymn);
		     
		float res=0;
		for(int j=0;j<N_Mus;j++){res+= (fabs(Ymn[j]-mn[j])) >10.2*EpsY? 1:0;}
	
		resSum=res;

     for(int j=0;j<N_Mus;j++)

     {																    
		  Y2[j]=Ymn[j]-mn[j];

      float step=0, signstep=0,  chooseMin=0,ss=0;
                  // to update drives
      if (fabs(Y2[j]-Y1[j])>0)
      {
        step= (Y2[j]*(DR2[j]-DR1[j]))/(Y2[j]-Y1[j]);
        signstep = (step > 0) ? 1 : ((step < 0) ? -1 : 0);
        chooseMin= (fabs(step)<= 0.001)? fabs(step) : 0.001;
        ss=signstep*chooseMin;
        DR[j]=DR2[j]-ss;
      }

      DR1[j]=DR2[j];
      DR2[j]=DR[j];
      Y1[j]=Y2[j];     

     }
    
   } while(resSum>0);
 		 
      

      //for(int j=0;j<N_Mus;j++){CorticalData<<DR[j]<<'\t';}CorticalData<<endl;

     
  for(int j=0;j<N_Mus;j++)
  {
    AllD_CorticalData<<DR[j]<<'\t';
  }
  AllD_CorticalData<<endl;
	
		
         
  } // end of for loop i 




} // end of dir loop TD


 
  return 0;

  cout<<"------- recalibrating arm finished "<<endl;

}  // end of main

