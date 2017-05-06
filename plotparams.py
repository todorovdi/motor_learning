try:
    import ConfigParser
except ModuleNotFoundError:
    import configparser as ConfigParser
try:
    import StringIO
except ModuleNotFoundError:
    import io as StringIO

import math
 
#if __name__ != "__main__":
#    paramsEnv = ConfigParser.RawConfigParser(allow_no_value=True) 
#    print "import thing"

def paramFileRead(fname):
    ini_str = '[root]\n' + open(fname, 'r').read()
    ini_fp = StringIO.StringIO(ini_str)
    params = ConfigParser.RawConfigParser(allow_no_value=True)
    params.optionxform = str 
    params.readfp(ini_fp)
    #sect = paramsEnv_pre.sections()
    items= params.items('root')
    params_ = dict(items)
    return params_

def eucl2angDist(d):
    arr = float(paramsEnv["armReachRadius"]) 
    szrad = math.asin(d/arr ) 
    return ( szrad / math.pi * 180. )

def exportVarsInit(params_):    # to avoid recalc if one just want to change export param (currently one takes all params from last initizlized param file, which is usually one from output data)
    #global armFileSkiprows
    global pdfForEachSession
    global reachBoxXsz
    global reachBoxYsz
    global cbMiscGraph_y_axis_max
    global cbMiscErrMult
    global xtickSkip
    global plotReachAngles
    global y_axis_max
    global y_axis_min
    global y_axis_signed
    global y_axis_step
    global emphPhase
    global showPointNumbers
    global multiSameGraph
    global plotPubFile
    global signed_dist_err
    global plotAngleErrs
    global datMult
    global cbStateMax
    global baseAng_reachAngDisp
    global target_radius
    global multi_onlyMainPlots
    global rwdPlot_ymax
    global rwdPlot_ymin
    global rwdPlot_step
    global multi_ncols
    #global plotHandSpace

    #armFileSkiprows = int(params_["armFileSkiprows"])
    pdfForEachSession = int(params_["pdfForEachSession"])
    reachBoxXsz = float(params_["reachBoxXsz"])
    reachBoxYsz = float(params_["reachBoxYsz"])
    cbMiscGraph_y_axis_max = float(params_["cbMiscGraph_y_axis_max"])
    cbMiscErrMult = float(params_["cbMiscErrMult"])
    xtickSkip = int(params_["xtickSkip"])
    y_axis_step = float(params_["y_axis_step"])

    #try:
    #    plotHandSpace = int(params_["plotHandSpace"])
    #except KeyError as e:
    #    plotHandSpace = 0
    #    print(str(e))

    try:
        multi_ncols = int(params_["multi_ncols"])
    except KeyError as e:
        multi_ncols = -1
        print(str(e))

    try:
        multi_onlyMainPlots = int(params_["multi_onlyMainPlots"])
    except KeyError as e:
        multi_onlyMainPlots = 0
        print(str(e))

    rsz = float(params_["rewardSize"])
    try:
        rwdPlot_ymax = float(params_["rwdPlot_ymax"])
    except KeyError as e:
        rwdPlot_ymax = rsz*2.
        print(str(e))

    try:
        rwdPlot_ymin = float(params_["rwdPlot_ymin"])
    except KeyError as e:
        rwdPlot_ymin = -1
        print(str(e))

    try:
        rwdPlot_step = float(params_["rwdPlot_step"])
    except KeyError as e:
        rwdPlot_step = rsz*0.5
        print(str(e))

    try:
        baseAng_reachAngDisp = float(params_["baseAng_reachAngDisp"])
    except KeyError as e:
        baseAng_reachAngDisp = 0
        print(str(e))

    try:
        cbStateMax = float(params_["cbStateMax"])
    except KeyError as e:
        cbStateMax = 0.3
        print(str(e))

    try:
        plotAngleErrs = int(params_["plotAngleErrs"])
    except KeyError as e:
        plotAngleErrs = 0
        print(str(e))

    try:
        datMult = int(params_["datMult"])
    except KeyError as e:
        datMult = 1
        print(str(e))
    
    try:
        signed_dist_err = int(params_["signed_dist_err"])
    except KeyError as e:
        signed_dist_err = 0
        print(str(e))

    try:
        multiSameGraph = int(params_["multiSameGraph"] )
    except KeyError as e:
        multiSameGraph = 0
        print(str(e))
    try:
        showPointNumbers = int(params_["showPointNumbers"] )
    except KeyError as e:
        showPointNumbers = 0
        print(str(e))
    try:
        plotReachAngles = int(params_["plotReachAngles"])
    except KeyError as e:
        plotReachAngles = 0
        print(str(e))
    y_axis_max = float(params_["y_axis_max"])
    y_axis_step = float(params_["y_axis_step"])
    try:
        y_axis_signed = int(params_["y_axis_signed"])
    except KeyError as e:
        y_axis_signed = 1
        print(str(e))
    try:
        y_axis_min = float(params_["y_axis_min"])
    except KeyError as e:
        y_axis_min = y_axis_max * (1-2*y_axis_signed )
        print(str(e))
    try:
        emphPhase = int(params_["emphPhase"])
    except KeyError as e:
        emphPhase = -1
        print(str(e))

    try:
        plotPubFile = params_["plotPubFile"]
    except KeyError as e:
        plotPubFile = ""
        print(str(e))
        
    target_radius = rewardDist
    if( plotReachAngles or plotAngleErrs):
        target_radius = 2*eucl2angDist(rewardDist/2.)

