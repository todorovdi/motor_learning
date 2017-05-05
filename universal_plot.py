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
try:
    import ConfigParser
except ModuleNotFoundError:
    import configparser as ConfigParser
import io
import math
import sys

from matplotlib import cm
from scipy import stats

import plotparams as pp
from stdplots import *

from matplotlib.backends.backend_pdf import PdfPages
import re

def annotateGraph(ax):
    n = pp.phaseBegins[-1]
    ax.set_xticks(range(n)[::pp.xtickSkip] )
    ax.set_xlim([0,n])
    ax_ = ax.twiny()
    ax_.set_xlim([0, n])
    ax_.set_xticks(pp.phaseBegins[1:-1])
    ax_.set_xticklabels(pp.phaseNames)
    ax_.xaxis.grid(True,color='k')

###############################

def genMainPlot(ax,fnames,nums):
    angs,SEMs = doStats(fnames)

    if(len(fnames) == 1):
        ax.plot(nums, angs)
    else:
        ax.errorbar(nums, angs, yerr=SEMs)
    annotateGraph(ax)
    ax.yaxis.grid(True)

    print("last nums ",nums[-1], " last phase ",pp.phaseBegins[-1])

    if (pp.plotReachAngles  != 0 ) :
        ax.set_title("Average Endpoint Angles and SEMs", y=1.04)
        ax.set_ylabel("Endpoint Angle")
    else:
        ax.set_title("Average Errors and SEMs", y=1.04)
        ax.set_ylabel("Error")
        if pp.signed_dist_err:
            ax.set_ylabel("x-Error")
    ymin = 0.
    ymax = pp.y_axis_max 
    ymin = pp.y_axis_min
    step = (ymax-ymin) / 10.
    if "y_axis_step" in pp.paramsEnv:
        step = (float(pp.paramsEnv["y_axis_step"]) ) 
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,step))

    ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

    tsz = pp.target_radius
    ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
    ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)

    nsz = float(pp.paramsEnv["finalNoiseAmpl"] )
    if( pp.plotReachAngles or pp.plotAngleErrs):
        nsz = 2. * pp.eucl2angDist( 0.5*nsz )
    myell = [1,167./255.,66./255.]
    ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
    ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)
  
#    if pp.plotPubFile != "":
#        import imp
#        nicener = imp.load_source(pp.plotPubFile,pp.plotPubFile)
#        nicener.makeNicer(ax)

def removeWrongDataLen(fnames,n):   # check if all have the same data length
    fngood = []
    for fname in fnames:
        paramsInitFromArmFname(fname)
        n = pp.phaseBegins[-1]
        armData = armFileRead(fname)
        nrows,ncols = np.shape(armData) 
        if nrows != n:
            print("------- Warning: wrong length of data file table, maybe calc was terminated too early -> ",fname)
        else:
            fngood.append(fname)
    return fngood

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

    print("plotting from ",len(fnames)," sessions")

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

    genMainPlot(ax,fnames,nums)

    pos1 = axs[0,0].get_position()
    xLeft = pos1.x0
    yTop = pos1.y0+pos1.height
    posLeftMargin = [0,0,xLeft-0.00,1]             # from left, from down, width, height 
    printParams(fig,posLeftMargin)

    if(len(fnames) == 1):
        pdfname = pp.out_dir_pdf_single+outname+".pdf"
    else:
        pdfname = pp.out_dir_pdf+outname+"_stats_n="+str(len(fnames))+".pdf"

    print('pdfname', pdfname)

    with PdfPages(pdfname) as pdf:
        pdf.savefig()
        plt.close()
        genReachingByPhase(fileToPlot)
        pdf.savefig()
        plt.close()

        #fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        #ax = axs[0,1]
        #if(int(pp.paramsEnv["nc"]) > 1):
        #    genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'),1)
        #    annotateGraph(ax)

        fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        ax = axs[0,0]
        genCBStateMaxPlot(fig,ax,fileToPlot.replace('arm','CBState'))
        annotateGraph(ax)
        #genCBStatePlot(fig,ax,fileToPlot.replace('arm','CBState'))
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

        ax = axs[0,1]
        #genCBTuningPlot(fig,ax,fileToPlot.replace('arm','CBTuning'))
        genRwdPlot(fig,ax,fileToPlot.replace('arm','output'))
        annotateGraph(ax)
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

        ax = axs[1,0]
        genCBMiscPlot(fig,ax,fileToPlot.replace('arm','CBMisc'))
        annotateGraph(ax)
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

        ax = axs[1,1]
        genCBMisc2Plot(fig,ax,fileToPlot.replace('arm','CBMisc'))
        annotateGraph(ax)
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

