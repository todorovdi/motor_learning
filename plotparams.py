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
    global numPhases
    global numTrialsPre
    global numTrialsAdapt
    global numTrialsPost
    global trials1End
    global dirShift

    paramsEnv = paramFileRead(fname) 

    rewardDist = float(paramsEnv["rewardDist"] )
    out_dir = paramsEnv["output_dir"]
    out_dir_pdf = paramsEnv["output_dir_pdf"]
    #iniBG = paramsEnv["iniBG"]
    #iniArm = paramsEnv["iniArm"]

    from os.path import expanduser
    out_dir = expanduser(out_dir)

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

    targetPre1_def = float(paramsEnv["targetPre1"])
    targetPre2 = float(paramsEnv["targetPre2"])
    dirShift = float(paramsEnv["dirShift"])
    numPhases = int(paramsEnv["numPhases"])
    numTrialsPre = int(paramsEnv["numTrialsPre"])
    numTrialsAdapt = int(paramsEnv["numTrialsAdapt"])
    numTrialsPost = int(paramsEnv["numTrialsPost"])

    trials1End = numTrialsPre+numTrialsAdapt+numTrialsPost
    trials1 = range(trials1End)
    trials2 = range(trials1End,trials1End*2)
