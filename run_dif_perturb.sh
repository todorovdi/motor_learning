

# $1 -additional args, $2 bg state, $3 cb state
genPdfSuffix()
{
  pdfSuffix="${experimentName}_${1}_bg${2}_cb${3}"
}
            
#when both systems are one
genPdfSuffixSimple()    
{
  pdfSuffix="${experimentName}_${1}"
}

runSimulation()
{
  if [ $5 == "0" ] || [ $5 == "2" ]; then
    $runfile --nsessions=$4 --learn_bg=$2 --learn_cb=$3 --pdfSuffix="$pdfSuffix" $1 --seed=$seed
  fi
}

runSimulationSimple()
{
  if [ $3 == "0" ] || [ $3 == "2" ]; then
    $runfile --nsessions=$2 --pdfSuffix="$pdfSuffix" $1 --seed=$seed
  fi
}

rmOldData()
{
  if [ $1 == "0" ] || [ $1 == "3" ]; then
    tt="${calc_dir}/${pdfSuffix}_seed_"
    echo ".sh deleting $tt"
    #rm "$tt"
    rm "$tt"*.dat
  fi
}

# $1 -additional args, $2 bg state, $3 cb state, $4 - nsessions 
perturb()
{
  uod=$useOldData
  if [ $# -eq 5 ]; then
    uod=$5
  fi

  genPdfSuffix "$1" "$2" "$3"
  rmOldData $uod

  echo ".sh pdfSuffix ="$pdfSuffix
  runSimulation  "$1" "$2" "$3" "$4" $uod
  if [ $uod -ne 9 ] && [ $uod -ne 3 ] && [ $uod -ne 4 ]; then
    python "$plotfile" "$pdfSuffix"
  fi
  
  ./beep.sh
}

# $1 -additional args, $2 - nsession, $3 useOldData
perturbSimple()
{
  uod=$useOldData
  if [ $# -eq 3 ]; then
    uod=$3
  fi
  genPdfSuffixSimple "$1" 
  rmOldData $uod

  echo ".sh pdfSuffix ="$pdfSuffix
  runSimulationSimple  "$1" "$2" $uod
  if [ $uod -ne 9 ] && [ $uod -ne 3 ] && [ $uod -ne 4 ]; then
    python "$plotfile" "$pdfSuffix"
  fi
  
  ./beep.sh
}

perturbAllConfig()
{
  args="$2"
  args_mult1=""
  args_mult2=""
  args_mult3=""
  if [ $bg_on_cb_on -eq 1 ]; then
    perturb "$args" 1 1 $1
    #args_mult1="${experimentName}_${args}_bg1_cb1"
    args_mult1="$pdfSuffix"
  fi
  if [ $bg_off_cb_on -eq 1 ]; then
    perturb "$args" 0 1 $1
    #perturb "$args" 0 1 1
    args_mult2="${args}_bg0_cb1"
    args_mult2="$pdfSuffix"
  fi
  if [ $bg_on_cb_off -eq 1 ]; then
    perturb "$args" 1 0 $1
    args_mult3="${args}_bg1_cb0"
    args_mult3="$pdfSuffix"
  fi

  python "$plotfile" "$args_mult1" "$args_mult2" "$args_mult3"
}

if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
  exit 1
fi

if [ $# -eq 1 ]; then
  echo "Please tell, to calc, erasing existing dat files (0), to plot existing data (1), or to add more sessions (2), or to generate only multi plots (9), or to recalibrate arm (10)"
  echo "One can also calc without plotting, erasing existing dat files (3), or to do the same but keeping existing dat files (4)"
  exit 1
fi

if [ $2 == "10" ]; then
  make pert
  ./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1
  exit 0
fi

mkdir -p $pdfdir
mkdir -p $pdfdir/single_sess
calc_dir=$HOME/tmp_out_calc 
mkdir -p $calc_dir

useOldData="1"  #by default we use old data
if [ $# -ge 2 ]; then
  useOldData=$2
fi

if [ $useOldData == "1" ]; then
  echo "#####################  Plotting without recalc"
fi

if [ $useOldData == "2" ]; then
  echo "#####################  Adding sessions to plot"
fi

if [ $useOldData == "3" ]; then
  echo "#####################  Only generating data and final/multi plots, replace existing"
fi

if [ $useOldData == "4" ]; then
  echo "#####################  Only generating data and final/multi plots, add to existing"
fi

if [ $useOldData == "9" ]; then
  echo "#####################  Only generating pdfPrefixes to use for final/multi plots"
fi

if [ $useOldData == "0" ]; then
  make $runfile
fi

#perturbAllConfig $1 "--endpoint_rotation1=1 --cbLRate=4 --trainCBEveryTrial=0 --retrainCB_useCurW=0 --dirShift=90" 
#perturbAllConfig $1 "--endpoint_rotation1=1 --cbLRate=4 --dirShift=90" 
#perturbAllConfig $1 "--endpoint_rotation1=1 --retrainCB_useCurW=0 --cbLRate=4 --dirShift=90" 

#perturbAllConfig $1 "--rotateErr=90." 
#perturbAllConfig $1 "--endpoint_rotation1=1 --dirShift=90" 

#perturbAllConfig $1 "--endpoint_xreverse1=1" 
#perturbAllConfig $1 "--xreverseErr=1" 

#perturbAllConfig $1 "--action_change1=1 --cue_change1=1" 
#perturbAllConfig $1 "--action_change1=1 --cue_change1=1 --retrainCB_useCurW=0" 

#perturbAllConfig $1 "--endpoint_rotation1=1 --dirShift=90 --cbLRate=4" 
#perturbAllConfig $1 "--endpoint_xreverse1=1" 
#perturbAllConfig $1 "--endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=90 --target_rotation1=1 --updateCBStateDist=10" 
#perturbAllConfig $1 "--endpoint_xreverse1=1 --cue_change1=1 --action_change1=1 --dirShift=180 --target_xreverse1=1 --updateCBStateDist=10" 

###############
#perturbAllConfig $1 "--endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=90 --target_rotation1=1  cbRateDepr=0.05 cbLRateUpdSpd=0.1"
#perturbAllConfig $1 "--percept_rot1=1 --dirShift=90" 

#p="--cbLRate=2 --cbRateDepr=0.03 --cbLRateUpdSpdUp=0.8 --cbLRateUpdSpdDown=1  --cbLRateUpdSpdMax=3 --Q=0.05" 

#p="--Q=0.05" 
#ud="--updateCBStateDist=0.12"

#perturbAllConfig $1 "--percept_xshift1=-0.25 --fake_prelearn=0 --numTrialsPrelearn=1400 $p $ud" 

#perturbAllConfig $1 "--actcue_change1=1 --dirShift=30 $p $ud" 
#perturbAllConfig $1 "--actcue_change1=1 --dirShift=90 $p $ud" 
#perturbAllConfig $1 "--percept_xshift1=-0.1 --percept_yshift1=0.1 --fake_prelearn=0 --numTrialsPrelearn=1400 $p $ud" 
#perturbAllConfig $1 "--percept_rot1=1 $p --dirShift=90 $ud" 
#perturbAllConfig $1 "--percept_rot1=1 $p --dirShift=30 $ud" 
#perturbAllConfig $1 "--percept_xrev1=1 $p              $ud" 
#perturbAllConfig $1 "--force_field1=-3.0 --targetPre1=90 $p" 

#bg_on_cb_on=1
#bg_off_cb_on=1
#bg_on_cb_off=1

#ud="--updateCBStateDist=0.06"

#perturbAllConfig $1 "--action_change1=1 --cue_change1=1 --dirShift=90 $p $ud" 
#perturbAllConfig $1 "--action_change1=1 --cue_change1=1 --dirShift=30 $p $ud" 
#perturbAllConfig $1 "--percept_xshift1=-0.1 --percept_yshift1=0.1 --fake_prelearn=0 --numTrialsPrelearn=1400 $p $ud" 
#perturbAllConfig $1 "--percept_xrev=1 $p  $ud" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=90 $ud" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=30 $ud" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=30 $ud" 
#perturbAllConfig $1 "--force_field1=-3.0 --targetPre1=90 $p $ud" 

#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=80 $ud" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=70 $ud" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=50 $ud" 

#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=80 $ud --rewardDist=0.05" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=70 $ud --rewardDist=0.05" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=50 $ud --rewardDist=0.05" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=30 $ud --rewardDist=0.05" 

#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=80 $ud --rewardDist=0.03" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=70 $ud --rewardDist=0.03" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=50 $ud --rewardDist=0.03" 
#perturbAllConfig $1 "--percept_rot=1 $p --dirShift=30 $ud --rewardDist=0.03" 

#perturbAllConfig $1 "--actcue_change1=1 $p --dirShift=30 $ud --rewardDist=0.03" 
#perturbAllConfig $1 "--actcue_change1=1 $p --dirShift=30 $ud --sector_reward=1" 

#perturbAllConfig $1 "--endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1" 
###############



#perturbAllConfig $1 "--endpoint_rotation1=1 --retrainCB_useCurW=0 --dirShift=90" 
#perturbAllConfig $1 "--endpoint_xreverse1=1 --retrainCB_useCurW=0" 

#perturbAllConfig $1 "--endpoint_xreverse1=1 --cue_change1=1 --action_change1=1 --dirShift=180 --target_xreverse1=1 --retrainCB_useCurW=0" 
#perturbAllConfig $1 "--target_rotation1=1" 
#perturbAllConfig $1 "--force_field1=-2.0 --targetPre1=90"    # force field perturbation just does not work correctly for other target angles