#        #x_target = armData[:,3]
#        #y_target = armData[:,4]
#        x_cbtgt = armData[:,10]
#        y_cbtgt = armData[:,11]
#
#        rangePre1 = range(0,pp.phaseBegins[1])
#        figName = "Adapt1"
#        if pp.emphPhase != -1:
#            ep = pp.emphPhase
#            rangeAdapt1 = range(pp.phaseBegins[ep],pp.phaseBegins[ep+1])
#            figName = pp.paramsEnv["name"+str(ep)]
#        else:
#            rangeAdapt1 = range(pp.phaseBegins[1],pp.phaseBegins[-2])
#        rangePost1 = range(pp.phaseBegins[-2],pp.phaseBegins[-1])
#        #genReachPlot(fig,axs[1,ind],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",tgt=zip(x_target,y_target))
#        ax=axs[1,1]
#        genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],figName,cbtgt=list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1])))

        pdf.savefig()
        plt.close()

        #ax = plt.gca
        fig, ax = plt.subplots(ncols=1, nrows=1, figsize=(30, 20))
        
        mpl.rcParams.update({'font.size': 35})
        genMainPlot(ax,fnames,nums)
        ax.set_title(pp.paramsEnv["pdfSuffix"])
        #ax.set_title("")

        pdf.savefig()
        plt.close()

    #plt.savefig(pp.out_dir_pdf+outname+".pdf")
    plt.close(fig)

    #plt.clf()
    #plt.cla()
    #return armData,dirShift,targetPre1,errs

def basename2armDatList(basename):
    matchStr = basename.lstrip() + '_seed_*_arm*.dat'
    print('matching string for *.dat files:', matchStr)
    fnames = []
    for filename in os.listdir(pp.out_dir):
        if fnmatch.fnmatch(filename, matchStr):
            fnames.append(pp.out_dir+filename)
    return fnames

