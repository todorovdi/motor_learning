import matplotlib.pyplot as plt
import plotparams as pp
from stdplots import *
import re
from scipy import stats

colorControls='blue'
colorPatients='skyblue'
colorRateControls='saddlebrown'
colorRatePatients='steelblue'

colorRate = colorRateControls  #[0.5, 0.5, 0, 1.0]

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

def plotInds(ax,fnames2d,inds,clr=[]):
    shadeFirst=0.3
    shadeSecond=0.6
    #colors = [ [shadeFirst, shadeFirst, shadeFirst, 1.0], [shadeSecond,shadeSecond,shadeSecond,1] ]
    #colors = [ [1, 0, 0, 1.0], [0, 0, 1, 1] ]
    colors = [ colorControls, colorPatients ]
    labels = ['controls', 'patients'] 

    if len(fnames2d) == 1:
        colors = [ clr ]
        colors = [ colorControls ]
        labels = [ 'controls' ]

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
        shadedErrorbar(ax,numsind,angs[inds],SEMs[inds],shade=0.6,colorMean=color)
        #angs_handle, = ax.plot(nums[inds], angs[inds], color=color, lw=1, label=label)
        #ax.fill_between(nums[inds], angs[inds]-SEMs[inds], angs[inds]+SEMs[inds], facecolor=(0.3, 0.3, 0.3, eopacity),edgecolor=(0,0,0,0))

def annotateG(ax,inds,xtickstep,twinyxtick=False,rateG=False):
    ax.set_xticks(range(inds[0],inds[-1])[::xtickstep] )
    ax.set_xlim([inds[0],inds[-1]])

    # for poster
    if poster_mode==True:
        ax.set_xticklabels(range(0,700,xtickstep))

    #ax.set_xlim([inds[0],inds[-1]])

    ax.yaxis.grid(True)
    ax.set_ylim([-10,30])
    ax.set_yticks(np.arange(-10,30,10))

    ax.xaxis.grid(True, which='minor')
    ax.xaxis.grid(True)

    if rateG==True:
        ax.set_ylim([0,2.5])
        ax.set_yticks(np.arange(0,2.5,0.5))

    if twinyxtick:
        ax_ = ax.twiny()
        ax_.set_xlim([inds[0],inds[-1]])
        #ax_.set_xticks(pp.phaseBegins[1:-1])
        #ax_.set_xticklabels(pp.phaseNames)

        ax_.set_xticks([50,150,310,520])
        ax_.set_xticklabels(['KR']+pp.phaseNames)
        if poster_mode==True:
            ax_.tick_params(axis=u'x', which=u'both',length=0)

        #xtickloc = []
        #for i in range(pp.numPhases):
        #    xtickk = float(pp.phaseBegins[i] + pp.phaseBegins[i+1] )/2.
        #    xtickloc.append(xtickk)

        #labels = ['BASELINE']+pp.phaseNames

        #ax_.set_xticks(xtickloc)
        #ax_.set_xticklabels(labels)

def meanSEMpooled(dat,inds_beg,inds_end):
    rmeans = []
    rSEMs = []

    for i in range(len(inds_beg)):
        # all files
        rng = range(inds_beg[i],inds_end[i])
        #print(type(dat))
        subdat = dat[:,rng]
        l1,l2=subdat.shape

        submeans=np.zeros(l1)

        # range over files
        for j in range(l1):
            submeans[j] = np.mean(subdat[j,:])

        #submean = np.mean(subdat.flatten())
        #subSEM = stats.sem(subdat.flatten())

        submean = np.mean(submeans)
        subSEM = stats.sem(submeans)

        rmeans.append(submean)
        rSEMs.append(subSEM)

    return rmeans, rSEMs

def calcMeanLRates(fnames,inds_beg,inds_end):
    #if avg==False:
    #fnames=[fnames]
    nfiles=len(fnames)

    n = pp.phaseBegins[-1]
    allRates = np.zeros(shape=(nfiles,n) )
    for i,fname in enumerate(fnames):
        misc = np.loadtxt(fname)
        allRates[i,:] = misc[:,0]

    # means across sessions
    #rates = np.zeros(n)
    #for r in range(n):
    #    rates[r] = np.mean(allRates[:,r])

    #print("-- mean rate between {0} and {1} is {2}. SEM is {3}".format(inds_beg[i],inds_end[i],t,tsem))
    return meanSEMpooled(allRates,inds_beg,inds_end) 


