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
import sys

from matplotlib import cm
from scipy import stats

import plotparams as pp
from stdplots import *

from matplotlib.backends.backend_pdf import PdfPages
import re

#def paramFileRead(fname):
#    ini_str = '[root]\n' + open(fname, 'r').read()
#    ini_fp = StringIO.StringIO(ini_str)
#    params = ConfigParser.RawConfigParser(allow_no_value=True)
#    params.readfp(ini_fp)
#    return params

def getReachAngles(armData,xerr=False):
    nums = armData[:,0]
    #xs = armData[:,1]
    #ys = armData[:,2]
    x_actual = armData[:,8]
    y_actual = armData[:,9]
    #xs_target = armData[:,3]
    #ys_target = armData[:,4]
    n = len(x_actual)

    angs = np.zeros(n)
    for (x,y,j) in zip(x_actual,y_actual,nums):
        xc = 0
        yc = 0.4
        angleDeg = ( math.atan( (y-yc) / (x-xc) )  ) / (2*math.pi) * 360.
    #print int(j),x,y,xt,yt, d
        angs[int(j)] = angleDeg
    return angs

def getErrs(armData,xerr=False):
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    xs_target = armData[:,3]
    ys_target = armData[:,4]
    n = len(xs)

    errs = np.zeros(n)
    for (x,y,xt,yt,j) in zip(xs,ys,xs_target,ys_target,nums):
        if(int(pp.paramsEnv["sector_reward"]) == 0):
            if(xerr):
                d =  -(x-xt)
            else:
                d = math.sqrt( (x-xt)**2 + (y-yt)**2 )
        else:
            xc = 0
            yc = 0.4
            xd = x-xc;
            yd = y-xc;
            tgtAngleDeg = ( math.atan( (xt-xc) / (yt-yc) )  ) / (2*math.pi) * 360.
            angleCur0 = math.atan(yd/xd) / (2*math.pi) * 360.
            if(xd<0):
                s = 0
                if(yd>0):
                    s = 90
                else:
                    s = -90
                angleCur0 = math.atan(-xd/yd)/ (2*math.pi) * 360  + s 
            angleCur = angleCur0
            if(angleCur0 < 0):
                angleCur = angleCur0 + 360
            dif = angleCur - tgtAngleDeg;
            dif1 = dif
            if(dif > 180.):
                dif1 = dif-360.
            d = dif1
        #print int(j),x,y,xt,yt, d
        errs[int(j)] = d
    return errs

def doStats(fnames,n,xerr):
    nsess = len(fnames)
    errs = np.zeros(shape=(nsess,n))
    for i,fname in enumerate(fnames):
        armData = np.loadtxt(fnames[i])
        errs[i,:] = getReachAngles(armData,xerr)
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

def annotateGraph(ax):
    n = pp.phaseEnds[-1]
    ax.set_xticks(range(n)[::xtickSkip] )
    ax_ = ax.twiny()
    #ax_.set_xlim([0, n])
    ax_.set_xticks(pp.phaseEnds[:-1])
    ax_.set_xticklabels(pp.phaseNames)
    ax_.xaxis.grid(True,color='w')

###############################