def genFigurePertMulti(dat_basenames):
    dat_basenames_nonempty = []
    for ind,dat_basename in enumerate(dat_basenames):
        if dat_basename=="":
            continue
        else:
            dat_basenames_nonempty.append(dat_basename)

    l =  len(dat_basenames_nonempty)
    if pp.multiSameGraph == 1:
        nr = 1
        nc = 1
    else:
        nr = 4
        nc = l

    if pp.multi_onlyMainPlots:
        if pp.multi_ncols > 0:
            ncfinal=pp.multi_ncols
            nr=int( math.ceil( float(l) / float(ncfinal) ) )
        else:
            ncfinal=int(math.ceil(nc/2.) )
        fig, axs = plt.subplots(ncols=ncfinal, nrows=nr, figsize=(5+10*ncfinal, 10*nr), sharex=False, sharey=False)
    else:
        fig, axs = plt.subplots(ncols=nc, nrows=nr, figsize=(5+10*l, 20), sharex=False, sharey=False)

    plt.subplots_adjust(left=0.16,right=0.98,bottom=0.1)

    if pp.multi_onlyMainPlots:
        axs=axs.ravel()

    fnames2d = []
    for ind,dat_basename in enumerate(dat_basenames_nonempty):
        fnames = basename2armDatList(dat_basename) 
        #fnames = removeWrongDataLen(fnames,pp.phaseBegins[-1])

        if(len(fnames) == 0):
            print("No .dat files found")
            return 0
        
        fnames2d.append(fnames)

        paramsInitFromArmFname(fnames[0])

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

        if pp.multiSameGraph == 0:
            if pp.multi_onlyMainPlots:
                ax = axs[ind]
            else:
                ax = axs[0,ind]
        else:
            ax = axs
            
        from textwrap import wrap

        genMainPlot(ax,fnames,nums)
        #ax.set_title("bg_"+pp.paramsEnv["learn_bg"]+"__cb_"+pp.paramsEnv["learn_cb"],y=1.04)
        ax.set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.04)

        if pp.multiSameGraph == 0:
            rangePre1 = range(0,pp.phaseBegins[1])
            figName = "Adapt1"
            if pp.emphPhase != -1:
                ep = pp.emphPhase
                rangeAdapt1 = range(pp.phaseBegins[ep],pp.phaseBegins[ep+1])
                figName = pp.paramsEnv["name"+str(ep)]
            else:
                rangeAdapt1 = range(pp.phaseBegins[1],pp.phaseBegins[-2])
            rangePost1 = range(pp.phaseBegins[-2],pp.phaseBegins[-1])
            #genReachPlot(fig,axs[1,ind],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],"Adapt1",tgt=zip(x_target,y_target))

        if pp.multi_onlyMainPlots == 0:
            ax = axs[1,ind]
            genBGActivityPlot(fig,ax,fileToPlot.replace('arm','var_dyn2'))
            annotateGraph(ax)

            ax = axs[2,ind]
            genCBStateMaxPlot(fig,ax,fileToPlot.replace('arm','CBState'))
            annotateGraph(ax)
            ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

            ax = axs[3,ind]
            #genCBMiscPlot(fig,ax,fileToPlot.replace('arm','CBMisc'))
            genRwdPlot(fig,ax,fileToPlot.replace('arm','output'))
            annotateGraph(ax)
            ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

            #genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],figName,cbtgt=list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1])))

    if pp.multiSameGraph == 0:
        if pp.multi_onlyMainPlots:
            ax = axs[ind]
        else:
            ax = axs[nr-1,ind]
    else: 
        ax = axs
    
    pos1 = ax.get_position()
    xLeft = pos1.x0
    yTop = pos1.y0+pos1.height
    posLeftMargin = [0,0,xLeft-0.00,1]             # from left, from down, width, height 
    printParams(fig,posLeftMargin)

    bb = pp.paramsEnv["pdfSuffix"]
    bb = re.sub('bg._cb.', '', bb)
    
    if(len(fnames) == 1):
        pdfname = pp.out_dir_pdf+bb+"_multi.pdf"
    else:
        pdfname = pp.out_dir_pdf+bb+"_stats_n_="+str(len(fnames))+"_multi.pdf"

    print('multi',pdfname)
    with PdfPages(pdfname) as pdf:
        pdf.savefig()
        plt.close()

        if pp.plotPubFile != "":

            import imp
            modName,_ = pp.plotPubFile.split('.')
            nicener_ = imp.load_source('nicener',pp.plotPubFile)
            #nicener.makeNicerMulti(fig,ax)
            import nicener
            nicener.makePubPlot(fnames2d)

            pdf.savefig()
            plt.close()

    #plt.savefig(pp.out_dir_pdf+outname+".pdf")
    plt.close(fig)

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

    rangePre1 = range(0,pp.phaseBegins[1])
    figName = "Adapt1"
    if pp.emphPhase != -1:
        ep = pp.emphPhase
        rangeAdapt1 = range(pp.phaseBegins[ep],pp.phaseBegins[ep+1])
        figName = pp.paramsEnv["name"+str(ep)]
    else:
        rangeAdapt1 = range(pp.phaseBegins[1],pp.phaseBegins[-2])
    rangePost1 = range(pp.phaseBegins[-2],pp.phaseBegins[-1])

    if(len(rangePre1)>0):
        genReachPlot(fig,axs[0,0],xs[rangePre1],ys[rangePre1],nums[rangePre1],"Pre1")
    genReachPlot(fig,axs[0,1],xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],figName,tgt=list(zip(x_target[rangeAdapt1],y_target[rangeAdapt1])),cbtgt=list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1])))
    if(len(rangePost1)>0):
        genReachPlot(fig,axs[1,0],xs[rangePost1],ys[rangePost1],nums[rangePost1],"Post1")

    genReachPlot(fig,axs[1,1],x_actual[rangeAdapt1],y_actual[rangeAdapt1],nums[rangeAdapt1],figName+" actual pos")

