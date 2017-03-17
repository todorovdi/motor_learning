runfile="./pert_prl"
#runfile="./pert"
plotfile= "pert.plot.py"

perturb()
{
  pdfSuffix="${1}_bg${2}_cb${3}"
  echo $pdfSuffix
  if [ $useOldData == "0" ]; then
    $runfile --nsessions=$4 --learn_bg=$2 --learn_cb=$3 --pdfSuffix="$pdfSuffix" $1 --seed=$seed
  fi
  python "$plotfile" "$pdfSuffix"
  #rm -f $calc_dir/*.dat
}

perturbAllConfig()
{
  args="$2"
  args_mult1=""
  args_mult2=""
  args_mult3=""
  if [ $bg_on_cb_on -eq 1 ]; then
    perturb "$args" 1 1 $1
    args_mult1="${args}_bg1_cb1"
  fi
  if [ $bg_off_cb_on -eq 1 ]; then
    perturb "$args" 0 1 $1
    #perturb "$args" 0 1 1
    args_mult2="${args}_bg0_cb1"
  fi
  if [ $bg_on_cb_off -eq 1 ]; then
    perturb "$args" 1 0 $1
    args_mult3="${args}_bg1_cb0"
  fi

  python pert.plot.py "$args_mult1" "$args_mult2" "$args_mult3"
}

if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
else

  mkdir -p output_pert
  mkdir -p output_pert/single_sess
  calc_dir=$HOME/tmp_out_calc 
  mkdir -p $calc_dir

  useOldData="1"  #by default we use old data
  if [ $# -ge 2 ]; then
    useOldData=$2
  fi

  if [ $useOldData == "0" ]; then
    rm -f output_pert/*.dat
    rm -f $calc_dir/*.dat
  else
    echo "Plotting without recalc"
  fi

  bg_on_cb_on=0
  bg_off_cb_on=1
  bg_on_cb_off=0

  seed=0     #makes <more or less> random seed

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
  p="--Q=0.05" 
  ud="--updateCBStateDist=0.12"

  #perturbAllConfig $1 "--percept_xshift1=-0.25 --fake_prelearn=0 --numTrialsPrelearn=1400 $p $ud" 

  #perturbAllConfig $1 "--actcue_change1=1 --dirShift=30 $p $ud" 
  #perturbAllConfig $1 "--actcue_change1=1 --dirShift=90 $p $ud" 
  #perturbAllConfig $1 "--percept_xshift1=-0.1 --percept_yshift1=0.1 --fake_prelearn=0 --numTrialsPrelearn=1400 $p $ud" 
  #perturbAllConfig $1 "--percept_rot1=1 $p --dirShift=90 $ud" 
  #perturbAllConfig $1 "--percept_rot1=1 $p --dirShift=30 $ud" 
  #perturbAllConfig $1 "--percept_xrev1=1 $p              $ud" 
  #perturbAllConfig $1 "--force_field1=-3.0 --targetPre1=90 $p" 

  bg_on_cb_on=1
  bg_off_cb_on=1
  bg_on_cb_off=1

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
  perturbAllConfig $1 "--actcue_change1=1 $p --dirShift=30 $ud --sector_reward=1" 

  #perturbAllConfig $1 "--endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1" 
  ###############



  #perturbAllConfig $1 "--endpoint_rotation1=1 --retrainCB_useCurW=0 --dirShift=90" 
  #perturbAllConfig $1 "--endpoint_xreverse1=1 --retrainCB_useCurW=0" 

  #perturbAllConfig $1 "--endpoint_xreverse1=1 --cue_change1=1 --action_change1=1 --dirShift=180 --target_xreverse1=1 --retrainCB_useCurW=0" 
  #perturbAllConfig $1 "--target_rotation1=1" 
  #perturbAllConfig $1 "--force_field1=-2.0 --targetPre1=90"    # force field perturbation just does not work correctly for other target angles
fi
