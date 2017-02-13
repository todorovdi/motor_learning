# percieved points here
#    unsigned int k = trialNum;
#    foutArm<<k<<'\t'       //1
#           <<xcur<<'\t'    //2
#           <<ycur<<'\t'    //3
#           <<x0  <<'\t'    //4   // most rewarded target coords
#           <<y0  <<'\t'    //5
#           <<d   <<'\t'    //6
#           <<xc  <<'\t'    //7
#           <<yc  <<'\t'    //8
#           <<endl;


#TODO: remove randmoness (if still there), 
# look at plain erros, without means
# compare for different values of dirShift where we are at the end of adapt epoch

import numpy as np
import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt

import fnmatch
import os
import ConfigParser
import io
import StringIO
import math

from matplotlib import cm
from scipy import stats

import plotparams as pp
from stdplots import *

#def paramFileRead(fname):
#    ini_str = '[root]\n' + open(fname, 'r').read()
#    ini_fp = StringIO.StringIO(ini_str)
#    params = ConfigParser.RawConfigParser(allow_no_value=True)
#    params.readfp(ini_fp)
#    return params


def getErrs(armData,xerr):
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    xs_target = armData[:,3]
    ys_target = armData[:,4]
    n = len(xs)

    errs = np.zeros(n)
    for (x,y,xt,yt,j) in zip(xs,ys,xs_target,ys_target,nums):
        if(xerr):
            d =  -(x-xt)
        else:
            d = math.sqrt( (x-xt)**2 + (y-yt)**2 )
        errs[int(j)] = d
    return errs

def doStats(fnames,n,xerr):
    nsess = len(fnames)
    errs = np.zeros(shape=(nsess,n))
    for i,fname in enumerate(fnames):
        armData = np.loadtxt(fnames[i],skiprows=2)
        errs[i,:] = getErrs(armData,xerr)
    #print math.isnan(t)
    means = np.zeros(n)
    SEMs = np.zeros(n)
    for i in range(n):
        means[i] = np.mean(errs[:,i])
        SEMs[i] = stats.sem(errs[:,i])
        
    #print max(SEMs)
    return means,SEMs

############################


###############################

def genFigureGalea(fnames,outname):
    file
    armData,dirShift,targetPre1 = armFileRead(fnames[0])
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(20, 20), sharex=False, sharey=False)

    ax = axs[0,0]
    genBGActivityPlot(ax,fnames[0].replace('arm','var_dyn2'))
    ax.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt])
    ax.set_xticklabels(['ADAPT1','POST1'])

    ax = axs[0,1]
    genBGWeightsPlot(ax,fnames[0].replace('arm','weights2'))
    ax.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt])
    ax.set_xticklabels(['ADAPT1','POST1'])

    genReachPlot(fig,axs[1,0],xs,ys,nums)

    ax = axs[1,1]

    errs,SEMs = doStats(fnames,n,True)
    #ax.errorbar(nums, means, yerr=SEMs)

    if(len(fnames) == 1):
        ax.plot(nums, errs)
    else:
        ax.errorbar(nums, errs, yerr=SEMs)

    ax.set_title('Errors averaged and SEMs',y=1.04)
    ax.set_xticks(np.arange(0,n,10))
    ax.set_yticks(np.arange(-0.9,0.9,0.1))
    ax.set_ylim([-0.5,0.5])
    ax.set_xlim([0,n])
    ax.grid()

    ax_ = ax.twiny()
    #ax_.set_xlabel("x-transformed")
    ax_.set_xlim(0, n)
    if pp.numPhases > 3:
        ax_.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt, pp.numTrialsPre+pp.numTrialsAdapt+pp.numTrialsPost, pp.numTrialsPre*2+pp.numTrialsAdapt+pp.numTrialsPost ,pp.numTrialsPre*2, pp.numTrialsAdapt*2+pp.numTrialsPost])
        ax_.set_xticklabels(['ADAPT1','POST1','PRE2','ADAPT2','POST2'])
    else:
        ax_.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt])
        ax_.set_xticklabels(['ADAPT1','POST1'])




    plt.savefig(pp.out_dir+outname+".pdf")
    plt.close(fig)
    #plt.clf()
    #plt.cla()
    #return armData,dirShift,targetPre1,errs

