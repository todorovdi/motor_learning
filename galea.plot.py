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

from matplotlib.backends.backend_pdf import PdfPages

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
    #tt = errs[:,-1]
    #print tt
    #print sum(tt)

    if nsess>1:
        for i in range(n):
            SEMs[i] = stats.sem(errs[:,i])
        
    #print max(SEMs)
    return means,SEMs

############################


###############################

def genFigureGalea(fnames,outname):
    fileToPlot = fnames[0]
    armData,dirShift,targetPre1 = armFileRead(fileToPlot)
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    nums = nums.astype(np.int)
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)

    ax = axs[0,0]
    genBGActivityPlot(fig,ax,fileToPlot.replace('arm','var_dyn2'))
    ax.set_xticks(pp.trials1[::10] )
    ax_ = ax.twiny()
    ax_.set_xlim(0, len(pp.trials1))
    ax_.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt])
    #ax_.set_xlabel("x-transformed")
    ax_.set_xticklabels(['ADAPT1','POST1'])
    ax_.xaxis.grid(True,color='w')

    ax = axs[0,1]
    genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'))
    ax.set_xticks(pp.trials1[::10])
    ax_ = ax.twiny()
    ax_.set_xlim(0, len(pp.trials1))
    ax_.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt])
    ax_.set_xticklabels(['ADAPT1','POST1'])
    ax_.xaxis.grid(True,color='w')

    genReachPlot(fig,axs[1,0],xs,ys,nums,twoPhases=True)

    ax = axs[1,1]

    errs,SEMs = doStats(fnames,n,False)
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
        ticks = [pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt, pp.numTrialsPre+pp.numTrialsAdapt+pp.numTrialsPost, pp.numTrialsPre*2+pp.numTrialsAdapt+pp.numTrialsPost ,pp.numTrialsPre*2 + pp.numTrialsAdapt*2+pp.numTrialsPost]
        ax_.set_xticks(ticks)
        ax_.set_xticklabels(['ADAPT1','POST1','PRE2','ADAPT2','POST2'])
    else:
        ticks = [pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt]
        ax_.set_xticks(ticks)
        ax_.set_xticklabels(['ADAPT1','POST1'])

    ax.set_xticks(ticks,minor=True)
    ax.xaxis.grid(True, which='minor')

    pos1 = axs[0,0].get_position()
    pos2 = axs[1,0].get_position()
    xLeft = pos1.x0
    yTop = pos1.y0+pos1.height
    posLeftMargin = [0,0,xLeft-0.00,1]             # from left, from down, width, height 
    printParams(fig,posLeftMargin)

    with PdfPages(pp.out_dir_pdf+outname+".pdf") as pdf:
        pdf.savefig()
        plt.close()
        genReachingByPhase(fileToPlot)
        pdf.savefig()
        plt.close()

    #plt.savefig(pp.out_dir_pdf+outname+".pdf")
    #plt.close(fig)

    #plt.clf()
    #plt.cla()
    #return armData,dirShift,targetPre1,errs

def genReachingByPhase(fname):
    fileToPlot = fnames[0]
    armData,dirShift,targetPre1 = armFileRead(fileToPlot)
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    nums = nums.astype(np.int)
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)

    subphase1 = pp.numTrialsPre
    subphase2 = subphase1 + pp.numTrialsAdapt
    subphase3 = subphase2 + pp.numTrialsPost
    subphase4 = subphase3 + pp.numTrialsPre
    subphase5 = subphase4 + pp.numTrialsAdapt
    subphase6 = subphase5 + pp.numTrialsPost

    rangePre1 = range(0,subphase1)
    rangeAdapt1 = range(subphase1,subphase2)
    rangePost1 = range(subphase2,subphase3)
    genReachPlot(fig,axs[0,0],xs[rangePre1],ys[rangePre1],nums[rangePre1],"Pre1")
    genReachPlot(fig,axs[0,1],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1")
    genReachPlot(fig,axs[1,0],xs[rangePost1],ys[rangePost1],nums[rangePost1],"Post1")

    if(pp.numPhases > 3):
        rangeAdapt2 = range(subphase4,subphase5)
        genReachPlot(fig,axs[1,1],xs[rangeAdapt2],ys[rangeAdapt2],nums[rangeAdapt2])

def genFigureSingleSess(fname,outname=""):
    armData,dirShift,targetPre1 = armFileRead(fname)
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, figsize=(20, 12), sharex=False, sharey=False)
    #fig.subplots_adjust(right=0.01,bottom=0.01,top=0.01,wspace=0.03)
    #fig.tight_layout(pad=0.1, w_pad=0.1, h_pad=1.0)

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
    ax_.set_xticks([pp.numTrialsPre, pp.numTrialsPre+pp.numTrialsAdapt, pp.numTrialsPre+pp.numTrialsAdapt+pp.numTrialsPost, pp.numTrialsPre*2+pp.numTrialsAdapt+pp.numTrialsPost, pp.numTrialsPre*2+pp.numTrialsAdapt*2+pp.numTrialsPost])
    ax_.set_xticklabels(['ADAPT1','POST1','PRE2','ADAPT2','POST2'])


    ax = axs[0]
    #ax = plt.gca()
    genReachPlot(ax,xs,ys,nums)

    if(outname==""):
        plt.savefig(fname+".pdf")
    else:
        plt.savefig(pp.out_dir_pdf+outname+".pdf")
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
        ends.append(errs[pp.numTrialsPre+pp.numTrialsAdapt-1] )

    fig=plt.figure()
    plt.plot(dirShifts,ends,'ro')
    plt.title("targetPre1 = "+ str(targetPre1))
    plt.savefig(pp.out_dir_pdf+"dirShift2error_target="+str(targetPre1)+".pdf")

    plt.clf()
    plt.cla()

