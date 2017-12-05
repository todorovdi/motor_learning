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

def annotateCommon(ax,ystep=5):
    n = pp.phaseBegins[-1]
    #ax.set_xticks(range(n)[::pp.xtickSkip] )
    ax.set_xlim([0,n])
    ax_ = ax.twiny()
    ax_.set_xlim([0, n])

    xtickloc = []
    for i in range(pp.numPhases):
        xtickk = float(pp.phaseBegins[i] + pp.phaseBegins[i+1] )/2.
        xtickloc.append(xtickk)

    labels = ['BASELINE']+pp.phaseNames

    ax_.set_xticks(xtickloc)
    ax_.set_xticklabels(labels)
    #ax_.set_xticks(pp.phaseBegins[1:-1])
    #ax_.set_xticklabels(pp.phaseNames)
    #ax_.xaxis.grid(True,color='w')

    ax_.tick_params(axis=u'x', which=u'both',length=0)

    ax.yaxis.grid(True,linestyle='--',dashes=(2,14))

    ymin = -40
    ymax = 40 
    #ymin = -30
    #ymax = 50 
    #ystep = 10
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,ystep))

    ax.set_xticks(pp.phaseBegins[1:-1])
    ax.xaxis.grid(True, linestyle='--',dashes=(1,8))

    ax.set_xlabel("Movement Number", labelpad=7)
    ax.set_ylabel(r"Error in degrees",labelpad=7)

def makeReach(fig,ax,fname):
    fileToPlot = fname
    armData = armFileRead(fileToPlot)
    nums = armData[:,0]
    nums = nums.astype(np.int)
    xs = armData[:,1]
    ys = armData[:,2]
    #x_target = armData[:,3]
    #y_target = armData[:,4]
    #x_actual = armData[:,8]
    #y_actual = armData[:,9]
    x_cbtgt = armData[:,10]
    y_cbtgt = armData[:,11]

    #genReachPlot(fig,ax,xs,ys,nums,"Reaching plot",tgt=list(zip(x_target,y_target)),cbtgt=list(zip(x_cbtgt,y_cbtgt)),ptlabels=False)

    xmin=-0.1
    xmax=0.3
    ymin=0.2
    ymax=0.8

    ax.set_ylim([ymin,ymax]) # set ymin
    ax.set_xlim([xmin,xmax])
    ax.set_xticks(np.arange(xmin,xmax,0.1))
    ax.set_yticks(np.arange(ymin,ymax,0.1))

    ax.scatter(xs,ys,c=range(len(xs)),lw=0.0,cmap='inferno',s=25)

    cross_sz=0.03
    cross_lw=1

    x = x_cbtgt[1]
    y = y_cbtgt[1]
    ax.hlines(y,x-cross_sz,x+cross_sz,lw=cross_lw)
    ax.vlines(x,y-cross_sz,y+cross_sz,lw=cross_lw)

    ax.grid()

def addNoiseSz(ax):
    linew=0.5

    nsz = float(pp.paramsEnv["finalNoiseAmpl"] )
    if( pp.plotReachAngles or pp.plotAngleErrs):
        nsz = 2. * pp.eucl2angDist( 0.5*nsz )
    #myell = [1,167./255.,66./255.]

    ax.axhline(y=nsz,c=pp.mainColor,linewidth=linew,ls='dashdot',zorder=0)
    ax.axhline(y=-nsz,c=pp.mainColor,linewidth=linew,ls='dashdot',zorder=0)

def addTgtSz(ax,showNoise=True):
    tsz = pp.target_radius

    linew=0.5
    #ax.axhline(y=tsz,c="red",linewidth=linew,zorder=0)
    #ax.axhline(y=-tsz,c="red",linewidth=linew,zorder=0)
    #ax.axhline(y=nsz,c=myell,linewidth=linew,zorder=0)
    #ax.axhline(y=-nsz,c=myell,linewidth=linew,zorder=0)

    ds=(15,15)
    ax.axhline(y=tsz,c=pp.mainColor,linewidth=linew,ls='dashed',zorder=0,dashes=ds)
    ax.axhline(y=-tsz,c=pp.mainColor,linewidth=linew,ls='dashed',zorder=0,dashes=ds)

    if showNoise:
        addNoiseSz(ax)

