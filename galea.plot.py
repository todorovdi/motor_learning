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

ini_str = '[root]\n' + open('env.ini', 'r').read()
ini_fp = StringIO.StringIO(ini_str)
config = ConfigParser.RawConfigParser(allow_no_value=True)
config.readfp(ini_fp)

rewardDist = float(config.get("root","rewardDist") )
dir = config.get("root","output_dir")


fnames = []
for filename in os.listdir(dir):
    if fnmatch.fnmatch(filename, '_arm*.dat'):
        fnames.append(dir+filename)

print fnames

armData = np.loadtxt(fnames[0])
nums = armData[:,0]
xs = armData[:,1]
ys = armData[:,2]
n = len(xs)


nsess = len(fnames)
errs = np.zeros(shape=(nsess,n))
for i,fname in enumerate(fnames):
    armData = np.loadtxt(fnames[i])
    nums = armData[:,0]
    xs = armData[:,1]
    ys = armData[:,2]
    xs_target = armData[:,3]
    ys_target = armData[:,4]
    n = len(xs)

    for (x,y,xt,yt,j) in zip(xs,ys,xs_target,ys_target,nums):
        d = math.sqrt( (x-xt)**2 + (y-yt)**2 )
        errs[i,int(j)] = d

#print math.isnan(t)

means = np.zeros(n)
SEMs = np.zeros(n)
for i in range(n):
    means[i] = np.mean(errs[:,i])
    SEMs[i] = stats.sem(errs[:,i])
    
print max(SEMs)

addxs = [0.]
addys = [0.4]
addlabel = ["center"]
addnum = [0]

lastNum = nums[-1]

#xs = xs + addxs
#ys = ys + addys
#labels = labels + addlabel
nums = nums + addnum

#xs = np.concatenate((xs,addxs),0)
#ys =  np.concatenate((ys,addys),0)
#nums = np.concatenate((nums,addnum),0)

#plt.plot(xs,ys,color="red",linestyle="",label="#1")

fig, axs = plt.subplots(ncols=2, figsize=(20, 10), sharex=False, sharey=False)

ax = axs[1]
ax.errorbar(nums, means, yerr=SEMs)
#ax.plot(nums, means)
ax.set_title('Errors averaged and SEMs')
ax.set_xticks(np.arange(0,n,10))
ax.set_yticks(np.arange(0.0,0.9,0.1))
ax.set_ylim([0,0.5])
ax.set_xlim([0,n])


ax = axs[0]
#ax = plt.gca()
ax.set_xticks(np.arange(-0.3,0.3,0.1))
ax.set_yticks(np.arange(0.0,0.9,0.1))

ax.set_ylim([0,0.8])
ax.set_xlim([-0.3,0.3])

ax.scatter(xs,ys,c=nums,lw=0.0,cmap='jet')

#plt.scatter(xs, ys, color='r', marker='*', alpha=.4)
#ax.scatter(x2, y2, color='b', s=s/2, alpha=.4)

ax.grid()

if lastNum<60:
    for i, x, y in zip(nums, xs, ys):
        ax.annotate(int(i), (x,y))

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

learn_bg = int(config.get("root","learn_bg") )
if(learn_bg>0)
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

plt.savefig(dir+"galea.pdf")

#labels = ['point{0}'.format(i) for i in range(N)]