def printParams(fig,pos):
    axlm= fig.add_axes(pos,frameon=False); 
    initX = 0.01;  initY = 0.9; ysubtr=0.02;
    fsz = 15

    paramsToPlot = []
    paramsToPlot.append("learn_bg")
    paramsToPlot.append("learn_cb")
    paramsToPlot.append("learn_cb2")
    paramsToPlot.append("setRPre0")
    paramsToPlot.append("setRPre1")
    paramsToPlot.append("setRPre3")

    paramsToPlot.append("")
    paramsToPlot.append("HD_D2activity_reduction")
    paramsToPlot.append("PD_LTP_reduction")

    paramsToPlot.append("")
    paramsToPlot.append("cbLRate")
    paramsToPlot.append("cbLRateMax")
    #paramsToPlot.append("randomCBStateInit")
    #paramsToPlot.append("randomCBStateInitAmpl")
    #paramsToPlot.append("trainCBEveryTrial")
    #paramsToPlot.append("retrainCB_useCurW")
    paramsToPlot.append("updateCBStateDist")
    paramsToPlot.append("cbRateDepr")
    paramsToPlot.append("cbLRateUpdSpdUp")
    paramsToPlot.append("cbLRateUpdSpdDown")
    #paramsToPlot.append("cbLRateUpdSpdMax")

    paramsToPlot.append("cbErrDepth")
    paramsToPlot.append("cbLRateUpdAbsErr_threshold")
    paramsToPlot.append("cbLRateUpdErrRatio_threshold")


    paramsToPlot.append("")
    paramsToPlot.append("fake_prelearn")
    paramsToPlot.append("wmmaxFP")
    paramsToPlot.append("numTrialsPrelearn")
    paramsToPlot.append("fake_prelearn_tempWAmpl")
    paramsToPlot.append("lam2")
    paramsToPlot.append("A_exp")
    paramsToPlot.append("Q")
    paramsToPlot.append("finalNoiseAmpl")

    paramsToPlot.append("")
    paramsToPlot.append("cue1")
    paramsToPlot.append("target_rotation1")
    paramsToPlot.append("target_xreverse1")
    paramsToPlot.append("endpoint_rotation1")
    paramsToPlot.append("endpoint_xreverse1")
    paramsToPlot.append("force_field1")
    paramsToPlot.append("actPrelearn1")

    paramsToPlot.append("gradedReward")
    paramsToPlot.append("rwdGradePower")
    paramsToPlot.append("perfBasedReward")
    paramsToPlot.append("perfRwdMult")
    paramsToPlot.append("perfRwdErrChange_threshold")
     

    paramsToPlot.append("")
    paramsToPlot.append("learn_cb2")
    paramsToPlot.append("resetCBState2")

    paramsToPlot.append("")
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

def paramsInitFromArmFname(filename):
    ree = '(.*).dat'
    basename = os.path.basename(filename)
    name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
    pfname = filename.replace("_arm","_modParams")
    #print 'params init: ',pfname
    pp.paramsInit(pfname,False)
#####################################
#####################################
#####################################

if __name__ == '__main__':
    print('Python plotting number of arguments: ', len(sys.argv))

    for ind,arg in enumerate(sys.argv):
        print('Plotting argument ',ind,' is '+arg+'$')

    val = ""
    iniParamFound = 0
    for farg in sys.argv:
        ree = '.*--ini=(.*\.ini).*'
        #ree = '(.*).dat'
        s = re.match(ree,farg)  #re.search(ree,fnames[0])
        if s!= None:
            val = s.group(1)

    if val == "":
        paramFileName = 'shmuelof.ini'
    else:
        paramFileName = val

    pp.paramsInit(paramFileName)
    pp.exportVarsInit(pp.paramsEnv)

    dat_basenames = sys.argv[1:]

    if(len(dat_basenames) >  1):
        print("Plot single session")
        genFigurePertMulti(dat_basenames)
    elif(len(dat_basenames) == 1):
        fnames = basename2armDatList(dat_basenames[0]) 
        #matchStr = dat_basenames[0] + '_seed_*_arm*.dat'
        #print 'matching string for *.dat files:', matchStr
        #for filename in os.listdir(pp.out_dir):
        #    if fnmatch.fnmatch(filename, matchStr):
        #        fnames.append(pp.out_dir+filename)

        if(len(fnames) == 0):
            print("No .dat files found")
        else:
            paramsInitFromArmFname(fnames[0])

        #fnames = removeWrongDataLen(fnames,pp.phaseBegins[-1])

        if(len(fnames) == 0):
            print("No .dat files found")
        else:
            paramsInitFromArmFname(fnames[0])

            #if( abs(float(pp.paramsEnv["dirShiftInc"]) )>0.00001 ):
            #    genCritAnglePics(fnames)
            #else:
            genFigurePert(fnames,pp.paramsEnv["pdfSuffix"])

        if pp.pdfForEachSession:
            print("Generate pictures for each seed")
            for i,filename in enumerate(fnames):
                print("making graph # "+str(i) + " out of " + str(len(fnames)))
                paramsInitFromArmFname(filename)
                genFigurePert([filename],pp.paramsEnv["pdfSuffix"] + name);
