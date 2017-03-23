import ConfigParser
import StringIO
 
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

def exportVarsInit(params_):    # to avoid recalc if one just want to change export param (currently one takes all params from last initizlized param file, which is usually one from output data)
    global armFileSkiprows
    global pdfForEachSession
    global reachBoxXsz
    global reachBoxYsz
    global cbMiscGraph_y_axis_max
    global cbMiscErrMult
    global xtickSkip
    global plotReachAngles
    global y_axis_max
    global y_axis_signed
    global y_axis_step
    global emphPhase
    global showPointNumbers
    global multiSameGraph
    global plotPubFile

    armFileSkiprows = int(params_["armFileSkiprows"])
    pdfForEachSession = int(params_["pdfForEachSession"])
    reachBoxXsz = float(params_["reachBoxXsz"])
    reachBoxYsz = float(params_["reachBoxYsz"])
    cbMiscGraph_y_axis_max = float(params_["cbMiscGraph_y_axis_max"])
    cbMiscErrMult = float(params_["cbMiscErrMult"])
    xtickSkip = int(params_["xtickSkip"])
    y_axis_step = float(params_["y_axis_step"])
    
    try:
        multiSameGraph = int(params_["multiSameGraph"] )
    except KeyError as e:
        multiSameGraph = 0
        #print "no showPointNumbers in params"
        print str(e)
    try:
        showPointNumbers = int(params_["showPointNumbers"] )
    except KeyError as e:
        showPointNumbers = 0
        #print "no showPointNumbers in params"
        print str(e)
    try:
        plotReachAngles = int(params_["plotReachAngles"])
    except KeyError as e:
        plotReachAngles = 0
        print str(e)
    y_axis_max = float(params_["y_axis_max"])
    y_axis_signed = int(params_["y_axis_signed"])
    y_axis_step = float(params_["y_axis_step"])
    try:
        emphPhase = int(params_["emphPhase"])
    except KeyError as e:
        emphPhase = -1
        #print "no emphPhase in params"
        print str(e)

    try:
        plotPubFile = params_["plotPubFile"]
    except KeyError as e:
        plotPubFile = ""
        print str(e)
        

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
        print "fdsfd"
