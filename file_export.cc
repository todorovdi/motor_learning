#include "file_export.h"

ofstream foutPerTrial;
ofstream foutVarDyn;
ofstream foutWeights;

void exportDynDataStd(unsigned int k, float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float R, float sr)
{
//    foutPerTrial<<k<<" ";
//    foutPerTrial<<"M1 = ";
//    for(int i=0;i<na;i++) foutPerTrial<<y[i]<<" ";                              
//    foutPerTrial<<endl;
//
//    foutPerTrial<<"D1= ";
//    for(int i=0;i<na;i++) foutPerTrial<<d1[i]  <<" ";
//    foutPerTrial<<endl;
//
//    foutPerTrial<<"D2= ";
//    for(int i=0;i<na;i++) foutPerTrial<<d2[i] <<" ";                              
//    foutPerTrial<<endl;
//
//    foutPerTrial<<"GPE= ";
//    for(int i=0;i<na;i++) foutPerTrial<<gpe[i]<<" ";
//    foutPerTrial<<endl;
//
//    foutPerTrial<<"GPI= ";
//    for(int i=0;i<na;i++) foutPerTrial<<gpi[i]<<" ";
//    foutPerTrial<<endl;
//
//    foutPerTrial<<"t exceptionality is "<< (t>=0.75)<<" reward is "<<R <<"  success rate is "<<sr<<endl;


    //foutVarDyn<<k<<" ";
    //for(int i=0;i<na;i++) 
    //{
    //    foutVarDyn<<y[i]<<" "<<d1[i]<<" "<<d2[i]<<" "<<gpe[i]<<" "<<gpi[i]<<" ";
    //}
    //foutVarDyn<<(t>=0.75)<<" "<<sr<<endl;


    foutVarDyn<<k<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<y[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<d1[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<d2[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<gpe[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<gpi[i]<<'\t';
    foutVarDyn<<endl;
}

void exportWeightsStd(unsigned int k, float ** w1,float ** w2,float **wm)
{
//    for(int i=0;i<na;i++) 
//    {
//        foutPerTrial<<"conn from cue to i D1 ";
//        for(int j=0;j<nc;j++)
//            foutPerTrial<<w1[j][i]<<" ";  
//        foutPerTrial<<endl;
//    }
//    for(int i=0;i<na;i++) 
//    {
//        foutPerTrial<<"conn from cue to i D2 ";
//        for(int j=0;j<nc;j++)
//            foutPerTrial<<w2[j][i]<<" ";  
//        foutPerTrial<<endl;
//    }
//    for(int i=0;i<na;i++) 
//    {
//        foutPerTrial<<"conn from cue to i M1 ";
//        for(int j=0;j<nc;j++)
//            foutPerTrial<<wm[j][i]<<" ";  
//        foutPerTrial<<endl;
//    }
//    foutPerTrial<<endl;

//    // for actions for 1 cue, actions for 2nd cue..
//    foutWeights<<k<<" ";
//    for(int i=0;i<nc;i++)
//    {
//        for(int j=0;j<na;j++)
//        {
//            foutWeights<<w1[i][j]<<" ";
//        }
//    }
//    for(int i=0;i<nc;i++)
//    {
//        for(int j=0;j<na;j++)
//        {
//            foutWeights<<w2[i][j]<<" ";
//        }
//    }
//    for(int i=0;i<nc;i++)
//    {
//        for(int j=0;j<na;j++)
//        {
//            foutWeights<<wm[i][j]<<" ";
//        }
//    }
//

    for(int i=0;i<na;i++) 
    {  
        for(int j=0;j<nc;j++) 
            foutWeights<<w1[j][i]<<'\t';
        foutWeights<<endl;
    }	
    foutWeights<<endl<<endl;	


    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeights<<w2[j][i]<<'\t';
        foutWeights<<endl;	
    }
    foutWeights<<endl<<endl;	
    
    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeights<<wm[j][i]<<'\t';
        foutWeights<<endl;	
    }
    foutWeights<<endl<<endl;	


//    foutWeights<<endl;
}

void exportCuesState(unsigned int k, float * x)
{
    foutPerTrial<<"trial number k="<<k<<" cues are ";
    for(int i=0;i<nc;i++)
        foutPerTrial<<x[i]<<"  ";
    foutPerTrial<<endl;
}

void trialEndExport(float * sumM1freq, unsigned int seed)
{
    foutPerTrial<<"sums are  ";
    for(int i=0;i<na;i++)
        foutPerTrial<<sumM1freq[i]<<"  ";
    foutPerTrial<<endl;
    foutPerTrial<<"seed was "<<seed<<endl;
}


/*
 *
 *  
std::fstream w1w2All;
	w1w2All.open("w1w2All", std::fstream::out |  std::ios::app);

               for(int i=0;i<na;i++) w1w2All<<w1[25][i]<<'\t';
		for(int i=0;i<na;i++) w1w2All<<w2[25][i]<<'\t';
					
                         w1w2All << endl;
	                  
	w1w2All.close();	
	
}
 *
 *
 *
	std::fstream w1w2;
	w1w2.open("w1w2", std::fstream::out | std::fstream::trunc);

	for(int j=0;j<nc;j++)
	{
		for(int i=0;i<na;i++)
		{
			w1w2 << w1[j][i] << '\t' << w2[j][i] << endl;
		}
         
	}
	w1w2.close();	
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

