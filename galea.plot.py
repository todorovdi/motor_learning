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
import ConfigParser
import io
import StringIO
import math

from matplotlib import cm
from scipy import stats

def paramFileRead(fname):
    ini_str = '[root]\n' + open(fname, 'r').read()
    ini_fp = StringIO.StringIO(ini_str)
    params = ConfigParser.RawConfigParser(allow_no_value=True)
    params.readfp(ini_fp)
    return params

def armFileRead(fname):
    f=open(fname)
    line=f.next().strip()
    dirShift = float(line)
    line=f.next().strip()
    targetPre1 = float(line)
    f.close()
    armData = np.loadtxt(fname,skiprows=2)
    return armData,dirShift,targetPre1

def getErrs(armData):
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    xs_target = armData[:,3]
    ys_target = armData[:,4]
    n = len(xs)

    errs = np.zeros(n)
    for (x,y,xt,yt,j) in zip(xs,ys,xs_target,ys_target,nums):
        d = math.sqrt( (x-xt)**2 + (y-yt)**2 )
        errs[int(j)] = d
    return errs

def doStats(fnames):
    nsess = len(fnames)
    errs = np.zeros(shape=(nsess,n))
    for i,fname in enumerate(fnames):
        armData = np.loadtxt(fnames[i],skiprows=2)
        errs[i,:] = getErrs(armData)
    #print math.isnan(t)
    means = np.zeros(n)
    SEMs = np.zeros(n)
    for i in range(n):
        means[i] = np.mean(errs[:,i])
        SEMs[i] = stats.sem(errs[:,i])
        
    print max(SEMs)
    ax.errorbar(nums, means, yerr=SEMs)


def genFigure(fname):
    armData,dirShift,targetPre1 = armFileRead(fname)
    #armData = np.loadtxt(fnames[0],skiprows=2)
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    n = len(xs)
    lastNum = nums[-1]

    fig, axs = plt.subplots(ncols=2, figsize=(20, 10), sharex=False, sharey=False)

    ax = axs[1]

    errs = getErrs(armData)
    #ax.errorbar(nums, means, yerr=SEMs)
    ax.plot(nums, errs)
    ax.set_title('Errors averaged and SEMs',y=1.04)
    ax.set_xticks(np.arange(0,n,10))
    ax.set_yticks(np.arange(0.0,0.9,0.1))
    ax.set_ylim([0,0.5])
    ax.set_xlim([0,n])
    ax.grid()

    ax_ = ax.twiny()
    #ax_.set_xlabel("x-transformed")
    ax_.set_xlim(0, n)
    ax_.set_xticks([numTrialsPre, numTrialsPre+numTrialsAdapt])
    ax_.set_xticklabels(['ADAPT','POST'])


    ax = axs[0]
    #ax = plt.gca()
    ax.set_xticks(np.arange(-0.5,0.5,0.1))
    ax.set_yticks(np.arange(0.0,0.9,0.1))

    ax.set_ylim([0,0.8])
    ax.set_xlim([-0.4,0.4])

    ax.scatter(xs[trials1],ys[trials1],c=nums[trials1],lw=0.0,cmap='jet')
    ax.scatter(xs[trials2],ys[trials2],c=nums[trials2],lw=0.0,cmap='jet',marker='^')


    #plt.scatter(xs, ys, color='r', marker='*', alpha=.4)
    #ax.scatter(x2, y2, color='b', s=s/2, alpha=.4)

    ax.grid()

    if lastNum<60:
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
    rewardSpot1 = plt.Circle((0.2, 0.4), rewardDist, color='b', fill=False)
    rewardSpot2 = plt.Circle((0.0, 0.6), rewardDist, color='b', fill=False)

    learn_bg = int(paramsML.get("root","learn_bg") )
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
    cbar = mpl.colorbar.ColorbarBase(ax=ax1,norm=norm,ticks=colorTicks,orientation='vertical',cmap='jet')
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

    plt.savefig(fname+".pdf")
    plt.close(fig)
    #plt.clf()
    #plt.cla()
    return armData,dirShift,targetPre1,errs

    #labels = ['point{0}'.format(i) for i in range(N)]

def getAdaptEnds(armData):
    errs[numTrialsPre+numTrialsAdapt-1]
    return 0

paramsEnv = paramFileRead('env.ini')

rewardDist = float(paramsEnv.get("root","rewardDist") )
dir = paramsEnv.get("root","output_dir")
iniML = paramsEnv.get("root","iniML")
iniBG = paramsEnv.get("root","iniBG")
iniCB = paramsEnv.get("root","iniCB")
iniArm = paramsEnv.get("root","iniArm")

paramsML = paramFileRead(iniML)

targetPre1_def = float(paramsEnv.get("root","targetPre1"))
targetPre2 = float(paramsEnv.get("root","targetPre2"))
dirShift_def = float(paramsEnv.get("root","dirShift"))
numPhases = int(paramsEnv.get("root","numPhases"))
numTrialsPre = int(paramsEnv.get("root","numTrialsPre"))
numTrialsAdapt = int(paramsEnv.get("root","numTrialsAdapt"))
numTrialsPost = int(paramsEnv.get("root","numTrialsPost"))

trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
trials1 = range(trials1End)
trials2 = range(trials1End,trials1End*2)

fnames = []
for filename in os.listdir(dir):
    if fnmatch.fnmatch(filename, '*_arm*.dat'):
        fnames.append(dir+filename)


dirShifts=[]
ends=[]
for fname in fnames:
    armData,dirShift,targetPre1,errs = genFigure(fname)
    #
    armData,dirShift,targetPre1 = armFileRead(fname)
    errs = getErrs(armData)
    dirShifts.append(int(dirShift))
    ends.append(errs[numTrialsPre+numTrialsAdapt-1] )

fig, axs = plt.subplots(ncols=2, figsize=(20, 10), sharex=False, sharey=False)

fig=plt.figure()
plt.plot(dirShifts,ends,'ro')
plt.title("targetPre1 = "+ str(targetPre1))
plt.savefig(dir+"dirShift2error_target="+str(targetPre1)+".pdf")

plt.clf()
plt.cla()

#print fnames

#ax.plot(nums, means)




#xs = xs + addxs
#ys = ys + addys
#labels = labels + addlabel
#nums = nums + addnum

#xs = np.concatenate((xs,addxs),0)
#ys =  np.concatenate((ys,addys),0)
#nums = np.concatenate((nums,addnum),0)

#plt.plot(xs,ys,color="red",linestyle="",label="#1")