def genFigureSingleSess(fname,outname=""):
    armData,dirShift,targetPre1 = armFileRead(fname)
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, figsize=(20, 10), sharex=False, sharey=False)

    ax = axs[1]

    errs = getErrs(armData)
    #ax.errorbar(nums, means, yerr=SEMs)
    ax.plot(nums, errs)
    ax.set_title('Errors averaged and SEMs',y=1.04)
    ax.set_xticks(np.arange(0,n,10))
    ax.set_yticks(np.arange(-0.9,0.9,0.1))
    ax.set_ylim([-0.5,0.5])
    ax.set_xlim([0,n])
    ax.grid()

    ax_ = ax.twiny()
    #ax_.set_xlabel("x-transformed")
    ax_.set_xlim(0, n)
    ax_.set_xticks([numTrialsPre, numTrialsPre+numTrialsAdapt, numTrialsPre+numTrialsAdapt+numTrialsPost, numTrialsPre*2+numTrialsAdapt+numTrialsPost])
    ax_.set_xticklabels(['ADAPT1','POST1','PRE2','ADAPT2','POST2'])


    ax = axs[0]
    #ax = plt.gca()
    genReachPlot(ax,xs,ys,nums)

    if(outname==""):
        plt.savefig(fname+".pdf")
    else:
        plt.savefig(pp.out_dir+outname+".pdf")
    plt.close(fig)
    #plt.clf()
    #plt.cla()
    return armData,dirShift,targetPre1,errs

    #labels = ['point{0}'.format(i) for i in range(N)]

def genCritAnglePics(fnames):
    dirShifts=[]
    ends=[]
    for fname in fnames:
        armData,dirShift,targetPre1,errs = genFigureSingleSess(fname)
        #
        armData,dirShift,targetPre1 = armFileRead(fname)
        errs = getErrs(armData)
        dirShifts.append(int(dirShift))
        ends.append(errs[numTrialsPre+numTrialsAdapt-1] )

    #fig, axs = plt.subplots(ncols=2, figsize=(20, 10), sharex=False, sharey=False)

    fig=plt.figure()
    plt.plot(dirShifts,ends,'ro')
    plt.title("targetPre1 = "+ str(targetPre1))
    plt.savefig(pp.out_dir+"dirShift2error_target="+str(targetPre1)+".pdf")

    plt.clf()
    plt.cla()

pp.paramsInit('galea.ini')

#paramsEnv = paramFileRead('galea.ini')
#
#rewardDist = float(paramsEnv.get("root","rewardDist") )
#dir = paramsEnv.get("root","output_dir")
#iniML = paramsEnv.get("root","iniML")
#iniBG = paramsEnv.get("root","iniBG")
#iniCB = paramsEnv.get("root","iniCB")
#iniArm = paramsEnv.get("root","iniArm")
#
#paramsML = paramFileRead(iniML)
#
#armFileSkiprows = int(paramsEnv.get("root","armFileSkiprows"))
#
#targetPre1_def = float(paramsEnv.get("root","targetPre1"))
#targetPre2 = float(paramsEnv.get("root","targetPre2"))
#dirShift_def = float(paramsEnv.get("root","dirShift"))
#numPhases = int(paramsEnv.get("root","numPhases"))
#numTrialsPre = int(paramsEnv.get("root","numTrialsPre"))
#numTrialsAdapt = int(paramsEnv.get("root","numTrialsAdapt"))
#numTrialsPost = int(paramsEnv.get("root","numTrialsPost"))
#
#trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
#trials1 = range(trials1End)
#trials2 = range(trials1End,trials1End*2)

#from plotparams import paramsInit
#paramsInit('galea.ini')
#import plotparams
#print float(plotparams.paramsEnv.get("root","rewardDist") ) 
#exec(open('plotparams.py').read()) 
#
fnames = []
for filename in os.listdir(pp.out_dir):
    if fnmatch.fnmatch(filename, '*_arm*.dat'):
        fnames.append(pp.out_dir+filename)

#from stdplots import *
genFigureGalea(fnames,"galea")



#print fnames

#ax.plot(nums, means)




#xs = xs + addxs
#ys = ys + addys
#labels = labels + addlabel
#nums = nums + addnum

#xs = np.concatenate((xs,addxs),0)
#ys =  np.concatenate((ys,addys),0)
#nums = np.concatenate((nums,addnum),0)

#plt.plot(xs,ys,color="red",linestyle="",label="#1")


