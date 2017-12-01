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

def addTgtSz(ax):
    tsz = pp.target_radius

    linew=0.5
    #ax.axhline(y=tsz,c="red",linewidth=linew,zorder=0)
    #ax.axhline(y=-tsz,c="red",linewidth=linew,zorder=0)
    #ax.axhline(y=nsz,c=myell,linewidth=linew,zorder=0)
    #ax.axhline(y=-nsz,c=myell,linewidth=linew,zorder=0)

    ax.axhline(y=10.*tsz,c=pp.mainColor,linewidth=linew,ls='dashed',zorder=0)
    ax.axhline(y=-10.*tsz,c=pp.mainColor,linewidth=linew,ls='dashed',zorder=0)

    #addNoiseSz(ax)

def prep(fnames):
    global nums
    global errs
    global SEMs

    up.paramsInitFromArmFname(fnames[0])
    errs,SEMs = doStats(fnames)
    errs = (-10.)*errs
    SEMs = (-10.)*SEMs
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

    #ymin = -1.5
    #ymax = 2.3 
    ymin = -2
    ymax = 3 
    step = 1
    #ymin = -30
    #ymax = 50 
    #step = 10
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.append(np.arange(ymin,0,step),np.arange(0,ymax,step)) )

    ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

def makePubPlot(fnames2d,pdf):
    globalFontSz=10
    mpl.rcParams.update({'font.size': globalFontSz})

    wordTextWidth=6.78
    wordSingleFigHeight=3.38

    # shiny plotting code goes here
    if(len(fnames2d) != 6):
        print("---Error -- not enough data to plot, exiting! ")
        return

    fig, axs = plt.subplots(ncols=2, nrows=2, figsize=(6, 6), sharex=True, sharey=True)

    plt.subplots_adjust(left=0.09, right=0.98, bottom=0.09, top=0.92, wspace=0.2, hspace=0.2)

    fnames_nonEB_HD = fnames2d[0]
    fnames_nonEB_PD = fnames2d[1]
    fnames_nonEB_control_HD = fnames2d[2]
    fnames_EB_HD = fnames2d[3]
    fnames_EB_PD = fnames2d[4]
    fnames_EB_control_HD = fnames2d[5]

    fnames_nonEB_control_PD = fnames2d[6]
    fnames_EB_control_PD = fnames2d[7]

#    for i,f in enumerate(fnames2d):
#        print i,len(f)

    msz = 4
    xlegendloc = 0.0
    ylegendloc = 0.0
    legendloc = 'lower left'
    
    #fig.suptitle()
    fig.suptitle("EB and nonEB learning", size=1.2*globalFontSz,y=0.99)

    ax = axs[0,0]
    prep(fnames_EB_control_HD)
    ax.errorbar(nums, errs, yerr=SEMs, marker='o', color='blue',markersize=msz,label='CHD',linestyle='None',capsize=pp.capsz)
    prep(fnames_EB_HD)
    ax.errorbar(nums, errs, yerr=SEMs, marker='s', color='skyblue',markersize=msz,label='HD',linestyle='None',capsize=pp.capsz)
    annotateCommon(ax)
    
    ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
    ax.set_ylabel('Error (cm)')
    #addTgtSz(ax)

    ax = axs[0,1]
    prep(fnames_EB_control_PD)
    ax.errorbar(nums, errs, yerr=SEMs, marker='d', color='blue',markersize=msz,label='CPD',linestyle='None',capsize=pp.capsz)
    prep(fnames_EB_PD)
    ax.errorbar(nums, errs, yerr=SEMs, marker='^', color='skyblue',markersize=msz,label='PD',linestyle='None',capsize=pp.capsz)
    annotateCommon(ax)
    ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
    #addTgtSz(ax)

    #######################################
    lightcoral = [0.9, 0.4, 0.4]
    fireblack = [0.6, 0.0, 0.0]
    ax = axs[1,0]
    prep(fnames_nonEB_control_HD)
    ax.errorbar(nums, -1.*errs, yerr=SEMs, marker='o', color=fireblack,markersize=msz,label='CHD',linestyle='None',capsize=pp.capsz)
    prep(fnames_nonEB_HD)
    ax.errorbar(nums, -1.*errs, yerr=SEMs, marker='s', color=lightcoral,markersize=msz,label='HD',linestyle='None',capsize=pp.capsz)
    annotateCommon(ax)
    ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
    ax.set_ylabel('Error (cm)')
    ax.set_xlabel("Movement Number", labelpad=8)
    #addTgtSz(ax)

    ax = axs[1,1]
    prep(fnames_nonEB_control_PD)
    ax.errorbar(nums, -1.*errs, yerr=SEMs, marker='d', color=fireblack,markersize=msz,label='CPD',linestyle='None',capsize=pp.capsz)
    prep(fnames_nonEB_PD)
    ax.errorbar(nums, -1.*errs, yerr=SEMs, marker='^', color=lightcoral,markersize=msz,label='PD',linestyle='None',capsize=pp.capsz)
    annotateCommon(ax)
    ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
    ax.set_xlabel("Movement Number", labelpad=8)
    #addTgtSz(ax)

    ##########################

    noExt=pp.out_dir_pdf + pp.plotfname
    svgname=noExt+'.svg'
    print svgname
    plt.savefig(svgname)#,bbox_inches='tight')

    import os
    inkscapeArgs ='inkscape --without-gui --export-emf="'+noExt+'.emf" "'+svgname+'"'
    print inkscapeArgs
    os.system(inkscapeArgs)


    # plt.plot([1,2,3], label="test1")
    #plt.plot([3,2,1], label="test2")
    ## Place a legend to the right of this smaller subplot.

#    from textwrap import wrap
#    ax.set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)
