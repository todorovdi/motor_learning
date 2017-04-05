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
from stdplots_robbie import *

from matplotlib.backends.backend_pdf import PdfPages
import re

def annotateGraph(ax):
    n = pp.phaseBegins[-1]
    ax.set_xticks(range(n)[::pp.xtickSkip] )
    ax.set_xlim([0,n])
    ax_ = ax.twiny()
    ax_.set_xlim([0, n])
    ax_.set_xticks(pp.phaseBegins[1:-1])
    ax_.set_xticklabels(pp.phaseNames, size=20)
    ax_.xaxis.grid(True,color='k')
    ax.tick_params(axis='x', labelsize=20, pad=15)

###############################

def genMainPlot(fig,ax,fnames,nums):
    angs,SEMs = doStats(fnames)

    if(len(fnames) == 1):
        ax.plot(nums, angs)
    else:
        ax.errorbar(nums, angs, yerr=SEMs)
    annotateGraph(ax)
    ax.yaxis.grid(False)

    stitle = "CB\n"
    if "force_field1" in pp.paramsEnv and abs(float(pp.paramsEnv['force_field1'])) > 0.0:
        stitle = stitle + r"Force Field Perturbation: %.1f" % (round(float(pp.paramsEnv["force_field1"]), 1),)
    elif "percept_rot1" in pp.paramsEnv and abs(float(pp.paramsEnv["percept_rot1"])) > 0.0:
        stitle = stitle + r"Perception Rotation: $%.1f^o$" % (round(float(pp.paramsEnv["percept_rot1"]), 1),)
    elif "percept_xrev1" in pp.paramsEnv and bool(pp.paramsEnv["percept_xrev1"] ) is True:
        stitle = stitle + "X Reversal"

    if (pp.plotReachAngles  != 0 ) :
        ax.set_title("Average Endpoint Angles and SEMs", size=30, y=1.04)
        ax.set_ylabel("Endpoint Angle", size=26)
    else:
        ax.set_title("Average Errors and SEMs", size=30, y=1.04)
        ax.set_ylabel("Error", size=26)

    fig.suptitle(stitle, size=35, y=1.0)

    ymax = pp.y_axis_max
    ymin = pp.y_axis_min

    step = (ymax-ymin) / 10.
    if "y_axis_step" in pp.paramsEnv:
        step = (float(pp.paramsEnv["y_axis_step"]) ) 
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,step))

    ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

    ax.tick_params(axis='x', which='major', labelsize=24)
    ax.tick_params(axis='y', which='major', labelsize=18)

    ax.set_xlabel("Trials", size=26)


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
    #########################################
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

    fig = plt.figure(figsize=(30,20))
    ax = plt.gca()

    if(len(fnames) == 1):
        pdfname = pp.out_dir_pdf_single+outname+".pdf"
    else:
        pdfname = pp.out_dir_pdf+outname+"_stats_n="+str(len(fnames))+".pdf"

    # name for PNG figures
    figname = '_'.join(fnames[0].split('--'))
    figname = figname.replace(' ', '_')
    figname = figname.replace('.pdf', '')
    figname = ''.join(figname.split('='))
    figname = figname.split('/')[-1]
    figname = '/'.join(pdfname.split('/')[:-1]) + '/' + figname

    genMainPlot(fig,ax,fnames,nums)
    plt.savefig(figname+'ERROR'+".png") # SAVE ERROR FIGURE

    # print('pdfname', pdfname)

    print("PNG name: ", figname)

    with PdfPages(pdfname) as pdf:
        pdf.savefig()
        plt.close()
        genReachingByPhase(fileToPlot)
        plt.savefig(figname+'REACH'+".png") # SAVE REACH FIGURE
        pdf.savefig()
        plt.close()

        fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        ax = axs[0,1]
        if(int(pp.paramsEnv["nc"]) > 1):
            genBGWeightsPlot(fig,ax,fileToPlot.replace('arm','weights2'),1)
            annotateGraph(ax)

        fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
        ax = axs[0,0]
        #genCBStatePlot(fig,ax,fileToPlot.replace('arm','CBState'))
        genRwdPlot(fig,ax,fileToPlot.replace('arm','output'))
        annotateGraph(ax)

        ax = axs[0,1]
        #genCBTuningPlot(fig,ax,fileToPlot.replace('arm','CBTuning'))
        genCBStateMaxPlot(fig,ax,fileToPlot.replace('arm','CBState'))
        annotateGraph(ax)
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

        ax = axs[1,0]
        genCBMiscPlot(fig,ax,fileToPlot.replace('arm','CBMisc'))
        annotateGraph(ax)
        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)

        #x_target = armData[:,3]
        #y_target = armData[:,4]
        x_cbtgt = armData[:,10]
        y_cbtgt = armData[:,11]

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
        ax=axs[1,1]
        cbtgt = list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1]))
        genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],figName,cbtgt=cbtgt)
        pdf.savefig()
        plt.close()

        #ax = plt.gca
        fig, ax = plt.subplots(ncols=1, nrows=1, figsize=(30, 20))
        genMainPlot(fig,ax,fnames,nums)
        ax.set_title(pp.paramsEnv["pdfSuffix"])

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
        nr = 2
        nc = l
    fig, axs = plt.subplots(ncols=nc, nrows=nr, figsize=(10*l, 20), sharex=False, sharey=False)

    fnames2d = []
    for ind,dat_basename in enumerate(dat_basenames_nonempty):
        fnames = basename2armDatList(dat_basename) 
        #fnames = removeWrongDataLen(fnames,pp.phaseBegins[-1])

        if(len(fnames) == 0):
            print("No .dat files found")
            return 0
        
        fnames2d.append(fnames)

        paramsInitFromArmFname(fnames[0])

        fileToPlotReach = fnames[0]
        armData = armFileRead(fileToPlotReach)
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
            ax = axs[0,ind]
        else:
            ax = axs
            
        from textwrap import wrap

        genMainPlot(fig,ax,fnames,nums)
        
        #ax.set_title("bg_"+pp.paramsEnv["learn_bg"]+"__cb_"+pp.paramsEnv["learn_cb"],y=1.04)
        ax.set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)

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

            ax = axs[1,ind]

            genReachPlot(fig,ax,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],
                            figName,cbtgt=list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1])))

    if pp.multiSameGraph == 0:
        ax = axs[1,ind]
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

    # fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(30, 20), sharex=False, sharey=False)
    fig = plt.figure(figsize=(30,20))
    axs = plt.gca()

    rangePre1 = range(0,pp.phaseBegins[1])
    figName = "Adapt1"
    if pp.emphPhase != -1:
        ep = pp.emphPhase
        rangeAdapt1 = range(pp.phaseBegins[ep],pp.phaseBegins[ep+1])
        figName = pp.paramsEnv["name"+str(ep)]
    else:
        rangeAdapt1 = range(pp.phaseBegins[1],pp.phaseBegins[-2])
    rangePost1 = range(pp.phaseBegins[-2],pp.phaseBegins[-1])

    # if(len(rangePre1)>0):
        # genReachPlot(fig,axs[0,0],xs[rangePre1],ys[rangePre1],nums[rangePre1],"Pre1")
    genReachPlot(fig,axs,xs[rangeAdapt1],ys[rangeAdapt1],nums[rangeAdapt1],figName,tgt=list(zip(x_target[rangeAdapt1],y_target[rangeAdapt1])),cbtgt=list(zip(x_cbtgt[rangeAdapt1],y_cbtgt[rangeAdapt1])))
    # if(len(rangePost1)>0):
        # genReachPlot(fig,axs[1,0],xs[rangePost1],ys[rangePost1],nums[rangePost1],"Post1")

    # genReachPlot(fig,axs[1,1],x_actual[rangeAdapt1],y_actual[rangeAdapt1],nums[rangeAdapt1],figName+" actual pos")
    # plt.savefig(pp.out_dir_pdf+fname+".png")

def printParams(fig,pos):
    axlm= fig.add_axes(pos,frameon=False); 
    initX = 0.01;  initY = 0.9; ysubtr=0.02;
    fsz = 15

    paramsToPlot = []
    paramsToPlot.append("learn_bg")
    paramsToPlot.append("learn_cb")
    paramsToPlot.append("learn_cb2")
    paramsToPlot.append("resetRPre0")
    paramsToPlot.append("resetRPre1")
    paramsToPlot.append("resetRPre3")

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
    paramsToPlot.append("cbLRateUpdSpdMax")

    paramsToPlot.append("")
    paramsToPlot.append("fake_prelearn")
    paramsToPlot.append("wmmax_fake_prelearn")
    paramsToPlot.append("numTrialsPrelearn")
    paramsToPlot.append("fake_prelearn_tempWAmpl")
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
