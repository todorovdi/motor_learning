#import plotly.plotly as py
#import plotly.graph_objs as go

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
from matplotlib import cm
from scipy import stats

import plotparams as pp

def coord2AngleDeg(x,y):    # returns -180 to 180
    xc = float(pp.paramsEnv["x_center"])
    yc = float(pp.paramsEnv["y_center"])
    baseAng = 0.  # relative to EAST direction

    xd = x-xc
    yd = y-yc

    if(xd >= 0):
        angleDeg = ( math.atan( yd / xd )  ) / (2*math.pi) * 360.
#        if(yd>0):
#            angleDeg = 360 - angleDeg
    else:
        angleDeg = ( math.atan( yd / (-xd) )   ) / (2*math.pi) * 360.
        if yd >= 0:
            angleDeg = math.pi/2 + (math.pi/2-angleDeg)
        else:
            angleDeg = -math.pi/2 - (math.pi/2+angleDeg)

    return angleDeg - baseAng

# returns a1-a2
def angleDegDif(a1,a2):    # suppose 0 <= a1,a2 < 360
    r = a1 - a2
    if (fabs(r) > 180):
        if a1 < 180:
            r = a1 + (180*2 - a2)
        else:
            r = -angleDegDif(a2,a1)
    return r

def getReachAngles(armData):
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
        angleDeg = coord2AngleDeg(x,y)
    #print int(j),x,y,xt,yt, d
        angs[int(j)] = angleDeg
    return angs

def getErrs(armData):
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    xs_target = armData[:,3]
    ys_target = armData[:,4]
    n = len(xs)

    errs = np.zeros(n)
    for (x,y,xt,yt,j) in zip(xs,ys,xs_target,ys_target,nums):
        if(pp.plotAngleErrs == 0):
            if(pp.signed_dist_err):
                d =  -(x-xt)
            else:
                d = math.sqrt( (x-xt)**2 + (y-yt)**2 )
        else:
            tgtAngleDeg = coord2AngleDeg(xt,yt) 
            angleCurDeg = coord2AngleDeg(x,y) 
            dif = angleCurDeg - tgtAngleDeg;
            dif1 = dif
            if(dif > 180.):
                dif1 = dif-360.
            d = dif1
        #print int(j),x,y,xt,yt, d
        errs[int(j)] = d
    return errs

def doStats(fnames):
    n = pp.phaseBegins[-1] 
    nsess = len(fnames)
    errs = np.zeros(shape=(nsess,n))
    for i,fname in enumerate(fnames):
        armData = np.loadtxt(fnames[i])
        if pp.plotReachAngles != 0 :
            dat = getReachAngles(armData)
        else:
            dat = getErrs(armData) 
        if len(dat) != n:
            print("------- doStats Warning: wrong length of data file table, maybe calc was terminated too early")
        else:
            errs[i,:] = dat * pp.datMult
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

def armFileRead(fname):
    armData = np.loadtxt(fname)#,skiprows=pp.armFileSkiprows)
    return armData

def genReachPlot(fig,ax,xs,ys,nums,title="",twoPhases=False,tgt=[],cbtgt=[],tgt_actual=[],cbtgt_actual=[]):

    yc = 0.4
    xlim = pp.reachBoxXsz
    yadd = pp.reachBoxYsz
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
    if len(xs)<=30 or pp.showPointNumbers:
        for i, x, y in zip(nums, xs, ys):
            if (math.sqrt( (lastx-x)**2 + (lasty-y)**2 ) > 0.03):
                ax.annotate(int(i), (x,y), fontsize=7)
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
        tgtRot =  float(pp.paramsEnv["target_rotation1"] ) 
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

        dirShift = 0
        if(len(pp.phaseBegins) > 1):  # if using new version of ini files
            dirShift = tgtRot
        else:
            dirShift = pp.dirShift
        if(math.fabs(tgtRot) > 0.0001 ):
            shiftedTgt =  tgtPre1 + ( dirShift/360.) * 2 * math.pi
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

    if dat==0:
        mult = max(1,int((vals[-1]+1)/tickSkip) )
        colorTicks = [i for i in vals if i%mult == 0]  

        norm = mpl.colors.Normalize(vmin=vals[0], vmax=vals[-1])
        cbar = mpl.colorbar.ColorbarBase(ax=ax1,norm=norm,ticks=colorTicks,orientation='vertical',cmap='inferno')
    else:
        cbar = fig.colorbar(dat,cax=ax1,orientation='vertical',cmap='inferno')

def genBGActivityPlot(fig,ax,fname,cols=range(0,300)):
    activity = np.loadtxt(fname)
    activity = activity[:,cols]
    (nrows, ncols) = activity.shape

    pcol = ax.pcolor(activity.transpose(), cmap='inferno',lw=0,rasterized=True)
    pcol.set_edgecolor('face')
    ax.set_aspect('auto')
    ax.grid(False)
    ax.set_title('BG populations activity plot',y=1.04)
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

def genCBStateMaxPlot(fig,ax,fname):
    data = np.loadtxt(fname)
    state = data[:,range(1,6*6+1)]
    (nrows, ncols) = state.shape

    maxs = []
    for r in range(nrows):
        s = state[r,:]
        maxs.append(  math.fabs( max(s.min(), s.max(), key=abs) )   )

    ax.plot(maxs)
    
    ax.xaxis.grid(True)

    ax.set_title('CB state max',y=1.04)
    #ax.set_yticks(range(0,6*6*2,6))
    ax.set_ylabel('abs max',rotation=90)
    ax.set_ylim(0,pp.cbStateMax)

def genCBTuningPlot(fig,ax,fname):
    # 300 -- w1 + w2 + wm
    pcol = genBGActivityPlot(fig,ax,fname,range(1,6*6*2+1) )
    ax.set_title('CB Tuning plot',y=1.04)
    ax.set_yticks(range(0,6*6*2,6))
    ax.set_ylabel('dfwx dfwy',rotation=90)
    ax.set_ylim(0,36*2)

def genCBMiscPlot(fig,ax,fname):
    #errMultSmall = 5.

    errMult = pp.cbMiscErrMult

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

    ylmax = pp.cbMiscGraph_y_axis_max
    ylmin = -mux
    ax.set_ylim(ylmin,ylmax)
    ax.set_yticks(np.arange(ylmin,ylmax,1.))
    #legend = ax.legend(loc=(pos.x0+pos.width/2,pos.y0-20), shadow=True)
    ax.set_title('CB misc plot',y=1.04)

    ax.xaxis.grid(True, which='minor')

def genRwdPlot(fig,ax,fname):
    misc = np.loadtxt(fname)
    rwd = misc[:,1]
    rpre = misc[:,2]

    ax.plot(rwd,label='rwd',color='blue')
    ax.plot(rpre,label='Rpre',color='red')

    ax.legend(loc='upper right')
    
    ax.set_title('Reward plot',y=1.04)
    
    mux =float(pp.paramsEnv["cbLRateUpdSpdMax"])  
    rsz = float(pp.paramsEnv["rewardSize"])
    ylmax = 4.
    ylmin = 0
    ax.set_ylim(ylmin,ylmax)
    ax.set_yticks(np.arange(ylmin,ylmax,1.))
    #legend = ax.legend(loc=(pos.x0+pos.width/2,pos.y0-20), shadow=True)

    ax.xaxis.grid(True)
