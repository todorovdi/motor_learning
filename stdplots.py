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

def genReachPlot(fig,ax,xs,ys,nums,title="",twoPhases=False):
    ax.set_xticks(np.arange(-0.5,0.5,0.1))
    ax.set_yticks(np.arange(0.0,0.9,0.1))

    ax.set_ylim([0,0.8])
    ax.set_xlim([-0.4,0.4])

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

    if len(xs)<=30:
        for i, x, y in zip(nums, xs, ys):
            ax.annotate(int(i), (x,y))

    addxs = [0.]
    addys = [0.4]
    addnum = [0]
    addlabel = ["center"]

    # todo make additional labels for targets
    for label, x, y in zip(addlabel, addxs, addys):
        ax.annotate(
            label,
            xy=(x, y), xytext=(-20, 20),
            textcoords='offset points', ha='right', va='bottom',
            bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
            arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

    xc = 0
    yc = 0.4

    import math

    arrad = float(pp.paramsEnv["armReachRadius"])
    tgtPre1 = float(pp.paramsEnv["targetPre1"])
    xr1 = xc + arrad*math.cos(tgtPre1 )
    yr1 = yc + arrad*math.sin(tgtPre1)


    tgtRot =  int(pp.paramsEnv["target_rotation1"] ) 
    tgtRev =  int(pp.paramsEnv["target_xreverse1"] ) 

    if(tgtRot):
        xr2 = xc + arrad*math.cos(tgtPre1 + pp.dirShift)
        yr2 = yc + arrad*math.sin(tgtPre1 + pp.dirShift)
    if(tgtRev):
        xr2 = xc - arrad*math.cos(tgtPre1)
        yr2 = yc + arrad*math.sin(tgtPre1)

    rewardSpot1 = plt.Circle((xr1, yr1), pp.rewardDist, color='b', fill=False)
    if(tgtRot or tgtRev):
        rewardSpot2 = plt.Circle((xr2, yr2), pp.rewardDist, color='r', fill=False)

    learn_bg = int(pp.paramsEnv["learn_bg"] )
    if(learn_bg>0):
        ax.add_artist(rewardSpot1)
        if(tgtRot or tgtRev):
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
    ax.set_xlim([0,pp.trials1End])
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
