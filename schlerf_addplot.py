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

def plotInds(ax,fnames2d,inds):
    shadeSecond=0.45
    colors = [ [0, 0, 0, 1.0], [shadeSecond,shadeSecond,shadeSecond,1] ]
    labels = ['controls', 'patients'] 

    for fnames,label,color in zip(fnames2d,labels,colors):
        filename = fnames[0] 
        #print('filename', filename)
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

        numsind=nums[inds]
        shadedErrorbar(ax,numsind,angs[inds],SEMs[inds])
        angs_handle, = ax.plot(nums[inds], angs[inds], color=color, lw=1, label=label)
        #ax.fill_between(nums[inds], angs[inds]-SEMs[inds], angs[inds]+SEMs[inds], facecolor=(0.3, 0.3, 0.3, eopacity),edgecolor=(0,0,0,0))

def annotateG(ax,inds,xtickstep,twinyxtick=False):
    ax.set_xticks(range(inds[0],inds[-1])[::xtickstep] )
    ax.set_xlim([inds[0],inds[-1]])
    ax.yaxis.grid(True)

    ax.set_ylim([-10,30])
    ax.set_yticks(np.arange(-10,30,10))

    ax.xaxis.grid(True, which='minor')

    if twinyxtick:
        ax_ = ax.twiny()
        ax_.set_xlim([inds[0],inds[-1]])
        ax_.set_xticks(pp.phaseBegins[1:-1])
        ax_.set_xticklabels(pp.phaseNames)
        ax_.xaxis.grid(True,color='w')


def makePubPlot(fnames2d,pdf):

    globalFontSz=10
    mpl.rcParams.update({'font.size': globalFontSz})

    wordTextWidth=6.78
    wordSingleFigHeight=3.38

    # shiny plotting code goes here
    fig = plt.gcf()
    ax = plt.gca()

    print("-----shiny plotting code is exectuded here")

    xrots = np.array( [0, 200, 232,  
            232, 232+16*1, 
            232+16*1, 232+16*2,
            232+16*2, 232+16*3,
            232+16*3, 232+16*5,  
            232+16*5, 232+16*6,  
            232+16*6, 232+16*7,  
            232+16*7, 232+16*8,  
            232+16*8, 232+16*8+32, 232+16*8+64, 
            232+16*8+64, 232+16*8+64+48, 
            232+16*8+64+48,  232+16*8+64+48+32, 
            232+16*8+64+48+32,  232+16*8+64+48*2+32, 
            232+16*8+64+48*2+32, 232+16*8+64+48*2+32*2] )
    yrots = np.array([0, 0, 0, 
            5, 5, 
            10, 10, 
            15, 15, 
            20, 20, 
            15, 15, 
            10, 10, 
            5, 5, 
            0, 0, 0,
            20, 20,
            0, 0,
            20, 20,
            0, 0]   )

    indGradualAux=range(1,19)
    indAbruptAux=range(18,len(yrots) )

    indGradual=range(xrots[indGradualAux[0] ] , xrots[ indGradualAux[-1] ]) 
    indAbrupt=range( xrots[indAbruptAux[0] ], xrots[ indAbruptAux[-1] ]) 

    #print('--------------indGradual',indGradual)
    #print('--------------indAbrupt',indAbrupt)

    ########################

    fig.set_size_inches(wordTextWidth, wordSingleFigHeight, 
            forward=True)

    plt.subplots_adjust(left=0.10, right=0.98, bottom=0.18, top=0.82)

    inds = range(pp.phaseBegins[0],pp.phaseBegins[-1])
    plotInds(ax,fnames2d,inds)

    # plot rotations
    #rot0 = 0.0
    #xmin0, xmax0 = 0, phases[0]
    #ax_.plot(np.arange(xmin0, xmax0), [rot0]*int(xmax0-xmin0), lw=2.0, zorder=2, color='k') # target center


    ax.plot(xrots,yrots, color='black')

    annotateG(ax,inds,50,True)
    ax.set_ylabel(r"Error in degrees",labelpad=8)
    ax.set_xlabel("Movement Number", labelpad=8)

    #fig.suptitle("Reach angle hand space minus target location",size=1.5*globalFontSz)#, y=0.99)

    pdf.savefig()
    plt.close()
#############################

    fig = plt.gcf()
    ax = plt.gca()

    fig, axs = plt.subplots(ncols=1, nrows=3, 
            figsize=(wordTextWidth, wordSingleFigHeight*3), 
            sharex=False, sharey=False)

    plt.subplots_adjust(left=0.12, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.3)

    #plt.subplots_adjust(left=0.11, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.4)

    # plot rotations
    #rot0 = 0.0
    #xmin0, xmax0 = 0, phases[0]
    #ax_.plot(np.arange(xmin0, xmax0), [rot0]*int(xmax0-xmin0), lw=2.0, zorder=2, color='k') # target center

    ax = axs[0]
    ffirst = [ [fnames[0]] for fnames in fnames2d]
    #print('-----------------ffirst',ffirst)
    plotInds(ax,ffirst,inds)
    ax.plot(xrots,yrots, color='black',lw=0.8)
    annotateG(ax,inds,100,True)
    ax.set_ylabel(r"Error in degrees",labelpad=8)
    ax.set_xlabel("Movement Number", labelpad=8)
    
    ax.text(-0.14, 1.06, 'A', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

    ax = axs[1]
    inds = indGradual
    plotInds(ax,fnames2d,inds)
    ax.plot(xrots[indGradualAux],yrots[indGradualAux], color='black',lw=0.8)
    annotateG(ax,inds,50)
    ax.set_ylabel(r"Error in degrees",labelpad=8)
    ax.set_xlabel("Movement Number", labelpad=8)

    ax.text(-0.14, 1.06, 'B', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

    ax = axs[2]
    inds = indAbrupt
    plotInds(ax,fnames2d,inds)
    ax.plot(xrots[indAbruptAux],yrots[indAbruptAux], color='black')
    annotateG(ax,inds,50)
    ax.set_ylabel(r"Error in degrees",labelpad=8)
    ax.set_xlabel("Movement Number", labelpad=8)

    ax.text(-0.14, 1.06, 'C', transform=ax.transAxes,
      fontsize=16, fontweight='bold', va='top', ha='right')

    fig.suptitle("Reach angle hand space minus target location",size=globalFontSz*1.5)#, y=0.99)

    noExt=pp.out_dir_pdf + pp.plotfname
    svgname=noExt+'.svg'
    print svgname
    plt.savefig(svgname)#,bbox_inches='tight')

    import os
    inkscapeArgs ='inkscape --without-gui --export-emf="'+noExt+'.emf" "'+svgname+'"'
    print inkscapeArgs
    os.system(inkscapeArgs)

