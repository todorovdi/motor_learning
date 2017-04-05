import matplotlib.pyplot as plt
import plotparams as pp
from stdplots_robbie import *
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

    fig = plt.figure()
    ax = plt.gca()

    print("-----shiny plotting code is executed here :^)")

    ii = 0
    handles = list()
    labels = list()

    for fnames in fnames2d:
        filename = fnames[0]

        ree = '(.*)_\w+\.dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)

        # Output file name
        outname = filename.split('/')[-1]
        outname = os.path.join('./output_for_paper', outname)
        # print("\n\nPLOTTING: ", outname)

        PEstr = str(pp.paramsEnv)
        BEonly = PEstr.find("learn_cb2=0") # check for BE only flag
        EAsymp = PEstr.find("numTrials4=100") # check for extended asymp. flag
        
        # Default plotting params
        color = [0, 0, 1, 1.0]
        label = 'BE + VE'
        eopacity = 0.25

        # Plotting params for BE
        if BEonly != -1:
            color = [1, 0, 0, 1.0]
            label = 'BE'

        printlabel = label

        # Plotting params for EAsymp
        if EAsymp != -1:
            label = ''
            color[-1] = 0.5
            eopacity = 0.15
            printlabel = printlabel + " EAsymp"

        print("\n#################\n\nPLOTTING: ", printlabel,"\n\n#############\n")

        # Calculate angles and SEMs
        angs,SEMs = doStats(fnames)

        fileToPlot = fnames[0]
        armData = armFileRead(fileToPlot)
        nums = armData[:,0]
        nums = nums.astype(np.int)

        # PLOT LINES AND ERROR FILL
        ax.fill_between(nums, angs-SEMs, angs+SEMs, facecolor=(0.3, 0.3, 0.3, eopacity))
        angs_handle, = ax.plot(nums, angs, color=color, lw=3.0, label=label)
        if label != '':
            handles.append(angs_handle)
            labels.append(label)

        # SET XTICKS
        xmin = 0
        xmax = 350
        xstep = 50

        ax.set_xlim([xmin,xmax])
        xticks = np.arange(xmin,xmax,xstep)
        ax.set_xticks(xticks)
        ax.set_xticklabels(xticks)

        # SET YTICKS
        ymin = -50
        ymax = 10
        step = 10
        
        ax.set_ylim([ymin,ymax])
        yticks = np.arange(ymin,ymax,step)
        ax.set_yticks(yticks)
        ax.set_yticklabels(yticks)

        ax.tick_params(axis='both', which='both', size=26)

        # LABELS
        ax.set_xlabel("Movement Number", size=25,labelpad=15)
        ax.set_ylabel(r"Hand Direction $(^\circ)$", size=25,labelpad=15)

        # Phase starts
        phases = [20., 80., 160., 190., 250., 290.]
        
        n = phases[-1] # Last phase, x value

        if ii == 1:
            ax_ = plt.twiny(ax)
            ax_.set_xlim([0, 350])
            ax_.set_xticks(phases)
            ax_.set_xticklabels(pp.phaseNames + ["WASHOUT 2"])
            # ax_.xaxis.set(ticks=[int(ph+15) for ph in phases[:-1]], ticklabels=pp.phaseNames) # to offset like in the paper
            ax_.tick_params(axis='both', which='both', size=26)

            ax_.vlines(phases, ymin=-50, ymax=10, linestyles='dotted', color='k') # plot phase starting points

            # Plot target locations
            rot1 = -30.0
            xmin1, xmax1 = 0, phases[2]
            ax_.plot(np.arange(xmin1, xmax1), [rot1]*int(xmax1-xmin1), lw=2.0, zorder=2, color='k') # target center
            ax_.plot(np.arange(xmin1, xmax1), [rot1+10]*int(xmax1-xmin1), lw=2.0, zorder=2, color='grey') # target top
            ax_.plot(np.arange(xmin1, xmax1), [rot1-10]*int(xmax1-xmin1), lw=2.0, zorder=2, color='grey') # target bottom
            rot2 = -45.0
            xmin2, xmax2 = phases[2], phases[3]
            ax_.plot(np.arange(xmin2, xmax2), [rot2]*int(xmax2-xmin2), lw=2.0, zorder=2, color='k') # target center
            ax_.plot(np.arange(xmin2, xmax2), [rot2+10]*int(xmax2-xmin2), lw=2.0, zorder=2, color='grey') # target top
            ax_.plot(np.arange(xmin2, xmax2), [rot2-10]*int(xmax2-xmin2), lw=2.0, zorder=2, color='grey') # target bottom
            rot3 = 0.0
            xmin3, xmax3 = phases[4], phases[-1]
            ax_.plot(np.arange(xmin3, xmax3), [rot3]*int(xmax3-xmin3), lw=2.0, zorder=2, color='k') # target center
            ax_.plot(np.arange(xmin3, xmax3), [rot3+10]*int(xmax3-xmin3), lw=2.0, zorder=2, color='grey') # target top
            ax_.plot(np.arange(xmin3, xmax3), [rot3-10]*int(xmax3-xmin3), lw=2.0, zorder=2, color='grey') # target bottom

        ii += 1
    
    plt.legend(handles=handles, labels=labels, loc=4, fontsize=25)
    
    fig.suptitle("Average Endpoint Angles and SEMs", size=30, y=0.99)

    fig.set_size_inches(20, 15, forward=True)

    fig.savefig('./output_for_paper/Shmuelof (MODEL) Figure 2B.png')

    # from textwrap import wrap
    # ax.set_title('\n'.join(wrap( pp.paramsEnv["pdfSuffix"], 60 )), y=1.08)
