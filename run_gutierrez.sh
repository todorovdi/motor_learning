runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=gutierrez
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

#make pert
#./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1

shortSim()
{
  perturbSimple "--ini=$ini --HD=1$addOptionsLoc" $1 $useOldData
  args_EB_HD=$pdfSuffix

  perturbSimple "--ini=$ini$addOptionsLoc" $1 $useOldData
  args_EB_control=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=1$addOptionsLoc" $1  $useOldData
  args_nonEB_control=$pdfSuffix
 
  python "$plotfile" "$args_EB_HD" "$args_EB_control" "$args_nonEB_control"
}

fullSim()
{
  if [ $runNEB_control == '1' ]; then
    perturbSimple "--ini=$ini --percept_xrev1=1$addOptionsLoc" $1  $useOldData
    args_nonEB_control=$pdfSuffix
  fi

  if [ $runDoubleControl == '1' ]; then
    #  --cue2=0   is redundant but is needed to separate two controls, otherwise we overwrite them
    perturbSimple "--ini=$ini --percept_xrev1=1 --cue2=0$addOptionsLoc" $1  $useOldData
    args_nonEB_control2=$pdfSuffix
  fi

  if [ $runEB_control == '1' ]; then
    perturbSimple "--ini=$ini$addOptionsLoc" $1 $useOldData
    args_EB_control=$pdfSuffix
  fi

  if [ $runDoubleControl == '1' ]; then
    perturbSimple "--ini=$ini --cue2=0$addOptionsLoc" $1 $useOldData
    args_EB_control2=$pdfSuffix
  fi

  if [ $runEB_patients == '1' ]; then
    perturbSimple "--ini=$ini --HD=1$addOptionsLoc" $1 $useOldData
    args_EB_HD=$pdfSuffix
  fi

  if [ $runDoublePatients == '1' ]; then
    perturbSimple "--ini=$ini --PD=1$addOptionsLoc" $1 $useOldData
    args_EB_PD=$pdfSuffix
  fi

  if [ $runNEB_patients == '1' ]; then
    perturbSimple "--ini=$ini --percept_xrev1=1 --HD=1$addOptionsLoc" $1 $useOldData
    args_nonEB_HD=$pdfSuffix
  fi

  if [ $runDoublePatients == '1' ]; then
    perturbSimple "--ini=$ini --percept_xrev1=1 --PD=1$addOptionsLoc" $1  $useOldData
    args_nonEB_PD=$pdfSuffix
  fi

  if [ $2 != '' ]; then
    fname=$2
  else
    fname=$addOptionsLoc
  fi

  python "$plotfile" "$args_nonEB_HD" "$args_nonEB_PD" "$args_nonEB_control" "$args_EB_HD" "$args_EB_PD" "$args_EB_control" "$args_nonEB_control2" "$args_EB_control2" "---plotfname=guti_$fname" 
}


if [ $# -ne 0 ]; then

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
    

  useOldData=$2   # 9 means don't plot

  #./inv `cat tmp` -n 1000 -T 1.0 -N 100 -minAngDeg 10 -maxAngDeg 170

  # default
  #./inv `cat tmp` -n 1000 -T 1.0 -N 100 -minAngDeg 0 -maxAngDeg 360 

  # n is timesteps
  # T is time
  # N is number of actions
  # minAngDeg  is start of the sector
  # maxAngDeg  is end of the sector

  echo "Starting experiment "$experimentName

  #addOptions=" --cbRetrainSpeedup=0 --wmmaxFP=0.55"
  #addOptions=" --debug_printAC=1"
  addOptions=""

  mp=0.63
  #1.6 because it is the baseline stationary CB learning rate
  rpre=`echo "scale=4;$mp*1.6" | bc -l`
  #rpre=`echo "scale=4;$mp*2" | bc -l`

  runNEB_patients=1
  runEB_control=1
  runNEB_control=1
  runEB_patients=1
  runDoubleControl=1
  runDoublePatients=1


  pertDist=0.13
  distPow=0.046

  initRate=0.8
  rwdSz=18
  rwdSz=24
  # 1.5 power
  rpre=`echo "scale=4;$perfRwd*$initRate-$distPow*$rwdSz" | bc -l`
  echo "----------- computed Rpre is  $rpre"
  #rpre=0.8
  #1,1.5 contol EB dies, paitnets NEB ok
  rpre=0

  perfRwd=0.9
  # more or less ok, but slighly worse than origin. But if we want to insist on parameter consistency maybe important
  #addOptionsLoc=" --cbLRate=1"$addOptions 
  #fullSim $1 "incStartRate"

  #addOptionsLoc=" --cbLRate=1 --rewardSize=24"$addOptions 
  #fullSim $1 "incRwd"

  #addOptionsLoc=" --perfRewardSize=0.9"$addOptions 
  #fullSim $1 "incperfRwd"

  addOptionsLoc=""$addOptions 
  fullSim $1 "origin"

  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=2.1 --acThrMult=2.3 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1 "incBothThrs"                   

  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2.5 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1 "incSpdUp"                  

  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=1.8 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1 "redSpdUp"                   

  #perfRwd=1
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1 "incPerfRwd"                   

  #rwdSz=25
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1 "incAbsRwd"                  

  ########################

  # so far the best, fits all three
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    


  # clost to be ok, origin of changes
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.7 --acThrMult=1.9 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    

  #initRate=1.3
  ## more or less ok
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.7 --acThrMult=1.9 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    


  # more or less ok, but less cool for controls NEBL
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.7 --acThrMult=1.9 --cbLRateUpdSpdDown=2.8 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    


  # much worse 
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=18"
  #addOptionsLoc="   --acLowThrMult=1.5 --acThrMult=1.9 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=0.7 --cbLRate=0.8 $preset"$addOptions 
  #fullSim $1                    



################################   These two worked ok with rate resetted

  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.9 --acThrMult=2.1 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    

  #initRate=1.3
  #preset=" --setRPre0=$rpre --setRPre1=$rpre --rewardSize=$rwdSz"
  #addOptionsLoc="   --acLowThrMult=1.7 --acThrMult=1.9 --cbLRateUpdSpdDown=3.5 --cbLRateUpdSpdUp=2 --perfRewardSize=$perfRwd --cbLRate=$initRate $preset"$addOptions 
  #fullSim $1                    


###############################


  # too large perfRwd destabilise basline which kills EBC AF



  #for small learning rate we adapt too little thus have small AF in EB
  #for large learning rate we can't turn it off in NEBL controls

  
  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