def barchart(ax,meansC,meansP,SEMsC,SEMsP):

    width = 0.07       # the width of the bars
    ind = 0.20* np.arange(2)  # the x locations for the groups

    #fig, ax = plt.subplots()
    rectCM = ax.bar(ind[0], meansC[0], width, color='r', yerr=SEMsC[0],hatch=r'\\',label='Controls Multi',edgecolor='black')
    rectCS = ax.bar(ind[0]+width, meansC[1], width, color='r', yerr=SEMsC[1],label='Controls Single',edgecolor='black')

    rectPM = ax.bar(ind[1], meansP[0], width, color='b', yerr=SEMsP[0],hatch=r'\\',label='Patients Multi',edgecolor='black')
    rectPS = ax.bar(ind[1]+width, meansP[1], width, color='b', yerr=SEMsP[1],label='Patients Multi',edgecolor='black')

    ymax = 2.
    # add some text for labels, title and axes ticks
    ax.set_ylabel('Learning rate',labelpad=8)
    #ax.set_title('Scores by group and gender')
    #ax.set_xticks(ind + width / 2)
    ax.set_xticks(ind + width / 2.)
    ax.set_yticks(np.arange(0.4, 2,0.3))
    ax.set_ylim([0.0,2])
    ax.set_xticklabels(('Controls', 'Patients'))

    #ax.legend(handles=[rectCM, rectCS, rectPM, rectPS])

    #ax.legend((rectCM, rectCS), ('Multi', 'Single'))
    #ax.legend((rectPM, rectPS), ('Multi', 'Single'))
    #ax.legend((rectPM, rectPS), ('Multi', 'Single'))

# labels array of param names
def barchart_multi(ax,means2d,SEMs2d,labels,colors,showLabels=True,showTicks=True):
    #print('meansd2ds', means2d.shape)
    l1,l2 = means2d.shape
    width = 0.07       # the width of the bars
    ind = (width*(l2+1))* np.arange(len(means2d))  # the x locations for the groups

    #hpat=['-', '/', r'\\', '//', '.', '*']


    # adapt mult, adapt single, lrate mult, lrate single
    for i,means in enumerate(means2d):
        for j in range(len(means)):
        #rect = 
            ax.barh(ind[i]+width*j, means2d[i,j], width, color=colors[j], xerr=SEMs2d[i,j],edgecolor='black')
            # ,hatch=hpat[j]

    # add some text for labels, title and axes ticks
    #ax.set_ylabel('Learning rate',labelpad=8)
    #ax.set_title('Scores by group and gender')
    #ax.set_xticks(ind + width / 2)

    ymax = 1.5
    ax.set_xlim([0.,ymax])
    if(showTicks):
        ax.set_xticks(np.arange(0, ymax,0.3))
    else:
        ax.set_xticks([])

    ax.set_yticks(ind + 0.5*(l2-1)*width  )
    ax.set_yticklabels(labels)

#def autolabel(rects):
#    """
#    Attach a text label above each bar displaying its height
#    """
#    for rect in rects:
#        height = rect.get_height()
#        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
#                '%d' % int(height),
#                ha='center', va='bottom')
#
#autolabel(rects1)
#autolabel(rects2)

