#include "BG_model.h"
#include "CB_model.h"

void CB_model::train(float x0, float y0, float * yy, float coef, bool flushW)
    // coef is the coef of the addition to the existing value
{
    float endpt[2];
//    if(yy == 0)
//        yy = last_y;

    x_cb_target = x0;     // TODO x_cb_target should debend on cues activated
    y_cb_target = y0;

    float wcb_backup[6][6];
    for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb_backup[k][l] = wcb[k][l];

	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++)
        {  
            // flush wcb
            for(int k=0;k<6;k++) 
                for(int l=0;l<6;l++) 
                    wcb[k][l]=0;

            // this is not needed because we add W*(activities ) to existing activities. So we actually do (Id + W)*(activities)
            //for(int l=0;l<6;l++) 
            //    wcb[l][l] = 1.;

            // set only current weight nonzero
            wcb[i][j]+=cb_init_shift_size;

            //x[0]=a; x[1]=b;
            
            // it influences update of DR which is basically some precalc * neuron activity
            arm->move(yy,endpt,wcb,0.);

            // compute error vector
            dfwx[i][j]= (1.-coef)*dfwx[i][j] + coef*(endpt[0]-x_cb_target)/cb_init_shift_size; 
            dfwy[i][j]= (1.-coef)*dfwy[i][j] + coef* (endpt[1]-y_cb_target)/cb_init_shift_size;
        }

    // flush cb weights so that they do not influence normal movements
    // we will set them to nonzero if we do cblearn
    if(flushW)
    { 
        for(int k=0;k<6;k++) 
            for(int l=0;l<6;l++) 
                wcb[k][l]=0;
    }
    else
    { 
        for(int k=0;k<6;k++) 
            for(int l=0;l<6;l++) 
                wcb[k][l] = wcb_backup[k][l];
    } 

    //learn_cb = true;
}

void CB_model::cblearn(float dx,float dy)
{
	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++) 
            wcb[i][j]-=cb_learn_rate*(dx*dfwx[i][j]+dy*dfwy[i][j]);
}

void CB_model::learn(float x,float y)
{
    cblearn(x-x_cb_target, y-y_cb_target);
}

void CB_model::flush()
{
    for(int i = 0; i<6;i++)
        for(int j=0;j<6;j++)
        {
            dfwx[i][j] = 0;
            dfwy[i][j] = 0;
        }
}

void CB_model::setArm(Arm * arm_)
{
    arm = arm_;
}

CB_model::CB_model(Arm * arm_)
{
    setArm(arm_);
    x_cb_target=0., y_cb_target=0.;
}

CB_model::CB_model()
{ 
    x_cb_target=0., y_cb_target=0.;
}

void CB_model::setCBtarget(float x, float y)
{
    x_cb_target=x, y_cb_target=y;
}

void CB_model::moveArm(float * y, float * out, float ffield)
{
    arm->move(y,out,wcb,ffield);
}

void CB_model::init(parmap & params,Exporter *exporter, Arm * arm_)
{
    //readIni(iniCBname,params);

    cb_learn_rate = stof(params["cb_learn_rate"]);
    cb_init_shift_size = stof(params["cb_init_shift_size"]);
    arm = arm_;
}