def makePubPlot(fnames2d,pdf):
    # shiny plotting code goes here

    globalFontSz=10
    mpl.rcParams.update({'font.size': globalFontSz})

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
    
    title_ycoord = 1.11

    linew = 0.8
    

    # 17.23 cm  x 8.59 cm  = 6.78 x 3.38
    wordTextWidth=6.78
    wordSingleFigHeight=3.38

    #subplots_adjust(left=None, bottom=None, right=None, top=None, wspace=None, hspace=None)

    #The parameter meanings (and suggested defaults) are:

    #left  = 0.125  # the left side of the subplots of the figure
    #right = 0.9    # the right side of the subplots of the figure
    #bottom = 0.1   # the bottom of the subplots of the figure
    #top = 0.9      # the top of the subplots of the figure
    #wspace = 0.2   # the amount of width reserved for blank space between subplots
    #hspace = 0.2   # the amount of height reserved for white space between subplots

    capsz=1.5
    rotColor='blue'
    shiftColor='darkgreen'
    xrevColor='red'
    xrevColor='firebrick'

    yminSRot=-100
    ymaxSRot=100
    ylimSRot=[-yminSRot,ymaxSRot]
    yticksSRot = np.append(np.arange(yminSRot,0,30),np.arange(0,ymaxSRot,30) )

    if pp.plotfname == 'mildPert_cbOnly':
        numcols=1
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth/1.9, wordSingleFigHeight), sharex=True, sharey=False)

        # reducing left reduces left margin
        plt.subplots_adjust(left=0.19, right=0.98, bottom=0.15, top=0.84, wspace=0.4)
        #plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)

        ymin = -40
        ymax = 40 
        ystep = 10

        ax=axs#[0]
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs)
        handle1, = shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("Shift and rotation by 30 degrees", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))
        #panel name
        #ax.text(-0.1, 1.15, 'A', transform=ax.transAxes,    fontsize=16, fontweight='bold', va='top', ha='right')

        #ax.get_lines()[0].set_color("grey")

        addNoiseSz(ax)

        #ax=axs[1]
        prep(fnames2d[1])
        #ax.errorbar(nums, errs, yerr=SEMs)
        handle2, = shadedErrorbar(ax,nums,errs,SEMs,colorMean=rotColor,lw=linew)
        #annotateCommon(ax)
        #ax.set_title("Rotation by 30 degrees", y=title_ycoord) 
        #ax.set_ylim([ymin,ymax])
        #ax.set_yticks(np.arange(ymin,ymax,ystep))
        #ax.text(-0.1, 1.15, 'B', transform=ax.transAxes, fontsize=16, fontweight='bold', va='top', ha='right')

        plt.legend(handles=[handle1, handle2], labels=['shift', 'rotation'], loc='lower left')
        
        addNoiseSz(ax)

    if pp.plotfname == 'strongPert_cbOnly':
        ymin = -180
        ymax = 180 
        ystep = 30
        cbStateLim=6

        numcols=2
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth, wordSingleFigHeight), sharex=False, sharey=False)

        plt.subplots_adjust(left=0.11, right=0.98, bottom=0.13, top=0.85, wspace=0.4, hspace=0.4)

        ax=axs[0]
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs)
        handle1, = shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor,lw=linew)
        #ax.get_lines()[0].set_color("grey")

        prep(fnames2d[1])
        #ax.errorbar(nums, errs, yerr=SEMs)
        handle2, = shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=rotColor,lw=linew)

        if len(fnames2d)>3:
            from matplotlib import colors as mcolors

            redAlpha=0.5

            shiftColor2 = []
            rotColor2 =  []
            # just because it is tuple and not array, which is not convenient
            shiftColor2 = [r for r in mcolors.to_rgba(shiftColor) ]
            rotColor2 = [r for r in mcolors.to_rgba(rotColor) ]
            shiftColor2[-1]=redAlpha
            rotColor2[-1]  =redAlpha

            prep(fnames2d[3])
            #ax.errorbar(nums, errs, yerr=SEMs)
            handle1, = shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor2,lw=linew)
            #ax.get_lines()[0].set_color("grey")

            prep(fnames2d[4])
            #ax.errorbar(nums, errs, yerr=SEMs)
            handle2, = shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=rotColor2,lw=linew)

        annotateCommon(ax)
        ax.set_title("rotation and shift 90, CB only", y=title_ycoord) 
        ax.set_ylim(ylimSRot)
        ax.set_yticks(yticksSRot )
        ax.text(-0.1, 1.15, 'A', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        #addNoiseSz(ax)
        plt.legend(handles=[handle1, handle2], labels=['shift', 'rotation'], loc='lower left')

        ax=axs[1]
        prep(fnames2d[2])
        #ax.errorbar(nums, errs, yerr=SEMs)
        shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=xrevColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("x-reflection, CB only", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )
        ax.text(-0.1, 1.15, 'B', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        #addNoiseSz(ax)

     #   ax = axs[1,0]
     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)
     #   ax.set_xlabel("Movement Number", labelpad=8)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 
     #   ax.text(-0.1, 1.15, 'A2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')


        #ax_ = ax.twiny()
        #ax_.xaxis.grid(True,color='w')
        #ax.yaxis.grid(True)
        #ax.xaxis.grid(False)
        #ax.xaxis.grid(True, which='minor')

     #   ax = axs[1,1]
     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[1] ]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)
     #   ax.set_xlabel("Movement Number", labelpad=8)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 
     #   ax.text(-0.1, 1.15, 'B2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