def makePubPlot(fnames2d,pdf):
    global poster_mode

    poster_mode=False


    globalFontSz=14
    mpl.rcParams.update({'font.size': globalFontSz})

    wordTextWidth=6.78
    wordSingleFigHeight=3.38


    # shiny plotting code goes here
    fig = plt.gcf()
    ax = plt.gca()

    print("-----shiny plotting code is exectuded here")

    xrots = np.array( [0, 200, 236,  
            236, 236+18*1, 
            236+18*1, 236+18*2,
            236+18*2, 236+18*3,
            236+18*3, 236+18*5,  
            236+18*5, 236+18*6,  
            236+18*6, 236+18*7,  
            236+18*7, 236+18*8,  
            236+18*8, 236+18*8+36, 236+18*8+72, 
            236+18*8+72, 236+18*8+72+54, 
            236+18*8+72+54,  236+18*8+72+54+36, 
            236+18*8+72+54+36,  236+18*8+72+54*2+36, 
            236+18*8+72+54*2+36, 236+18*8+72+54*2+36*2] )


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
 
    #print("---------yrots----")
    #for i,t in enumerate(yrots):
    #    print(i,"  ",t,"  ",xrots[i])

    # starts and ends of 20 deg pert
    pert_largest_begins=[290,452]
    pert_largest_ends=[326,506]

    # starts and ends of second half of 20 deg pert
    for i,b in enumerate(pert_largest_begins):
        pert_largest_begins[i] = int(float(pert_largest_begins[i] + pert_largest_ends[i]) / 2. )
    

    #
    pert_begins=[200,416]
    pert_ends=[416,520]

    

    fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
    controlRateMeans, controlRateSEMs = calcMeanLRates(fnamesCBMisc,[200,420],[400,520]) 
    fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames2d[1] ]
    patientsRateMeans, patientsRateSEMs = calcMeanLRates(fnamesCBMisc,[200,440],[400,550]) 
    # controlRateStats -- [gradual mean, single mean], gradual SEM 
    
    meanArr = np.zeros((len(fnames2d),4))
    SEMArr = np.zeros((len(fnames2d),4))
    for k,fnames in enumerate(fnames2d):
        filename = fnames[0] 
        #print('filename', filename)
        ree = '(.*)_\w+\.dat'
        basename = os.path.basename(filename)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        pp.paramsInit(filename.replace("_arm","_modParams"),False)

        dat = np.zeros((len(fnames), xrots[-1]))
        for i,fname in enumerate(fnames):
            rawData = np.loadtxt(fnames[i])
            tdat = getReachAngles(rawData)
            dat[i,:] = tdat 

        fnamesCBMisc=[ fname.replace('arm','CBMisc') for fname in fnames ]
        meanRates, SEMRates = calcMeanLRates(fnamesCBMisc,pert_begins,pert_ends) 
        meanAngs, SEMAngs = meanSEMpooled(dat,pert_largest_begins,pert_largest_ends) 

        meanArr[k,:] = np.concatenate((meanRates, meanAngs), axis=0)
        SEMArr[k,:] =  np.concatenate((SEMRates, SEMAngs), axis=0) 


    indMultAux=range(1,19)
    indSingleAux=range(18,len(yrots) )

    indMult=range(xrots[indMultAux[0] ] , xrots[ indMultAux[-1] ]) 
    indSingle=range( xrots[indSingleAux[0] ], xrots[ indSingleAux[-1] ]) 

    #print('--------------indMult',indMult)
    #print('--------------indSingle',indSingle)

    ########################

    #fig.set_size_inches(wordTextWidth, wordSingleFigHeight, 
    #        forward=True)

    #plt.subplots_adjust(left=0.10, right=0.98, bottom=0.18, top=0.82)

    inds = range(pp.phaseBegins[0],pp.phaseBegins[-1])
    #plotInds(ax,fnames2d,inds)

    ## plot rotations
    ##rot0 = 0.0
    ##xmin0, xmax0 = 0, phases[0]
    ##ax_.plot(np.arange(xmin0, xmax0), [rot0]*int(xmax0-xmin0), lw=2.0, zorder=2, color='k') # target center


    #ax.plot(xrots,yrots, color='black')

    #annotateG(ax,inds,50,True)
    #ax.set_ylabel(r"Error in degrees",labelpad=8)
    #ax.set_xlabel("Movement Number", labelpad=8)

    #if poster_mode==True:
    #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=8)

    ##fig.suptitle("Reach angle hand space minus target location",size=1.5*globalFontSz)#, y=0.99)

    #pdf.savefig()
    #plt.close()
