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
    f=open(fname)
    line=f.next().strip()
    dirShift = float(line)
    line=f.next().strip()
    targetPre1 = float(line)
    f.close()
    armData = np.loadtxt(fname,skiprows=pp.armFileSkiprows)
    return armData,dirShift,targetPre1

def genReachPlot(fig,ax,xs,ys,nums):
    ax.set_xticks(np.arange(-0.5,0.5,0.1))
    ax.set_yticks(np.arange(0.0,0.9,0.1))

    ax.set_ylim([0,0.8])
    ax.set_xlim([-0.4,0.4])

    ax.scatter(xs[pp.trials1],ys[pp.trials1],c=nums[pp.trials1],lw=0.0,cmap='inferno',s=30)
    if( int(pp.paramsEnv.get("root","numPhases")) > 3 ):
        ax.scatter(xs[pp.trials2],ys[pp.trials2],c=nums[pp.trials2],lw=0.0,cmap='inferno',marker='^')


    #plt.scatter(xs, ys, color='r', marker='*', alpha=.4)
    #ax.scatter(x2, y2, color='b', s=s/2, alpha=.4)

    ax.grid()
    ax.set_title('Reaching points')

    if len(xs)<20:
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

    ax.legend(loc=1)
    rewardSpot1 = plt.Circle((0.2, 0.4), pp.rewardDist, color='b', fill=False)
    rewardSpot2 = plt.Circle((0.0, 0.6), pp.rewardDist, color='b', fill=False)

    learn_bg = int(pp.paramsML.get("root","learn_bg") )
    if(learn_bg>0):
        ax.add_artist(rewardSpot1)
        ax.add_artist(rewardSpot2)


    #cax = ax.imshow(nums, interpolation='nearest', cmap=cm.coolwarm)
    #fig = plt.gcf()
    ax1= fig.add_axes([0.92,0.04,0.03,0.9]);

    #im = imshow

    tickNum = 10
    mult = int(nums[-1]/10)
    #print np.where(nums%mult==0)
    #colorTicks = [nums[i*mult] for i in range(len(nums[-1])) ]
    colorTicks = [i for i in nums if i%mult == 0]  
    colorTicks.append(nums[-1])

    norm = mpl.colors.Normalize(vmin=nums[0], vmax=nums[-1])
    #cbar = fig.colorbar(cax=ax1,norm=norm,ticks=nums,orientation='vertical')
    cbar = mpl.colorbar.ColorbarBase(ax=ax1,norm=norm,ticks=colorTicks,orientation='vertical',cmap='inferno')
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

def genBGActivityPlot(ax,fname):
    activity = np.loadtxt(fname)
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
    ax.set_title('BG populations activity plot')
    #fig.savefig('auto.png')
    #forceAspect(ax,aspect=1)
    #fig.savefig('force.png')

def genBGWeightsPlot(ax,fname):
    genBGActivityPlot(ax,fname)
    ax.set_title('BG weights plot')