def paramsInit(fname,origParamFile = True):  #origParamFile -- if use the one before calc, or not. The one after calc contains changes from command line
    global paramsEnv
    global paramsML
    global paramsCB
    global rewardDist
    global trials1
    global trials2
    global out_dir
    global out_dir_pdf
    global out_dir_pdf_single
    global numPhases
    global numTrialsPre
    global numTrialsAdapt
    global numTrialsPost
    global trials1End
    global dirShift
    global phaseBegins
    global phaseNames

    paramsEnv = None
    paramsEnv = paramFileRead(fname) 

    rewardDist = float(paramsEnv["rewardDist"] )
    out_dir = paramsEnv["output_dir"]
    out_dir_pdf = paramsEnv["output_dir_pdf"]
    out_dir_pdf_single = paramsEnv["output_dir_pdf_singleSession"]
    #iniBG = paramsEnv["iniBG"]
    #iniArm = paramsEnv["iniArm"]

    from os.path import expanduser
    out_dir = expanduser(out_dir)
    out_dir_pdf = expanduser(out_dir_pdf)
    out_dir_pdf_single = expanduser(out_dir_pdf_single)

    if(origParamFile):
        if "iniML" in paramsEnv:
            ini = paramsEnv["iniML"]
            paramsML = paramFileRead(ini)
            paramsEnv.update(paramsML)
        if "iniCB" in paramsEnv:
            ini = paramsEnv["iniCB"]
            paramsCB = paramFileRead(ini)
            paramsEnv.update(paramsCB)
        if "iniAdd" in paramsEnv:
            ini = paramsEnv["iniAdd"]
            paramsAdd = paramFileRead(ini)
            paramsEnv.update(paramsAdd)

    if "dirShift" in paramsEnv:
        dirShift = float(paramsEnv["dirShift"])
    if "numTrialsPre" in paramsEnv:
        numTrialsPre = int(paramsEnv["numTrialsPre"])
    if "numTrialsAdapt" in paramsEnv:
        numTrialsAdapt = int(paramsEnv["numTrialsAdapt"])
    if "numTrialsPost" in paramsEnv:
        numTrialsPost = int(paramsEnv["numTrialsPost"])

    numPhases = int(paramsEnv["numPhases"])


    phaseBegins = [0]   # next to last trial index of a phase
    phaseNames = []
    curTrial = 0
    for i in range(numPhases):
        keyname = "numTrials" + str(i)
        if (keyname not in paramsEnv):
            break
        n = int(paramsEnv[keyname] )
        if ("name"+str(i) ) in paramsEnv:
            name = paramsEnv["name"+str(i)]
            if(name == ""):
                continue
            phaseNames.append(name)
            phaseBegins.append(curTrial)
        curTrial = curTrial + n
    phaseBegins.append(curTrial)   # last is the total num trials

    if ("numTrialsPre" in paramsEnv) and len(phaseBegins) == 0:
        trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
        trials1 = range(trials1End)
        trials2 = range(trials1End,trials1End*2)
        print("---- old version of ini file found")


