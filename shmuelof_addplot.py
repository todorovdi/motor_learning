import matplotlib.pyplot as plt
import plotparams as pp
from stdplots import *
import re

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

def makePubPlot(fnames2d):
    # shiny plotting code goes here
    fig = plt.gcf()
    ax = plt.gca()

    print("-----shiny plotting code is exectuded here")

    for fnames in fnames2d:
        filename = fnames[0] 
        ree = '(.*)_\w+\.dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)

        angs,SEMs = doStats(fnames)

        fileToPlot = fnames[0]
        armData = armFileRead(fileToPlot)
        #armData = np.loadtxt(fnames[0])
        nums = armData[:,0]
        nums = nums.astype(np.int)

        if(len(fnames) == 1):
            print("Need more filenames")
            ax.plot(nums, angs)
        else:
            ax.errorbar(nums, angs, yerr=SEMs)

        n = pp.phaseBegins[-1]
        ax.set_xticks(range(n)[::pp.xtickSkip] )
        ax.set_xlim([0,n])
        ax_ = ax.twiny()
        ax_.set_xlim([0, n])
        ax_.set_xticks(pp.phaseBegins[1:-1])
        ax_.set_xticklabels(pp.phaseNames)
        ax_.xaxis.grid(True,color='w')

        ax.yaxis.grid(True)

        ymin = 0.
        ymax = pp.y_axis_max 
        if(pp.y_axis_signed): 
            ymin = -ymax
        step = (ymax-ymin) / 10.
        if "y_axis_step" in pp.paramsEnv:
            step = (float(pp.paramsEnv["y_axis_step"]) ) 
        ax.set_ylim([ymin,ymax])
        ax.set_yticks(np.arange(ymin,ymax,step))

        ax.set_xticks(pp.phaseBegins[1:-1],minor=True)
        ax.xaxis.grid(True, which='minor')

        from textwrap import wrap
        ax.set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)
