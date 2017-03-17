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

def paramsInit(fname,readMultParamFiles = True):
    global paramsEnv
    global paramsML
    global paramsCB
    global armFileSkiprows
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
    global phaseEnds
    global phaseNames

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

    armFileSkiprows = int(paramsEnv["armFileSkiprows"])

    if(readMultParamFiles):
        if "iniML" in paramsEnv:
            iniML = paramsEnv["iniML"]
            paramsML = paramFileRead(iniML)
            paramsEnv.update(paramsML)
        if "iniCB" in paramsEnv:
            iniCB = paramsEnv["iniCB"]
            paramsCB = paramFileRead(iniCB)
            paramsEnv.update(paramsCB)

    if "dirShift" in paramsEnv:
        dirShift = float(paramsEnv["dirShift"])
    if "numTrialsPre" in paramsEnv:
        numTrialsPre = int(paramsEnv["numTrialsPre"])
    if "numTrialsAdapt" in paramsEnv:
        numTrialsAdapt = int(paramsEnv["numTrialsAdapt"])
    if "numTrialsPost" in paramsEnv:
        numTrialsPost = int(paramsEnv["numTrialsPost"])

    numPhases = int(paramsEnv["numPhases"])


    phaseEnds = []
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
            phaseEnds.append(curTrial)
        curTrial = curTrial + n
    phaseEnds.append(curTrial)

    #if ("numTrialsPre" in paramsEnv) and len(phaseEnds) == 0:
    trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
    trials1 = range(trials1End)
    trials2 = range(trials1End,trials1End*2)
    print "fdsfd"