#############################

    param = "cbLRateIsConst"
    if param in pp.paramsEnv and pp.paramsEnv[param] != "":
        constRate = int(pp.paramsEnv[param])
    else:
        constRate = 0

    

    numConditions = len(fnames2d)
    if numConditions == 2: 
        fig = plt.gcf()
        ax = plt.gca()

        if( constRate == 0 ):
            #nr=6
            nr=5
            if poster_mode!=True:
                fig, axs = plt.subplots(ncols=1, nrows=nr, 
                        figsize=(wordTextWidth, wordSingleFigHeight*3), 
                        sharex=False, sharey=False)
                plt.subplots_adjust(left=0.12, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.3)
            else:
                fig, axs = plt.subplots(ncols=1, nrows=nr, 
                        figsize=(wordTextWidth, (wordSingleFigHeight*nr)*0.9 ), 
                        sharex=False, sharey=False,gridspec_kw={'height_ratios':[1.7,1,1,1,1]})
                plt.subplots_adjust(left=0.12, right=0.98, bottom=0.055, top=0.97, wspace=0.2, hspace=0.58)

            #plt.subplots_adjust(left=0.11, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.4)

            # plot rotations
            #rot0 = 0.0
            #xmin0, xmax0 = 0, phases[0]
            #ax_.plot(np.arange(xmin0, xmax0), [rot0]*int(xmax0-xmin0), lw=2.0, zorder=2, color='k') # target center

            xpanellabel=-0.04
            ypanellabel=1.04


            ax = axs[0]
            ffirst = [ [fnames[0]] for fnames in fnames2d]
            #print('-----------------ffirst',ffirst)
            plotInds(ax,ffirst,inds)
            ax.plot(xrots,yrots, color='black',lw=0.8)
            annotateG(ax,inds,100,True)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)
            
            ax.text(xpanellabel, ypanellabel, 'A', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

            ax = axs[1]
            inds = indMult
            plotInds(ax,fnames2d,inds)
            ax.plot(xrots[indMultAux],yrots[indMultAux], color='black',lw=0.8)
            annotateG(ax,inds,50)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)

            ax.text(xpanellabel, ypanellabel, 'B', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

            ########################################

            ax = axs[2]
            inds = indMult

            fnamesCBMiscControls=[ fname.replace('arm','CBMisc') for fname in fnames2d[0] ]
            fnamesCBMiscPatients=[ fname.replace('arm','CBMisc') for fname in fnames2d[1] ]

            genCBMiscPlot(fig,ax,fnamesCBMiscControls,rateOnly=True,avg=True,subrange=inds,colorMean=colorRateControls )
            genCBMiscPlot(fig,ax,fnamesCBMiscPatients,rateOnly=True,avg=True,subrange=inds,colorMean=colorRatePatients)

            annotateG(ax,inds,50,rateG=True)
            ax.set_ylabel(r"Learning rate",labelpad=16)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)
            ax.plot(xrots[indMultAux],0.1*yrots[indMultAux], color='black',lw=0.8)

            ax.text(xpanellabel, ypanellabel, 'C', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')
            
            ax.set_title("")

            ########################################

            ax = axs[3]
            inds = indSingle
            plotInds(ax,fnames2d,inds)
            ax.plot(xrots[indSingleAux],yrots[indSingleAux], color='black')
            annotateG(ax,inds,50)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)

            ax.text(xpanellabel, ypanellabel, 'D', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

            ########################################


            ax = axs[4]
            inds = indSingle
            genCBMiscPlot(fig,ax,fnamesCBMiscControls,rateOnly=True,avg=True,subrange=inds,colorMean=colorRateControls )
            genCBMiscPlot(fig,ax,fnamesCBMiscPatients,rateOnly=True,avg=True,subrange=inds,colorMean=colorRatePatients)
            annotateG(ax,inds,50,rateG=True)
            ax.set_ylabel(r"Learning rate",labelpad=16)
            ax.set_xlabel("Movement Number", labelpad=8)

            ax.plot(xrots[indSingleAux],0.1*yrots[indSingleAux], color='black')

            ax.text(xpanellabel, ypanellabel, 'E', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')
            ax.set_title("")

###############################################################
###########  COnstant LRate           #########################
###############################################################
        else:
            nr=3
            if poster_mode!=True:
                fig, axs = plt.subplots(ncols=1, nrows=nr, 
                        figsize=(wordTextWidth, wordSingleFigHeight*3), 
                        sharex=False, sharey=False)
                plt.subplots_adjust(left=0.12, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.3)
            else:
                fig, axs = plt.subplots(ncols=1, nrows=nr, 
                        figsize=(wordTextWidth, (wordSingleFigHeight*nr)*0.9 ), 
                        sharex=False, sharey=False,gridspec_kw={'height_ratios':[1.7,1,1,1,1]})
                plt.subplots_adjust(left=0.12, right=0.98, bottom=0.055, top=0.97, wspace=0.2, hspace=0.58)

            #plt.subplots_adjust(left=0.11, right=0.98, bottom=0.08, top=0.92, wspace=0.4, hspace=0.4)

            # plot rotations
            #rot0 = 0.0
            #xmin0, xmax0 = 0, phases[0]
            #ax_.plot(np.arange(xmin0, xmax0), [rot0]*int(xmax0-xmin0), lw=2.0, zorder=2, color='k') # target center

            xpanellabel=-0.04
            ypanellabel=1.04


            ax = axs[0]
            ffirst = [ [fnames[0]] for fnames in fnames2d]
            #print('-----------------ffirst',ffirst)
            plotInds(ax,ffirst,inds)
            ax.plot(xrots,yrots, color='black',lw=0.8)
            annotateG(ax,inds,100,True)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)
            
            ax.text(xpanellabel, ypanellabel, 'A', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

            ax = axs[1]
            inds = indMult
            plotInds(ax,fnames2d,inds)
            ax.plot(xrots[indMultAux],yrots[indMultAux], color='black',lw=0.8)
            annotateG(ax,inds,50)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)

            ax.text(xpanellabel, ypanellabel, 'B', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

            ########################################

            ax = axs[2]
            inds = indSingle
            plotInds(ax,fnames2d,inds)
            ax.plot(xrots[indSingleAux],yrots[indSingleAux], color='black')
            annotateG(ax,inds,50)
            ax.set_ylabel(r"Error in degrees",labelpad=8)
            ax.set_xlabel("Movement Number", labelpad=8)
            #if poster_mode==True:
            #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
            #    ax.set_xlabel("Trial Number", labelpad=5)

            ax.text(xpanellabel, ypanellabel, 'D', transform=ax.transAxes,
              fontsize=16, fontweight='bold', va='top', ha='right')

        #if poster_mode!=True:
        #    fig.suptitle("Reach angle hand space minus target location",size=globalFontSz*1.5)#, y=0.99)

        #ax = axs[5]
        ##means = [controlRateMeans, patientsRateMeans ]
        ##SEMs = [controlRateSEMs, patientsRateSEMs ]
        #barchart(ax,controlRateMeans, patientsRateMeans, controlRateSEMs, patientsRateSEMs)

        #ax.text(xpanellabel, ypanellabel, 'F', transform=ax.transAxes,
        #  fontsize=16, fontweight='bold', va='top', ha='right')
     
        pp.exportSvgEmf(plt)

        #noExt=pp.out_dir_pdf + pp.plotfname
        #svgname=noExt+'.svg'
        #print svgname
        #plt.savefig(svgname)#,bbox_inches='tight')

        #import os
        #inkscapeArgs ='inkscape --without-gui --export-emf="'+noExt+'.emf" "'+svgname+'"'
        #print inkscapeArgs
        #os.system(inkscapeArgs)

        ############################################################################################ 
        ############################################################################################ 
        ############################################################################################ 
        pdf.savefig()
        plt.close()

    fig = plt.gcf()
    ax = plt.gca()

    labels=[ 'controls', 'patients' ]
 
    globalFontSz=10
    mpl.rcParams.update({'font.size': globalFontSz})

    if(len(fnames2d)>2):
        labels=["controls",
        "+Noise +Var. est." ,
        "+Noise +Var. est. +Degradation" ,
        "+Noise" ,
        "+Noise -Var. est." ,
        "-optimality coefficient" ,
        "--Var. est." ,
        #"redErrThr2" ,
        "+Noise -Speed_up" ,
        "+Noise +Speed_down" ]

    fig,axs = plt.subplots(ncols=1,nrows=1,
                figsize=(wordTextWidth*2, wordSingleFigHeight*3),sharey=True )
    plt.subplots_adjust(left=0.0, right=0.98, bottom=0.05, top=1.0)
 
 
    from matplotlib.gridspec import GridSpec

                       #width_ratios=[1, 2],
                       #height_ratios=[4, 1]
    gs = GridSpec(numConditions*2, 5, width_ratios=[0.8,1,0.8,0.5,0.5] )
    gs.update(left=0.05, wspace=0.15, hspace=0.3)

    colors = ['violet','cyan','yellowgreen', 'yellow' ]
    # mult, single, mult, single


    for i in range(numConditions):


        axLvlMult = plt.subplot(gs[2*i, 3])
        axLrateMult = plt.subplot(gs[2*i+1, 3])
        axLvlSingle = plt.subplot(gs[2*i, 4])
        axLrateSingle = plt.subplot(gs[2*i+1, 4])

        fnames = [fnames2d[i] ]

        ax = axLvlMult
        inds = indMult
        plotInds(ax,fnames,inds)
        ax.plot(xrots[indMultAux],yrots[indMultAux], color='black',lw=0.8)
        annotateG(ax,inds,50)
        ax.yaxis.grid(False)

        ax.set_xticklabels=([])
        ax.set_xticks([])
        #ax.set_ylabel(r"Error in degrees",labelpad=8)
        #ax.set_xlabel("Movement Number", labelpad=8)
        #if poster_mode==True:
        #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
        #    ax.set_xlabel("Trial Number", labelpad=5)
        ax.set_yticks(np.arange(-10,30,10))
        ax.tick_params(axis='both', labelsize=8)

        ########################################

        ax = axLrateMult
        inds = indMult

        fnamesCBMisc =[ fname.replace('arm','CBMisc') for fname in fnames[0] ]

        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,subrange=inds,colorMean=colorRate )


        annotateG(ax,inds,50,rateG=True)
        ax.yaxis.grid(False)
        #ax.set_ylabel(r"Learning rate",labelpad=8)
        #ax.set_xlabel("Movement Number", labelpad=8)
        #if poster_mode==True:
        #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
        #    ax.set_xlabel("Trial Number", labelpad=5)
        #ax.plot(xrots[indMultAux],0.1*yrots[indMultAux], color='black',lw=0.8)
        ax.set_title("")

        ax.set_xticklabels=([])
        ax.set_xticks([])              
        ax.set_yticks(np.arange(0,2.5,0.5))
        ax.tick_params(axis='both', labelsize=8)

        ########################################

        ax = axLvlSingle
        inds = indSingle
        plotInds(ax,fnames,inds)
        ax.plot(xrots[indSingleAux],yrots[indSingleAux], color='black')
        annotateG(ax,inds,50)
        ax.set_yticklabels=([])
        ax.set_yticks([])
        #ax.set_ylabel(r"Error in degrees",labelpad=8)
        #ax.set_xlabel("Movement Number", labelpad=8)
        #if poster_mode==True:
        #    ax.set_ylabel(r"Reach Angle (deg)",labelpad=5)
        #    ax.set_xlabel("Trial Number", labelpad=5)

        ax.set_xticklabels=([])
        ax.set_xticks([])

        ########################################

        ax = axLrateSingle 
        inds = indSingle
        genCBMiscPlot(fig,ax,fnamesCBMisc,rateOnly=True,avg=True,subrange=inds,colorMean=colorRate )
        annotateG(ax,inds,50,rateG=True)
        #ax.set_ylabel(r"Learning rate",labelpad=8)
        #ax.set_xlabel("Movement Number", labelpad=8)

        #ax.plot(xrots[indSingleAux],0.1*yrots[indSingleAux], color='black')
        ax.set_title("")
        ax.set_yticklabels=([])
        ax.set_yticks([])

        ax.set_xticklabels=([])
        ax.set_xticks([])

        axFistCol = plt.subplot(gs[ (2*i):(2*(i+1))  , 1])
        axSecCol  = plt.subplot(gs[(2*i):(2*(i+1)), 2])

        ax=axFistCol
        barchart_multi(ax,meanArr[[i],0:2],SEMArr[[i],0:2],[],colors[0:2],showTicks=False)

        ax=axSecCol
        barchart_multi(ax,meanArr[[i],2:4],SEMArr[[i],2:4],[],colors[2:4],showTicks=False)
        axSecCol.set_xlim([0.,20.])

        axTxt = plt.subplot(gs[ (2*i):(2*(i+1))  , 0]) 
        ax = axTxt
        ax.text(0.5, 0.5, labels[i], ha='center', va='center')
        ax.axis('off')

    axFistCol.set_xticks(np.arange(0, 1.5,0.3))
    axSecCol.set_xticks(np.arange(0, 20.,5.))
    inds = indSingle
    axLrateSingle.set_xticks(range(inds[0],inds[-1])[::100] )
    inds = indMult
    axLrateMult.set_xticks(range(inds[0],inds[-1])[::100] ) 
 
    #plt.tight_layout()

    #if poster_mode!=True:
    #    fig.suptitle("Reach angle hand space minus target location",size=globalFontSz*1.5)#, y=0.99)



    #plot_names = []

    #fig, axs = plt.subplots(ncols=3, nrows=3, 
    #        figsize=(wordTextWidth, (wordSingleFigHeight*nr)*0.9 ), 
    #        sharex=False, sharey=False,gridspec_kw={'height_ratios':[1.7,1,1,1,1,1.2]})
    #plt.subplots_adjust(left=0.12, right=0.98, bottom=0.055, top=0.97, wspace=0.2, hspace=0.58)


    pp.exportSvgEmf(plt,'_altogether')
    #3x3