def genFigurePert(fnames,outname):
    fileToPlot = fnames[0]
    armData = armFileRead(fileToPlot)
    #armData = np.loadtxt(fnames[0])
    nums = armData[:,0]
    nums = nums.astype(np.int)
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)

    ax = axs[0,0]
    genBGActivityPlot(fig,ax,fileToPlot.replace('arm','var_dyn2'))
    annotateGraph(ax)

    #lines = ax_.get_lines()
    #lines = ax.get_legend().get_lines()
    #for line in lines:
    #    line.set_dashes([20,170])

    ax = axs[0,1]
    genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'))
    annotateGraph(ax)

    #genReachPlot(fig,axs[1,0],xs,ys,nums,twoPhases=True)

    if(int(pp.paramsEnv["nc"]) > 1):
        ax = axs[1,0]
        genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'),1)
        annotateGraph(ax)

    ax = axs[1,1]

    angs,SEMs = doStats(fnames,n,False)

    if(len(fnames) == 1):
        ax.plot(nums, angs)
    else:
        ax.errorbar(nums, angs, yerr=SEMs)
    annotateGraph(ax)
    ax.yaxis.grid(True)

    ax.set_title('Endpoint angles averaged and SEMs',y=1.04)
    ymax = 0.8
    ymin = 0.
    if "y_axis_max" in pp.paramsEnv:
        ymax = float(pp.paramsEnv["y_axis_max"] )
    if "y_axis_signed" in pp.paramsEnv:
        if(int(pp.paramsEnv["y_axis_signed"]) ): 
            ymin = -ymax
    step = (ymax-ymin) / 10.
    if "y_axis_step" in pp.paramsEnv:
        step = (int(pp.paramsEnv["y_axis_step"]) ) 
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,step))

    ax.set_xticks(pp.phaseEnds[:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

    pos1 = axs[0,0].get_position()
    pos2 = axs[1,0].get_position()
    xLeft = pos1.x0
    yTop = pos1.y0+pos1.height
    posLeftMargin = [0,0,xLeft-0.00,1]             # from left, from down, width, height 
    printParams(fig,posLeftMargin)

    if(len(fnames) == 1):
        pdfname = pp.out_dir_pdf_single+outname+".pdf"
    else:
        pdfname = pp.out_dir_pdf+outname+"_stats_n="+str(len(fnames))+".pdf"

    with PdfPages(pdfname) as pdf:
        pdf.savefig()
        plt.close()
        genReachingByPhase(fileToPlot)
        pdf.savefig()
        plt.close()

        fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        ax = axs[0,1]
        if(int(pp.paramsEnv["nc"]) > 1):
            genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'),1)
            annotateGraph(ax)

        fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        ax = axs[0,0]
        genCBStatePlot(fig,ax,fileToPlot.replace('arm','CBState'))
        annotateGraph(ax)

        ax = axs[0,1]
        genCBTuningPlot(fig,ax,fileToPlot.replace('arm','CBTuning'))
        annotateGraph(ax)

        ax = axs[1,0]
        genCBMiscPlot(fig,ax,fileToPlot.replace('arm','CBMisc'))
        annotateGraph(ax)

        ax.set_xticks(pp.phaseEnds[:-1],minor=True)
        ax.xaxis.grid(True, which='minor')

        #x_target = armData[:,3]
        #y_target = armData[:,4]
        x_cbtgt = armData[:,10]
        y_cbtgt = armData[:,11]

        rangePre1 = range(0,pp.phaseEnds[0])
        rangeAdapt1 = range(pp.phaseEnds[0],pp.phaseEnds[-2])
        rangePost1 = range(pp.phaseEnds[-2],pp.phaseEnds[-1])
        #genReachPlot(fig,axs[1,ind],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",tgt=zip(x_target,y_target))
        ax=axs[1,1]
        genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",cbtgt=zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1]))

        pdf.savefig()
        plt.close()

    #plt.savefig(pp.out_dir_pdf+outname+".pdf")
    plt.close(fig)

    #plt.clf()
    #plt.cla()
    #return armData,dirShift,targetPre1,errs

def genFigurePertMulti(dat_basenames):
    fig, axs = plt.subplots(ncols=len(dat_basenames), nrows=2, figsize=(30, 20), sharex=False, sharey=False)

    for ind,dat_basename in enumerate(dat_basenames):
        if dat_basename=="":
            continue
        fnames = []
        matchStr = '*' + dat_basename + '*_arm*.dat'
        print 'matching string for *.dat files:', matchStr
        for filename in os.listdir(pp.out_dir):
            if fnmatch.fnmatch(filename, matchStr):
                fnames.append(pp.out_dir+filename)

        if(len(fnames) == 0):
            print "No .dat files found"
            return 0

        filename = fnames[0]

        ree = '(.*)_\w+\.dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)

        fileToPlot = fnames[0]
        armData = armFileRead(fileToPlot)
        #armData = np.loadtxt(fnames[0])
        nums = armData[:,0]
        nums = nums.astype(np.int)
        xs = armData[:,1]
        ys = armData[:,2]
        x_target = armData[:,3]
        y_target = armData[:,4]
        x_cbtgt = armData[:,10]
        y_cbtgt = armData[:,11]
        n = len(xs)
        lastNum = nums[-1]

        ax = axs[0,ind]

        angs,SEMs = doStats(fnames,n,False)

        if(len(fnames) == 1):
            ax.plot(nums, angs)
        else:
            ax.errorbar(nums, angs, yerr=SEMs)
        annotateGraph(ax)
        ax.yaxis.grid(True)

        ax.set_title("bg_"+pp.paramsEnv["learn_bg"]+"__cb_"+pp.paramsEnv["learn_cb"],y=1.04)
        if(int(pp.paramsEnv["sector_reward"]) == 0):
            ax.set_ylim([-0.0,0.8])
            ax.set_yticks(np.arange(-0.9,0.9,0.1))
        else:
            degLim = 180
            ax.set_ylim([-degLim,degLim])
            ax.set_yticks(np.arange(-degLim,degLim,10))

        ax.set_xticks(pp.phaseEnds,minor=True)
        ax.xaxis.grid(True, which='minor')

        rangePre1 = range(0,pp.phaseEnds[0])
        rangeAdapt1 = range(pp.phaseEnds[0],pp.phaseEnds[-2])
        rangePost1 = range(pp.phaseEnds[-2],pp.phaseEnds[-1])
        #genReachPlot(fig,axs[1,ind],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",tgt=zip(x_target,y_target))
        ax=axs[1,1]
        genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",cbtgt=zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1]))

    pos1 = axs[0,0].get_position()
    pos2 = axs[1,0].get_position()
    xLeft = pos1.x0
    yTop = pos1.y0+pos1.height
    posLeftMargin = [0,0,xLeft-0.00,1]             # from left, from down, width, height 
    printParams(fig,posLeftMargin)

    bb = 'shmuelof_'+pp.paramsEnv["pdfSuffix"]
    bb = re.sub('bg._cb.', '', bb)
    
    if(len(fnames) == 1):
        pdfname = pp.out_dir_pdf+bb+"_multi.pdf"
    else:
        pdfname = pp.out_dir_pdf+bb+"_stats_n_="+str(len(fnames))+"_multi.pdf"

    print 'multi',pdfname
    with PdfPages(pdfname) as pdf:
        pdf.savefig()
        plt.close()

    #plt.savefig(pp.out_dir_pdf+outname+".pdf")
    plt.close(fig)

    #plt.clf()
    #plt.cla()
    #return armData,dirShift,targetPre1,errs

def genReachingByPhase(fname):
    fileToPlot = fname
    armData = armFileRead(fileToPlot)
    nums = armData[:,0]
    nums = nums.astype(np.int)
    xs = armData[:,1]
    ys = armData[:,2]
    x_target = armData[:,3]
    y_target = armData[:,4]
    x_actual = armData[:,8]
    y_actual = armData[:,9]
    x_cbtgt = armData[:,10]
    y_cbtgt = armData[:,11]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)

    rangePre1 = range(0,pp.phaseEnds[0])
    rangeAdapt1 = range(pp.phaseEnds[0],pp.phaseEnds[-2])
    rangePost1 = range(pp.phaseEnds[-2],pp.phaseEnds[-1])

    if(len(rangePre1)>0):
        genReachPlot(fig,axs[0,0],xs[rangePre1],ys[rangePre1],nums[rangePre1],"Pre1")
    genReachPlot(fig,axs[0,1],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",tgt=zip(x_target[rangeAdapt1],y_target[rangeAdapt1]),cbtgt=zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1]))
    if(len(rangePost1)>0):
        genReachPlot(fig,axs[1,0],xs[rangePost1],ys[rangePost1],nums[rangePost1],"Post1")

    genReachPlot(fig,axs[1,1],x_actual[rangeAdapt1],y_actual[rangeAdapt1],nums[rangeAdapt1],"Adapt actual pos")

def printParams(fig,pos):
    axlm= fig.add_axes(pos,frameon=False); 
    initX = 0.01;  initY = 0.9; ysubtr=0.02;
    fsz = 15

    paramsToPlot = []
    paramsToPlot.append("targetPre1")
    paramsToPlot.append("dirShift")
    paramsToPlot.append("learn_bg")
    paramsToPlot.append("learn_cb")
    paramsToPlot.append("cbLRate")
    paramsToPlot.append("randomCBStateInit")
    paramsToPlot.append("randomCBStateInitAmpl")
    paramsToPlot.append("trainCBEveryTrial")
    paramsToPlot.append("retrainCB_useCurW")
    paramsToPlot.append("updateCBStateDist")
    paramsToPlot.append("cbRateDepr")
    paramsToPlot.append("cbLRateUpdSpdUp")
    paramsToPlot.append("cbLRateUpdSpdDown")
    paramsToPlot.append("cbLRateUpdSpdMax")
    paramsToPlot.append("")
    paramsToPlot.append("fake_prelearn")
    paramsToPlot.append("wmmax_fake_prelearn")
    paramsToPlot.append("numTrialsPrelearn")
    if "fake_prelearn_tempWAmpl" in pp.paramsEnv:
        paramsToPlot.append("fake_prelearn_tempWAmpl")
    paramsToPlot.append("A_exp")
    paramsToPlot.append("Q")
    paramsToPlot.append("finalNoiseAmpl")
    paramsToPlot.append("")
    paramsToPlot.append("target_rotation1")
    paramsToPlot.append("target_xreverse1")
    paramsToPlot.append("cue_change1")
    paramsToPlot.append("action_change1")
    paramsToPlot.append("endpoint_rotation1")
    paramsToPlot.append("endpoint_xreverse1")
    paramsToPlot.append("force_field1")
    paramsToPlot.append("rotateErr")
    paramsToPlot.append("xreverseErr")
    paramsToPlot.append("")
    if "sess_seed" in pp.paramsEnv:
        paramsToPlot.append("sess_seed")
    paramsToPlot.append("seed")
    paramsToPlot.append("nsessions")

    paramsToPlot.append("")
    paramsToPlot.append("rewardDist")
    paramsToPlot.append("sector_reward")
    paramsToPlot.append("sector_width")
    paramsToPlot.append("sector_thickness")

    i = 0
    for param in paramsToPlot:
        if (param != ""):
            if param in pp.paramsEnv:
                axlm.text(initX,initY-ysubtr*i,param+" = "+ pp.paramsEnv[param],fontsize=fsz)                # count from left top
                i = i+1
            #else:
                #print "Missing param " + param
        else:
            i = i+1

#####################################
#####################################
#####################################

pp.paramsInit('shmuelof.ini') 

maxNumXtics = 21
xtickSkip = float(pp.phaseEnds[-1]) / float(maxNumXtics)
xtickSkip = int(xtickSkip)
print pp.phaseEnds
print pp.phaseNames

if "xtickSkip" in pp.paramsEnv:
    xtickSkip = int(pp.paramsEnv["xtickSkip"])

print 'Python plotting number of arguments: ', len(sys.argv)
#for ind,arg in enumerate(sys.argv):
#    print 'Plotting argument ',ind,' is ',arg
do_multi = 0
if(len(sys.argv)==2):
    dat_basename = sys.argv[1]
elif(len(sys.argv)<=1):
    dat_basename = "" #pp.paramsEnv["dat_basename"]
else:
    do_multi=1
#print 'Argument List:', str(sys.argv)

if(do_multi == 1):
    genFigurePertMulti(sys.argv[1:len(sys.argv)])
elif(do_multi == 0):
    fnames = []
    matchStr = '*' + dat_basename + '*_arm*.dat'
    print 'matching string for *.dat files:', matchStr
    for filename in os.listdir(pp.out_dir):
        if fnmatch.fnmatch(filename, matchStr):
            fnames.append(pp.out_dir+filename)

    pdfForEachSession = int(pp.paramsEnv["pdfForEachSession"])  #it is important to init it here and not later

    if(len(fnames) == 0):
        print "No .dat files found"
    else:
        filename = fnames[0]

        ree = '(.*)_\w+\.dat'
        #ree = '(.*).dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)

        #if( abs(float(pp.paramsEnv["dirShiftInc"]) )>0.00001 ):
        #    genCritAnglePics(fnames)
        #else:
        genFigurePert(fnames,'shmuelof_'+pp.paramsEnv["pdfSuffix"])

    if pdfForEachSession:
        print "Generate pictures for each seed"
        for i,filename in enumerate(fnames):
            ree = '(.*)_\w+\.dat'
            #ree = '(.*).dat'
            basename = os.path.basename(filename)
            name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
            modParamsFileName = filename.replace("_arm","_modParams")
            print "making graph # "+str(i) + " out of " + str(len(fnames))+"  "+modParamsFileName;
            pp.paramsInit(modParamsFileName,False)
            genFigurePert([filename],name);

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


