#import plotly.plotly as py
#import plotly.graph_objs as go

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

def armFileRead(fname):
    armData = np.loadtxt(fname,skiprows=pp.armFileSkiprows)
    return armData

def genReachPlot(fig,ax,xs,ys,nums,title="",twoPhases=False,tgt=[],cbtgt=[],tgt_actual=[],cbtgt_actual=[]):

    yc = 0.4
    xlim = 0.6
    yadd = 0.7
    if "reachBoxXsz" in pp.paramsEnv:
        xlim = float(pp.paramsEnv["reachBoxXsz"])
    if "reachBoxYsz" in pp.paramsEnv:
        yadd = float(pp.paramsEnv["reachBoxYsz"])
    ylim = yadd + yc

    ax.set_ylim([0,ylim])
    ax.set_xlim([-xlim,xlim])
    ax.set_xticks(np.arange(-xlim,xlim,0.1))
    ax.set_yticks(np.arange(0.0,ylim,0.1))

    if(twoPhases and int(pp.paramsEnv["numPhases"]) > 3 ):
        ax.scatter(xs[pp.trials1],ys[pp.trials1],c=nums[pp.trials1],lw=0.0,cmap='inferno',s=45)
        ax.scatter(xs[pp.trials2],ys[pp.trials2],c=nums[pp.trials2],lw=0.0,cmap='inferno',marker='^',s=45)
    else:
        #print zip(nums,xs,ys)
        ax.scatter(xs,ys,c=range(len(nums)),lw=0.0,cmap='inferno',s=45)


    #plt.scatter(xs, ys, color='r', marker='*', alpha=.4)
    #ax.scatter(x2, y2, color='b', s=s/2, alpha=.4)

    ax.grid()
    ax.set_title('Reaching points '+title)

    lastx=0
    lasty=0
    if len(xs)<=30:
        for i, x, y in zip(nums, xs, ys):
            if (math.sqrt( (lastx-x)**2 + (lasty-y)**2 ) > 0.03):
                ax.annotate(int(i), (x,y))
            lastx = x
            lasty = y

    addxs = [0.]
    addys = [0.4]
    #addnum = [0]
    addlabel = ["center"]

    learn_cb = int(pp.paramsEnv["learn_cb"] )

    if(len(cbtgt) > 0 and learn_cb):
        addxs.append(cbtgt[0][0])
        addys.append(cbtgt[0][1])
        addlabel.append("CBtgt_p")
    if(len(cbtgt_actual) > 0 and learn_cb):
        addxs.append(cbtgt_actual[0][0])
        addys.append(cbtgt_actual[0][1])
        addlabel.append("CBtgt_a")

    # todo make additional labels for targets
    for label, x, y in zip(addlabel, addxs, addys):
        ax.annotate(
            label,
            xy=(x, y), xytext=(0, -270),
            textcoords='offset points', ha='right', va='bottom',
            bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
            arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

    xc = 0
    yc = 0.4
         
    tgtRot = 0
    tgtRev = 0
    if "target_rotation1" in pp.paramsEnv:
        tgtRot =  int(pp.paramsEnv["target_rotation1"] ) 
    if "target_xreverse1" in pp.paramsEnv:
        tgtRev =  int(pp.paramsEnv["target_xreverse1"] ) 
    tgt1_defined=False
    tgt2_defined=False

    if(len(tgt) == 0):
        arrad = float(pp.paramsEnv["armReachRadius"])
        if "defTgt0" in pp.paramsEnv:
            tgtPre1 = float(pp.paramsEnv["defTgt0"])
        else:
            tgtPre1 = 0
        tgtPre1 = (tgtPre1 / 360.) * 2 * math.pi
        xr1 = xc + arrad*math.cos(tgtPre1)
        yr1 = yc + arrad*math.sin(tgtPre1)
        tgt1_defined = True

        if(tgtRot):
            shiftedTgt =  tgtPre1 + ( pp.dirShift/360.) * 2 * math.pi
            xr2 = xc + arrad*math.cos(shiftedTgt)
            yr2 = yc + arrad*math.sin(shiftedTgt)
            tgt2_defined = True
        if(tgtRev):
            xr2 = xc - arrad*math.cos(tgtPre1)
            yr2 = yc + arrad*math.sin(tgtPre1)
            tgt2_defined = True
    else:
        xr1 = tgt[0][0]
        yr1 = tgt[0][1]
        tgt1_defined = True
        if(len(tgt_actual)>0):
            tgt2_defined = True
            xr2 = tgt_actual[0][0]
            yr2 = tgt_actual[0][1]

    if(tgt1_defined):
        rewardSpot1 = plt.Circle((xr1, yr1), pp.rewardDist, color='b', fill=False)
    if(tgt2_defined):
        rewardSpot2 = plt.Circle((xr2, yr2), pp.rewardDist, color='r', fill=False)

    #learn_bg = int(pp.paramsEnv["learn_bg"] )
    if(tgt1_defined):
        ax.add_artist(rewardSpot1)
    if(tgt2_defined):
        ax.add_artist(rewardSpot2)
#        ax.add_artist(rewardSpot2)


    addColorBar(fig,ax,vals=nums)
    #cax = ax.imshow(nums, interpolation='nearest', cmap=cm.coolwarm)
    #fig = plt.gcf()


    #cbar.ax.set_yticklabels(nums)  # vertically oriented colorbar

    #cb2 = mpl.colorbar.ColorbarBase(ax2, cmap=cmap,
    #                                norm=norm,
    #                                # to use 'extend', you must
    #                                # specify two extra boundaries:
    #                                boundaries=[0] + bounds + [13],
    #                                extend='both',
    #                                ticks=bounds,  # optional
    #                                spacing='proportional',
    #                                orientation='horizontal')
    #cb2.set_label('Discrete intervals, some other units')

    #plt.show()

def addColorBar(fig,ax_,vals=0,tickSkip=10,dat=0,wd=0.01):

    pos1 = ax_.get_position() # get the original position 
    #pos1new = [pos1.x0, pos1.y0,  pos1.width, pos1.height ] 
    #ax_.set_position(pos1new)
    pos1 = ax_.get_position() # get the original position 
    pos2 = [pos1.x0 + pos1.width+wd/2, pos1.y0,  wd, pos1.height ] 

    ax1= fig.add_axes(pos2); # from left, from down, width, height

    #im = imshow

    if dat==0:
        mult = max(1,int((vals[-1]+1)/tickSkip) )
        colorTicks = [i for i in vals if i%mult == 0]  
        #colorTicks.append(vals[-1])

        norm = mpl.colors.Normalize(vmin=vals[0], vmax=vals[-1])
    #cbar = fig.colorbar(cax=ax1,norm=norm,ticks=nums,orientation='vertical')
        cbar = mpl.colorbar.ColorbarBase(ax=ax1,norm=norm,ticks=colorTicks,orientation='vertical',cmap='inferno')
    else:
        cbar = fig.colorbar(dat,cax=ax1,orientation='vertical',cmap='inferno')

def genBGActivityPlot(fig,ax,fname,cols=range(0,300)):
    activity = np.loadtxt(fname)
    activity = activity[:,cols]
    (nrows, ncols) = activity.shape

    #data = [go.Heatmap( z=activity.transpose(), x=range(nrows) )]
    #py.iplot(data, filename='heatmap')

    #ax.imshow(activity.transpose(), cmap='inferno', interpolation='nearest')
    pcol = ax.pcolor(activity.transpose(), cmap='inferno',lw=0,rasterized=True)
    pcol.set_edgecolor('face')
    #ax.set_aspect('equal')
    #ax.set_aspect(2)
    #fig.savefig('equal.png')
    ax.set_aspect('auto')
    ax.grid(False)
    ax.set_title('BG populations activity plot',y=1.04)
    #fig.savefig('auto.png')
    #forceAspect(ax,aspect=1)
    #fig.savefig('force.png')
    ax.yaxis.grid(True,color='w')
    ax.set_yticks(range(0,301,100))
    #ax.set_ylabel('y   d1   d2   gpe   gpi',rotation=90)
    ax.set_ylabel('y   d1   d2',rotation=90)


#[8, 4, 2, 4, 2, 4] means
#
#    8 points on, (dash)
#    4 points off,
#    2 points on, (dot)
#    4 points off,
#    2 points on, (dot)
#    4 points off.

    addColorBar(fig,ax_=ax,dat=pcol,wd=0.01)

def genBGWeightsPlot(fig,ax,fname,cue=0):
    # 300 -- w1 + w2 + wm
    pcol = genBGActivityPlot(fig,ax,fname,range(cue*300,(cue+1)*300))
    ax.set_title('BG weights plot, cue='+str(cue),y=1.04)
    ax.set_yticks(range(0,301,100))
    ax.set_ylabel('w1    w2    wm',rotation=90)

def genCBStatePlot(fig,ax,fname):
    # 300 -- w1 + w2 + wm
    pcol = genBGActivityPlot(fig,ax,fname,range(1,6*6+1) )
    ax.set_title('CB state plot',y=1.04)
    ax.set_yticks(range(0,36))
    ax.set_ylabel('wcb[0 1 2 3 4 5][*]',rotation=90)
    ax.set_ylim(0,36)

def genCBTuningPlot(fig,ax,fname):
    # 300 -- w1 + w2 + wm
    pcol = genBGActivityPlot(fig,ax,fname,range(1,6*6*2+1) )
    ax.set_title('CB Tuning plot',y=1.04)
    ax.set_yticks(range(0,6*6*2,6))
    ax.set_ylabel('dfwx dfwy',rotation=90)
    ax.set_ylim(0,36*2)

def genCBMiscPlot(fig,ax,fname):
    errMult = 7.
    #errMultSmall = 5.

    if "cbMiscErrMult" in pp.paramsEnv:
        errMult = float(pp.paramsEnv["cbMiscErrMult"])

    misc = np.loadtxt(fname)
    rates = misc[:,0]
    errAbsLarge = errMult * misc[:,1]
    #errAbsSmall = errMultSmall * misc[:,1]
    ratios = misc[:,2]
    #prevErrAbs = errMult * misc[:,3]
    #(nrows, ncols) = misc.shape

    ax.plot(rates,label='rate')
    ax.plot(errAbsLarge,label=str(errMult)+'*cur_errAbs')
    ax.plot(ratios,label='ratio')
    #ax.plot(prevErrAbs,label=str(errMult)+'*prevErrAbs')
    #ax.plot(errAbsSmall,label=str(errMultSmall)+'*cur_errAbs')
    pos = ax.get_position()
    ax.legend(loc='lower left')
    
    mx = float(pp.paramsEnv["cbLRate"])
    mux =float(pp.paramsEnv["cbLRateUpdSpdMax"])  
    cbUpdDst =float(pp.paramsEnv["updateCBStateDist"])  

    ylmax =  cbUpdDst/0.01 #1.8*mx
    if "cbMiscGraph_y_axis_max" in pp.paramsEnv:
        ylmax = float(pp.paramsEnv["cbMiscGraph_y_axis_max"] )
    ylmin = -mux
    ax.set_ylim(ylmin,ylmax)
    ax.set_yticks(np.arange(ylmin,ylmax,1.))
    #legend = ax.legend(loc=(pos.x0+pos.width/2,pos.y0-20), shadow=True)