def printParams(fig,pos):
    axlm= fig.add_axes(pos,frameon=False); 
    initX = 0.06;  initY = 0.5; ysubtr=0.02;
    fsz = 15

    i = 0
    axlm.text(initX,initY-ysubtr*i,"targetPre1 = "+ pp.paramsEnv["targetPre1"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"dirShift = "+ pp.paramsEnv["dirShift"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"learn_bg = "+ pp.paramsEnv["learn_bg"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"learn_cb = "+ pp.paramsEnv["learn_cb"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"cb_learn_rate = "+ pp.paramsEnv["cb_learn_rate"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"wmmax = "+ pp.paramsEnv["wmmax"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"fake_prelearn = "+ pp.paramsEnv["fake_prelearn"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"numTrialsPrelearn = "+ pp.paramsEnv["numTrialsPrelearn"],fontsize=fsz)                # count from left top
    i+=1
 
    axlm.text(initX,initY-ysubtr*i,"wmmax = "+ pp.paramsEnv["wmmax"],fontsize=fsz)                # count from left top
    i+=1

    axlm.text(initX,initY-ysubtr*i,"seed = "+ pp.paramsEnv["seed"],fontsize=fsz)                # count from left top
    i+=1


#paramsEnv = paramFileRead('galea.ini')
#
#rewardDist = float(paramsEnv["rewardDist") )
#dir = paramsEnv["output_dir")
#iniML = paramsEnv["iniML")
#iniBG = paramsEnv["iniBG")
#iniCB = paramsEnv["iniCB")
#iniArm = paramsEnv["iniArm")
#
#params = paramFileRead(iniML)
#
#armFileSkiprows = int(paramsEnv["armFileSkiprows"))
#
#targetPre1_def = float(paramsEnv["targetPre1"))
#targetPre2 = float(paramsEnv["targetPre2"))
#dirShift_def = float(paramsEnv["dirShift"))
#numPhases = int(paramsEnv["numPhases"))
#numTrialsPre = int(paramsEnv["numTrialsPre"))
#numTrialsAdapt = int(paramsEnv["numTrialsAdapt"))
#numTrialsPost = int(paramsEnv["numTrialsPost"))
#
#trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
#trials1 = range(trials1End)
#trials2 = range(trials1End,trials1End*2)

#from plotparams import paramsInit
#paramsInit('galea.ini')
#import plotparams
#print float(plotparams.paramsEnv["rewardDist") ) 
#exec(open('plotparams.py').read()) 
#

pp.paramsInit('galea.ini') 

fnames = []
for filename in os.listdir(pp.out_dir):
    if fnmatch.fnmatch(filename, '*_arm_*.dat'):
        fnames.append(pp.out_dir+filename)

pdfForEachSession = int(pp.paramsEnv["pdfForEachSession"])  #it is important to init it here and not later

filename = fnames[0]
import re
ree = '(.*)_\w+\.dat'
basename = os.path.basename(filename)
name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
pp.paramsInit(filename.replace("_arm","_modParams"),False)

if pdfForEachSession:
    print "Generate pictures for each seed"
    for filename in fnames:
        ree = '(.*)_\w+\.dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)
        genFigureGalea([filename],name+"_seed_"+pp.paramsEnv["seed"]);

genFigureGalea(fnames,'galea')



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


