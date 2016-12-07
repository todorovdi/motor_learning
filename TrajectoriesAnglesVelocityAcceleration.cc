
// This calculates Angles  angular velocities and accelarations

#include <math.h>
#include "par.h"

using namespace std;
void elbow(float* e,float x,float y)
{
	// % Calculate the position of the elbow (xe, ye) and the two angles

	float A=x*x+y*y;
	float Q=L1*L1-L2*L2+A;
	
	if(fabs(y)>fabs(x))
	{
		float B=-x*Q;
		float C=1./4*Q*Q-L1*L1*y*y;
		float D=B*B-4*A*C;
		if(y<0) e[0]=(-B-sqrt(D))/A/2; else e[0]=(-B+sqrt(D))/A/2;
		e[1]=(Q-2*x*e[0])/y/2;
	}
	else
	{
		float B=-y*Q;
		float C=1./4*Q*Q-L1*L1*x*x;
		float D=B*B-4*A*C;
		if(x>0) e[1]=(-B-sqrt(D))/A/2; else e[1]=(-B+sqrt(D))/A/2;
		e[0]=(Q-2*y*e[1])/x/2;
	}
}

void AnglesVelocityAcceleration(float  x1,float x2, float y1,float y2, float T,float n, float * t1,float * t2, float * t1v,float * t2v,float * t1a,float * t2a)
// t1= shoulder angle, t2=elbow angle, v- for velocity, a- for accelaration
{   float dt=T/n;
	for(int i=0;i<n;i++)
	{
		float t=i*dt;

// % Positions of the palm  calculated using given values
	float L=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)); // % distance that the palm moves
	float u[2]={(x2-x1)/L,(y2-y1)/L}; // %unit vector for motion direction 

// % Speed  of the palm, given

	float speed=(L/T)*(1-cos(2*M_PI*t/T));
 
// % Positions of the palm  calculated using given values
	
	float x=x1+u[0]*(L/T)*(t-(T/(2*M_PI))*sin(2*M_PI*t/T)); 
	float y=y1+u[1]*(L/T)*(t-(T/(2*M_PI))*sin(2*M_PI*t/T));
        
	
		float e[2];
		elbow(e,x,y);
       
		
		t1[i]=atan2(-e[0],e[1]);
		t2[i]=atan2(-x+e[0],y-e[1]);

   // to calculate velocities and accelarations 
   // d is for first derivative, and dd is for second derivative 
      float xd=u[0]*(L/T)*(1-cos(2*M_PI*t/T));  // x
       float xdd=u[0]*(L/T)*(2*M_PI/T)*(sin(2*M_PI*t/T));
       float yd=u[1]*(L/T)*(1-cos(2*M_PI*t/T));
       float ydd=u[1]*(L/T)*(2*M_PI/T)*(sin(2*M_PI*t/T));
       float  xed= e[1]/(x*e[1]-y*e[0])*((x-e[0])*xd+(y-e[1])*yd);
       float  yed=-e[0]*xed/e[1];
       float  xedd=(-y*e[0]*yed-e[1]*e[1]*xd+e[0]*e[1]*yd+y*e[1]*xed)/(pow((x*e[1]-y*e[0]),2))*((x-e[0])*xd+(y-e[1])*yd)+
                  e[1]/(x*e[1]-y*e[0])*((xd-xed)*xd+(x-e[0])*xdd+(yd-yed)*yd+(y-e[1])*ydd);
       float  yedd=-((xed*xed+e[0]*xedd)*e[1]-e[0]*xed*yed)/(e[1]*e[1]);
      if(fabs(e[1])>fabs(e[0]))
       { t1v[i]=-xed/e[1]; 
         t1a[i] =-(xedd*e[1]-xed*yed)/(e[1]*e[1]);
       }
       else
       {t1v[i]=yed/e[0];
        t1a[i] =(yedd*e[0]-xed*yed)/(e[0]*e[0]);
       }
      
       if(fabs(y-e[1])>fabs(x-e[0]))
       { t2v[i]=-(xd-xed)/(y-e[1]);
          t2a[i]=-((xdd-xedd)*(y-e[1])-(xd-xed)*(yd-yed))/((y-e[1])*(y-e[1]));
       }
       else
       { t2v[i]=(yd-yed)/(x-e[0]);
        t2a[i]=((ydd-yedd)*(x-e[0])-(yd-yed)*(xd-xed))/((x-e[0])*(x-e[0]));
       }
    }
}
