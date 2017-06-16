import matplotlib.pyplot as plt
import plotparams as pp
from stdplots import *
import universal_plot as up
import stdplots as stdp
import re
import numpy as np
import matplotlib  as mpl

def makeNicer(ax):            
    return
def makeNicerMulti(fig,ax):            
    print("Making nicer!")
    #ax.set_color_cycle(['blue', 'red'])
    ax.set_prop_cycle(plt.cycler('color', ['b','r']))

    #lines = ax.lines
    #lines[0].set_color('b')
    #lines[3].set_color('r')

   # colors = cm(np.linspace(0, 1, len(lines)))
   # for line, c in zip(lines, colors):
   #     line.set_color(c)
    return

def prep(fnames):
    global nums
    global errs
    global SEMs

    up.paramsInitFromArmFname(fnames[0])
    errs,SEMs = doStats(fnames)
    armData = stdp.armFileRead(fnames[0])
    #armData = np.loadtxt(fnames[0])
    nums = armData[:,0]
    nums = nums.astype(np.int)

def annotateCommon(ax):
    n = pp.phaseBegins[-1]
    ax.set_xticks(range(n)[::pp.xtickSkip] )
    ax.set_xlim([0,n])
    ax_ = ax.twiny()
    ax_.set_xlim([0, n])
    ax_.set_xticks(pp.phaseBegins[1:-1])
    ax_.set_xticklabels(pp.phaseNames)
    ax_.xaxis.grid(True,color='w')

    ax.yaxis.grid(True)

    ymin = -40
    ymax = 40 
    ystep = 5
    #ymin = -30
    #ymax = 50 
    #ystep = 10
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,ystep))

    ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

    ax.set_xlabel("Movement Number", size=25,labelpad=15)
    ax.set_ylabel(r"Error in degrees", size=25,labelpad=15)

def makeReach(fig,ax,fname):
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

    genReachPlot(fig,ax,xs,ys,nums,"Reaching plot",tgt=list(zip(x_target,y_target)),cbtgt=list(zip(x_cbtgt,y_cbtgt)))

def makePubPlot(fnames2d,pdf):
    # shiny plotting code goes here

    mpl.rcParams.update({'font.size': 22})

    msz = 12
    xlegendloc = 0.0
    ylegendloc = 1.0
    legendloc = 'upper left'
    
    #ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
    #ax.set_ylabel('Error (cm)')

    figH1=20
    figH2=20
    figH3=20

    figW1=20
    figW2=20
    figW3=20
    
    title_ycoord = 1.05
    
    tsz = pp.target_radius
    nsz = float(pp.paramsEnv["finalNoiseAmpl"] )
    if( pp.plotReachAngles or pp.plotAngleErrs):
        nsz = 2. * pp.eucl2angDist( 0.5*nsz )
    myell = [1,167./255.,66./255.]

    if pp.plotfname == 'pertSmall_cbOnly':
        numcols=2
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1), sharex=True, sharey=False)

        ymin = -40
        ymax = 40 
        ystep = 5

        ax=axs[0]
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("Shift of the vision by 30 degrees", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax=axs[1]
        prep(fnames2d[1])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("Angle error rotation 30", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

    if pp.plotfname == 'strongPert_cbOnly':
        ymin = -110
        ymax = 110 
        ystep = 10

        numcols=2
        numrows=2
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1), sharex=False, sharey=False)

        ax=axs[0,0]
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("Angle error rotation 90", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax=axs[0,1]
        prep(fnames2d[1])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("Angle error x-reflection", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax = axs[1,0]
        genCBStateMaxPlot(fig,ax,fnames2d[0][0].replace('arm','CBState'))
        ax.set_ylabel(r"max of moduli of CB state entries", size=25,labelpad=15)

        ax = axs[1,1]
        genCBStateMaxPlot(fig,ax,fnames2d[1][0].replace('arm','CBState'))
        ax.set_ylabel(r"max of moduli of CB state entries", size=25,labelpad=15)
 
    if pp.plotfname == 'difRotations_cbOnly':
        numcols=3
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1), sharex=False, sharey=False)

        f=pp.reachBoxXsz
        pp.reachBoxXsz=0.3

        ax=axs[0]
        makeReach(fig,ax,fnames2d[0][0])
        ax.set_title("Reaching plot, rotation=60")

        ax=axs[1]
        makeReach(fig,ax,fnames2d[1][0])
        ax.set_title("Reaching plot, rotation=75")

        ax=axs[2]
        makeReach(fig,ax,fnames2d[2][0])
        ax.set_title("Reaching plot, rotation=85")

        pp.reachBoxXsz=f

    if pp.plotfname == 'mildPert_noAC':
        numcols=1
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1), sharex=False, sharey=False)

        ax=axs
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB, Shift", size=30, y=title_ycoord) 

        ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
        ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)

        ax.legend(loc='upper right')
 
    if pp.plotfname == 'strongPert_noAC':
        ymin = -110
        ymax = 110 
        ystep = 10

        numcols=2
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1), sharex=False, sharey=False)

        ax=axs[0]
        prep(fnames2d[0])
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB, rotation 90", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
        ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)

        ax.legend(loc='upper right')

        ax=axs[1]
        prep(fnames2d[1])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB, x-mirror", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
        ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)

        ax.legend(loc='upper right')

    if pp.plotfname == 'strongPert_AC':
        ymin = -110
        ymax = 110 
        ystep = 10

        numcols=2
        numrows=3
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1/2), sharex=False, sharey=False)
        plt.tight_layout()

        ax=axs[0,0]
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB, rotation 90", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
        ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)

        ax.legend(loc='upper right')

        fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
        ax=axs[1,0]
        genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
        ax.set_ylabel(r"max of moduli of CB state entries", size=25,labelpad=15)
        ax=axs[2,0]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True)
        ax.set_ylabel(r"CB learning rate", size=25,labelpad=15)

        ax=axs[0,1]
        prep(fnames2d[1])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB, x-mirror", size=30, y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        ax.axhline(y=tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=-tsz,c="red",linewidth=0.5,zorder=0)
        ax.axhline(y=nsz,c=myell,linewidth=1,zorder=0)
        ax.axhline(y=-nsz,c=myell,linewidth=1,zorder=0)

        ax.legend(loc='upper right')

        fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[1] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[1] ]
        ax=axs[1,1]
        genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
        ax.set_ylabel(r"max of moduli of CB state entries", size=25,labelpad=15)
        ax=axs[2,1]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True)
        ax.set_ylabel(r"CB learning rate", size=25,labelpad=15)

    if pp.plotfname == 'mildPert_AC':
        numcols=1
        numrows=3
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(numcols*figW1, numrows*figH1/2), sharex=False, sharey=False)
        plt.tight_layout()

        ax=axs[0]
        prep(fnames2d[0])
        ax.errorbar(nums, errs, yerr=SEMs)
        annotateCommon(ax)
        ax.set_title("BG and CB with AC, shift", size=30, y=title_ycoord) 

        fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
        ax=axs[1]
        genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
        ax.set_ylabel(r"max of moduli of CB state entries", size=25,labelpad=15)
        ax=axs[2]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True)
        ax.set_ylabel(r"CB learning rate", size=25,labelpad=15)

    plt.savefig(pp.plotfname+'.svg')

#    from textwrap import wrap
#    ax.set_set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)
