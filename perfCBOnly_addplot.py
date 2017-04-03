import matplotlib.pyplot as plt
import plotparams as pp
from stdplots import *
import universal_plot as up
import stdplots as stdp
import re
import numpy as np
import matplotlib.pyplot as plt

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
    errs = (-1.)*errs
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

    ymin = -0.4
    ymax = 0.5 
    step = 0.1
    #ymin = -30
    #ymax = 50 
    #step = 10
    ax.set_ylim([ymin,ymax])
    ax.set_yticks(np.arange(ymin,ymax,step))

    ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
    ax.xaxis.grid(True, which='minor')

def makePubPlot(fnames2d):
    # shiny plotting code goes here
    fig, axs = plt.subplots(ncols=3, nrows=2, figsize=(30, 20), sharex=True, sharey=True)

    fnames_force_field = fnames2d[0]
    fnames_percept_rot = fnames2d[1]
    fnames_percept_xrev = fnames2d[2]
    fnames_percept_small_rot = fnames2d[3]

#    for i,f in enumerate(fnames2d):
#        print i,len(f)

    msz = 12
    xlegendloc = 0.0
    ylegendloc = 1.0
    legendloc = 'upper left'

    # plot errors here
    ax = axs[1,1]
    prep(fnames_nonEB_control)
    ax.errorbar(nums, errs, yerr=SEMs, marker='d', color=fireblack,markersize=msz,label='CPD')
    prep(fnames_nonEB_PD)
    ax.errorbar(nums, errs, yerr=SEMs, marker='^', color=lightcoral,markersize=msz,label='PD')
    annotateCommon(ax)

    #plot reaching points here

    ax.legend(bbox_to_anchor=(xlegendloc, ylegendloc), loc=legendloc, borderaxespad=0.)
