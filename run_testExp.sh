runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_test
experimentName=testExp
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

#make pert
#./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1

fullSim()
{
  # want to have AF here
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_EB=$pdfSuffix

  # want to avoid AF here
  perturbSimple " --percept_xrev1=1$addOptionsLoc" $nsess  $useOldData
  args_nonEB=$pdfSuffix

  # don't turn off CB too early
  #perturbSimple " --endpoint_rotation1=30 --endpoint_rotation2=30 --setRPre1=0 --cue1=0 --numTrials1=60 --numTrials2=80 --learn_cb2=0$addOptionsLoc" $nsess  $useOldData
  #args_enableBG=$pdfSuffix

  python "$plotfile" "$args_EB" "$args_nonEB" 
}

gradedRwd()
{
  addOptionsLoc=" --ini=testExp_gradedRwd.ini$1"
  fullSim 
}

perfErrDif()
{
  addOptionsLoc=" --ini=testExp_perfErrDif.ini$1"
  fullSim 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

perfErrDifNorm()
{
  addOptionsLoc=" --ini=testExp_perfErrDifNorm.ini$1"
  fullSim 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

perfFromLambda()
{
  addOptionsLoc=" --ini=tepl.ini$1"
  fullSim 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

gradedRwd_sweepPower()
{
  ao="$1"
  gradedRwd " --rwdGradePower=0.4$ao"
  a1EB=$args_EB
  a1nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=0.6$ao"
  a2EB=$args_EB
  a2nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=0.8$ao"
  a3EB=$args_EB
  a3nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=1.0$ao"
  a4EB=$args_EB
  a4nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=1.2$ao"
  a5EB=$args_EB
  a5nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=1.4$ao"
  a6EB=$args_EB
  a6nonEB=$args_nonEB
  gradedRwd " --rwdGradePower=1.6$ao"
  a7EB=$args_EB
  a7nonEB=$args_nonEB

  python "$plotfile" "$a1EB" "$a1nonEB" \
    "$a2EB" "$a2nonEB" \
    "$a3EB" "$a3nonEB" \
    "$a4EB" "$a4nonEB" \
    "$a5EB" "$a5nonEB" \
    "$a6EB" "$a6nonEB" \
    "$a7EB" "$a7nonEB" 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

gradedRwd_sweepCoef()
{
  ao="$1"
  gradedRwd " --Rpre_coef=0.85$ao"
  a1EB=$args_EB
  a1nonEB=$args_nonEB
  gradedRwd " --Rpre_coef=0.6$ao"
  a2EB=$args_EB
  a2nonEB=$args_nonEB
  gradedRwd " --Rpre_coef=0.4$ao"
  a3EB=$args_EB
  a3nonEB=$args_nonEB
  gradedRwd " --Rpre_coef=0.2$ao"
  a4EB=$args_EB
  a4nonEB=$args_nonEB

  python "$plotfile" "$a1EB" "$a1nonEB" \
    "$a2EB" "$a2nonEB" \
    "$a3EB" "$a3nonEB" \
    "$a4EB" "$a4nonEB" \

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

gradedRwd_sweepPowerRcoef()
{
  gradedRwd_sweepPower " --Rpre_coef=0.94$1"
  gradedRwd_sweepPower " --Rpre_coef=0.91$1"
  gradedRwd_sweepPower " --Rpre_coef=0.85$1"
  gradedRwd_sweepPower " --Rpre_coef=0.6$1"
  gradedRwd_sweepPower " --Rpre_coef=0.4$1"
  gradedRwd_sweepPower " --Rpre_coef=0.2$1"
}

# makes rwd = -3 for first adaptation trial
gradeAndReward()
{
  pow=$2
  dist=0.1368
  distpow=`echo "scale=4;e(l($dist)*$pow)" | bc -l`
  rwd=`echo "scale=4;$desiredRwd/$distpow" | bc -l`
  # bc -l

  ao="$1"
  perfFromLambda " --rwdGradePower=$pow --rewardSize=$rwd$ao"
}

fromLambda_sweepPower()
{
  gradeAndReward "$1" "0.5"
  a1EB=$args_EB
  a1nonEB=$args_nonEB
  gradeAndReward "$1" "1.0"             
  a2EB=$args_EB
  a2nonEB=$args_nonEB
  gradeAndReward "$1" "1.5"
  a3EB=$args_EB
  a3nonEB=$args_nonEB
  gradeAndReward "$1" "2"
  a4EB=$args_EB
  a4nonEB=$args_nonEB
  gradeAndReward "$1" "2.5"
  a5EB=$args_EB
  a5nonEB=$args_nonEB
  gradeAndReward "$1" "3"
  a6EB=$args_EB
  a6nonEB=$args_nonEB
  #gradeAndReward "$1" "0.1"
  #a7EB=$args_EB
  #a7nonEB=$args_nonEB

  python "$plotfile" \
    "$a2EB" "$a2nonEB" \
    "$a3EB" "$a3nonEB" \
    "$a4EB" "$a4nonEB" \
    "$a5EB" "$a5nonEB" \
    "$a6EB" "$a6nonEB" \
    "---plotfname=${1}_sweepPow"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}


if [ $# -ne 0 ]; then

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
    

  nsess=$1
  useOldData=$2   # 9 means don't plot

  echo "Starting experiment "$experimentName
      
  desiredRwd=1.5
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=5 --rwdFromcbLRate_add=-1"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=4 --rwdFromcbLRate_add=-1"

  fromLambda_sweepPower " --rwdFromcbLRate_mult=1.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=2"
  fromLambda_sweepPower " --rwdFromcbLRate_mult=2.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3"

  fromLambda_sweepPower " --rwdFromcbLRate_mult=1.5 --rwdFromcbLRate_add=-2"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=2 --rwdFromcbLRate_add=-2.5"
  fromLambda_sweepPower " --rwdFromcbLRate_mult=2.5 --rwdFromcbLRate_add=-3"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3 --rwdFromcbLRate_add=-3.5"


  #gradeAndReward " --rwdFromcbLRate_mult=4" "1.5"
  #gradeAndReward " --rwdFromcbLRate_mult=5" "1.5"


  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3 --acThrMult=2.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3.5 --acThrMult=2.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=4 --acThrMult=2.5"

  # worsens EBL because CB works worse
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3 --acThrMult=2.5 --cbInitShiftSz=0.35"

  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3 --acThrMult=2.5 --cbInitShiftSz=0.15"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=2 --acThrMult=2.5"

  #fromLambda_sweepPower " --rwdFromcbLRate_mult=1.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=2.5"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=3"
  #fromLambda_sweepPower " --rwdFromcbLRate_mult=2"


  #fromLambda_sweepPower " --perfRewardSize=3"
  #fromLambda_sweepPower " --perfRewardSize=2.75"
    #fromLambda_sweepPower " --perfRewardSize=2.5"
  #fromLambda_sweepPower " --perfRewardSize=2.25"
  #fromLambda_sweepPower " --perfRewardSize=2"
  #fromLambda_sweepPower " --perfRewardSize=1"

  #desiredRwd=4
  #fromLambda_sweepPower " --perfRewardSize=3"
  #fromLambda_sweepPower " --perfRewardSize=2.5"
  #fromLambda_sweepPower " --perfRewardSize=2"
  #fromLambda_sweepPower " --perfRewardSize=1"
  #fromLambda_sweepPower " --perfRewardSize=1"

  #desiredRwd=5
  #fromLambda_sweepPower " --perfRewardSize=3"
  #fromLambda_sweepPower " --perfRewardSize=2.5"
  #fromLambda_sweepPower " --perfRewardSize=2"
  #fromLambda_sweepPower " --perfRewardSize=1"

  #gradedRwd_sweepCoef " --rwdGradePower=0.63 --wmmaxFP=0.7"

  #gradedRwd_sweepPowerRcoef " --wmmaxFP=0.5"
  #gradedRwd_sweepPowerRcoef " --wmmaxFP=0.7"
  #gradedRwd_sweepPowerRcoef " --wmmaxFP=0.3"

  #ao=" --Rpre_coef=0.6$addOptions"

  #addOptionsLoc=" --rwdGradePower=0.5$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=1$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=1.5$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=2$ao"
  #fullSim

  #  ao=" --Rpre_coef=0.6 --wmmaxFP=0.7$addOptions"

  #  addOptionsLoc=" --rwdGradePower=0.5$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.6$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.63$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.65$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.7$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.8$ao"
  #  fullSim

  ##########################
  ao=" --Rpre_coef=0.4 --wmmaxFP=0.8$addOptions"

  #addOptionsLoc=" --rwdGradePower=0.5$ao"
  #fullSim

  #  addOptionsLoc=" --rwdGradePower=0.6$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.63$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.65$ao"
  #  fullSim

  #  addOptionsLoc=" --rwdGradePower=0.67$ao"
  #  fullSim

  #addOptionsLoc=" --rwdGradePower=0.8$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=0.9$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=1$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=1.5$ao"
  #fullSim

  #addOptionsLoc=" --rwdGradePower=2$ao"
  #fullSim

  ##########################
    #ao=" --Rpre_coef=0.2$addOptions"

    #addOptionsLoc=" --rwdGradePower=0.5$ao"
    #fullSim

    #addOptionsLoc=" --rwdGradePower=1$ao"
    #fullSim

    #addOptionsLoc=" --rwdGradePower=1.5$ao"
    #fullSim

    #addOptionsLoc=" --rwdGradePower=2$ao"
    #fullSim

  ##########################
  addOptions=" --ini=testExp_perfErrDif.ini"

  #  addOptionsLoc=" --rewardSize=3 --perfGradePower=0.6$addOptions"
  #  fullSim                          

  #  addOptionsLoc=" --rewardSize=3 --perfGradePower=0.7$addOptions"
  #  fullSim                          

  #  addOptionsLoc=" --rewardSize=3 --perfGradePower=0.8$addOptions"
  #  fullSim                          

  #  addOptionsLoc=" --rewardSize=3 --perfGradePower=0.9$addOptions"
  #  fullSim                          

  #addOptionsLoc=" --rewardSize=3 --perfGradePower=0.5$addOptions"
  #fullSim                          
  #                                 
  #addOptionsLoc=" --rewardSize=3 --perfGradePower=1$addOptions"
  #fullSim                          
  #                                 
  #addOptionsLoc=" --rewardSize=3 --perfGradePower=1.5$addOptions"
  #fullSim                          
  #                                 
  #addOptionsLoc=" --rewardSize=3 --perfGradePower=2$addOptions"
  #fullSim

  # if perfFromAC = 1 and perfBasedReward=1
  #addOptionsLoc=" $addOptions --perfRwdMult=9"
  #fullSim

  ############################################

  #perfErrDifNorm " --perfGradePower=0.9"

  #  absRwd="1.5"
  #  perfFromLambda " --rewardSize=$absRwd --setRPre0=$absRwd --setRPre1=$absRwd --perfRewardSize=$absRwd"
  #  absRwd="1.5"
  #  perfFromLambda " --rewardSize=$absRwd --setRPre0=$absRwd --setRPre1=$absRwd"

  #  perfFromLambda " --absRewardOn=0 --setRPre0=0 --setRPre1=0"

  #exit 1

  ##

  #addOptionsLoc=" --perfGradePower=0.2$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.3$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.4$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.5$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.6$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.7$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.8$addOptions"
  #fullSim           

  #addOptionsLoc=" --perfGradePower=0.9$addOptions"
  #fullSim                          

  #addOptionsLoc=" --perfGradePower=1.2$addOptions"
  #fullSim                          

  #addOptionsLoc=" --perfGradePower=1.5$addOptions"
  #fullSim                          

  #addOptionsLoc=" --perfGradePower=1.8$addOptions"
  #fullSim                          

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