#################################################################
#################################################################
#################################################################
#################################################################
 
    if pp.plotfname == 'difRotations_cbOnly':
        numcols=3
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth, wordSingleFigHeight), sharex=False, sharey=True)

        plt.subplots_adjust(left=0.06, right=0.90, bottom=0.08, top=0.86, wspace=0.1, hspace=0.4)

        ax=axs[0]
        makeReach(fig,ax,fnames2d[0][0])
        ax.set_title("rotation=60$^\circ$")#,y=1.03)

        ax=axs[1]
        makeReach(fig,ax,fnames2d[1][0])
        ax.set_title("rotation=75$^\circ$")

        ax=axs[2]
        makeReach(fig,ax,fnames2d[2][0])
        ax.set_title("rotation=85$^\circ$")

        addColorBar(fig,ax,vals=range(pp.phaseBegins[-1]),wd=0.03 )

        fig.suptitle("Adaptation dependence on rotation amplitude", size=1.2*globalFontSz,y=0.99)

#################################################################
#################################################################
#################################################################
#################################################################

    if pp.plotfname == 'mildPert_noCrit':
        numcols=1
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth / 1.9, wordSingleFigHeight), sharex=False, sharey=False)

        plt.subplots_adjust(left=0.2, right=0.96, bottom=0.17, top=0.84, wspace=0.4)

        ax=axs
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs,capsize=capsz)
        shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB, Shift", y=title_ycoord) 

        ymin = 50
        ymax = -20 
        ax.set_yticks(np.arange(-40,40,10))

        addTgtSz(ax)

        #ax.legend(loc='lower right')
 
    if pp.plotfname == 'strongPert_noCrit':
        ymin = -180
        ymax = 180 
        ystep = 30
        cbStateLim=6

        numcols=2
        numrows=1
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth, wordSingleFigHeight), sharex=False, sharey=False)

        plt.subplots_adjust(left=0.11, right=0.96, bottom=0.14, top=0.85, wspace=0.4, hspace=0.4)

        ax=axs[0]
        prep(fnames2d[0])
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs, capsize=capsz)
        shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=rotColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("rotation 90, BG and CB", y=title_ycoord) 
        ax.set_ylim(ylimSRot)
        ax.set_yticks(yticksSRot)

     #(_, caps, _) = errorbar(x, y, yerr=[y1,y2], elinewidth=2)
        #for cap in caps:
             #cap.set_markeredgewidth(2)
             #cap.set_capsize(2)

        addTgtSz(ax,False)
        ax.text(-0.17, 1.12, 'A', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        #ax.legend(loc='upper right')

        ax=axs[1]
        prep(fnames2d[1])
        #ax.errorbar(nums, errs, yerr=SEMs,capsize=capsz)
        shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=xrevColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB, x-reflection", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,ystep))

        addTgtSz(ax,False)
        ax.text(-0.17, 1.12, 'B', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        #ax.legend(loc='upper right')

    if pp.plotfname == 'strongPert_Crit':
        ymin = -30
        ymax = 100 
        ystep = 30
        cbStateLim=0.3
        cbLRateLim=6

        numcols=2
        numrows=2
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth, wordSingleFigHeight*2), sharex=True, sharey=False)
        plt.tight_layout()

        plt.subplots_adjust(left=0.11, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.20)

        ax=axs[0,0]
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs)
        shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=rotColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB with critic, rotation 90", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )

        addTgtSz(ax,False)
        ax.text(-0.1, 1, 'A1', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        ax.legend(loc='upper right')

        ax.set_xlabel("")

     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
     #   ax=axs[1,0]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,0.1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 

     #   ax.text(-0.1, 1.15, 'A2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[1,0]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,colorMean=rotColor)
        annotateCommon(ax)
        ax.set_ylabel(r"CB learning rate", labelpad=15)
        ax.set_ylim([0,cbLRateLim])
        ax.set_yticks(np.arange(0,cbLRateLim,2))
        ax.set_title("", y=title_ycoord) 

        ax.text(-0.1, 1, 'A2', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[0,1]
        prep(fnames2d[1])
        #ax.errorbar(nums, errs, yerr=SEMs)
        shadedErrorbar(ax,nums,-1.*errs,SEMs,colorMean=xrevColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB with critic, x-reflection", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )

        ax.text(-0.1, 1, 'B1', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        addTgtSz(ax,False)
        ax.legend(loc='upper right')
        ax.set_xlabel("")

     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[1] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[1] ]
     #   ax=axs[1,1]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,0.1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)

     #   ax.text(-0.1, 1.15, 'B2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[1,1]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,colorMean=xrevColor)
        annotateCommon(ax)
        ax.set_ylabel(r"CB learning rate", labelpad=15)
        ax.set_ylim([0,cbLRateLim])
        ax.set_yticks(np.arange(0,cbLRateLim,2))
        ax.set_title("", y=title_ycoord) 

        ax.text(-0.1, 1, 'B2', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

    if pp.plotfname == 'mildPert_Crit':
        ymin = -20
        ymax = 40 
        ystep = 10
        cbStateLim=0.3
        cbLRateLim=7

        numcols=1
        numrows=2
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth/1.9, wordSingleFigHeight*2), sharex=True, sharey=False)

        plt.subplots_adjust(left=0.18, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.3)

        ax=axs[0]
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs,color='black')
        shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor,lw=linew)

        annotateCommon(ax)
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )
        ax.set_title("BG and CB with critic, shift", y=title_ycoord) 
        ax.text(-0.1, 1, 'A', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')
        ax.set_xlabel("")

        addTgtSz(ax)

     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
     #   ax=axs[1]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,0.1))
     #   ax.set_ylabel(r"max of moduli of CB state entries",labelpad=15)
     #   ax.set_xlabel("")
     #   ax.set_title("")
     #   ax.text(-0.1, 1.15, 'B', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[1]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,colorMean=shiftColor)
        annotateCommon(ax)
        ax.set_ylim([0,cbLRateLim])
        ax.set_yticks(np.arange(0,cbLRateLim,2))
        ax.set_ylabel(r"CB learning rate", labelpad=15)
        ax.set_title("")
        ax.text(-0.1, 1, 'B', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

    if pp.plotfname == 'mildPert_CritcbOnly':
        ymin = -30
        ymax = 40 
        ystep = 10
        cbStateLim=0.3
        cbLRateLim=7

        numcols=2
        numrows=2
        fig, axs = plt.subplots(ncols=numcols, nrows=numrows, 
                figsize=(wordTextWidth, wordSingleFigHeight*2), sharex=True, sharey=False)
        plt.tight_layout()

        plt.subplots_adjust(left=0.11, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.20)

        ax=axs[0,0]
        prep(fnames2d[0])
        #ax.errorbar(nums, errs, yerr=SEMs)
        shadedErrorbar(ax,nums,errs,SEMs,colorMean=shiftColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB with critic, shift 30", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )

        addTgtSz(ax)
        ax.text(-0.1, 1, 'A1', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        ax.legend(loc='upper right')

        ax.set_xlabel("")

     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[0] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
     #   ax=axs[1,0]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,0.1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 

     #   ax.text(-0.1, 1.15, 'A2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[1,0]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,colorMean=shiftColor)
        annotateCommon(ax)
        ax.set_ylabel(r"CB learning rate", labelpad=15)
        ax.set_ylim([0,cbLRateLim])
        ax.set_yticks(np.arange(0,cbLRateLim,2))
        ax.set_title("", y=title_ycoord) 

        ax.text(-0.1, 1, 'A2', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[0,1]
        prep(fnames2d[1])
        #ax.errorbar(nums, errs, yerr=SEMs)
        shadedErrorbar(ax,nums,errs,SEMs,colorMean=rotColor,lw=linew)
        annotateCommon(ax)
        ax.set_title("BG and CB with critic, rotation 30", y=title_ycoord) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.append(np.arange(ymin,0,ystep),np.arange(0,ymax,ystep) ) )

        ax.text(-0.1, 1, 'B1', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

        addTgtSz(ax)
        ax.legend(loc='upper right')
        ax.set_xlabel("")

     #   fnamesCBState=[ fname.replace('arm','CBState') for fname in fnames2d[1] ]
        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[1] ]
     #   ax=axs[1,1]
     #   genCBStateMaxPlot(fig,ax,fnamesCBState,avg=True)
     #   annotateCommon(ax)
     #   ax.set_ylim([0,cbStateLim])
     #   ax.set_yticks(np.arange(0,cbStateLim,0.1))
     #   ax.xaxis.grid(False, which='major')
     #   ax.set_title("", y=title_ycoord) 
     #   ax.set_ylabel(r"max of moduli of CB state entries", labelpad=15)

     #   ax.text(-0.1, 1.15, 'B2', transform=ax.transAxes,
     # fontsize=16, fontweight='bold', va='top', ha='right')

        ax=axs[1,1]
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,colorMean=rotColor)
        annotateCommon(ax)
        ax.set_ylabel(r"CB learning rate", labelpad=15)
        ax.set_ylim([0,cbLRateLim])
        ax.set_yticks(np.arange(0,cbLRateLim,2))
        ax.set_title("", y=title_ycoord) 

        ax.text(-0.1, 1, 'B2', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

                                       
    pp.exportSvgEmf(plt)

#    from textwrap import wrap
#    ax.set_set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)
