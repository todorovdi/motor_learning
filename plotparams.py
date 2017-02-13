import ConfigParser
import StringIO
 
#if __name__ != "__main__":
#    paramsEnv = ConfigParser.RawConfigParser(allow_no_value=True) 
#    print "import thing"

def paramFileRead(fname):
    ini_str = '[root]\n' + open(fname, 'r').read()
    ini_fp = StringIO.StringIO(ini_str)
    params = ConfigParser.RawConfigParser(allow_no_value=True)
    params.readfp(ini_fp)
    return params

def paramsInit(fname):
    global paramsEnv
    global armFileSkiprows
    global rewardDist
    global paramsML
    global trials1
    global trials2
    global out_dir
    global numPhases
    global numTrialsPre
    global numTrialsAdapt
    global numTrialsPost
    global trials1End

    paramsEnv = paramFileRead(fname) 

    rewardDist = float(paramsEnv.get("root","rewardDist") )
    out_dir = paramsEnv.get("root","output_dir")
    iniML = paramsEnv.get("root","iniML")
    iniBG = paramsEnv.get("root","iniBG")
    iniCB = paramsEnv.get("root","iniCB")
    iniArm = paramsEnv.get("root","iniArm")

    armFileSkiprows = int(paramsEnv.get("root","armFileSkiprows"))

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
