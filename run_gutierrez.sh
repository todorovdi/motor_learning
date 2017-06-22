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
  perturbSimple "--ini=$ini$addOptionsLoc" $1 $useOldData
  args_EB_control=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=1$addOptionsLoc" $1  $useOldData
  args_nonEB_control=$pdfSuffix
 
  python "$plotfile" "$args_EB_control" "$args_nonEB_control"
}

fullSim()
{
  perturbSimple "--ini=$ini$addOptionsLoc" $1 $useOldData
  args_EB_control=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=1$addOptionsLoc" $1  $useOldData
  args_nonEB_control=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=1 --HD=1$addOptionsLoc" $1 $useOldData
  args_nonEB_HD=$pdfSuffix

  perturbSimple "--ini=$ini --HD=1$addOptionsLoc" $1 $useOldData
  args_EB_HD=$pdfSuffix

  perturbSimple "--ini=$ini --PD=1$addOptionsLoc" $1 $useOldData
  args_EB_PD=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=1 --PD=1$addOptionsLoc" $1  $useOldData
  args_nonEB_PD=$pdfSuffix

  python "$plotfile" "$args_nonEB_HD" "$args_nonEB_PD" "$args_nonEB_control" "$args_EB_HD" "$args_EB_PD" "$args_EB_control" "---plotfname=guti_$addOptionsLoc" 
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

  addOptions=" acOptimalRateMult=0.2"

  addOptionsLoc=" --gradedReward=1"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.3"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.1"$addOptions
  fullSim $1

  #addOptionsLoc=" --gradedReward=0 --rewardSize=1.5 --perfRewardSize=3 --setRpre1=1.5"$addOptions
  #fullSim $1

  ############################
  addOptions=" --cbStateDepr=0.02"

  addOptionsLoc=" --gradedReward=1"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.3"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.1"$addOptions
  fullSim $1

  #addOptionsLoc=" --gradedReward=0 --rewardSize=1.5 --perfRewardSize=3 --setRpre1=1.5"$addOptions
  #fullSim $1

  addOptions=" --cbStateDepr=0.01"

  addOptionsLoc=" --gradedReward=1"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.3"$addOptions
  fullSim $1

  addOptionsLoc=" --gradedReward=1 --cbLRate=0.1"$addOptions
  fullSim $1

  #addOptionsLoc=" --gradedReward=0 --rewardSize=1.5 --perfRewardSize=3 --setRpre1=1.5"$addOptions
  #fullSim $1

  #######################

  #addOptionsLoc=" --trainCBEveryTrial=0"$addOptions
  #fullSim $1

  #addOptionsLoc=" --trainCBEveryTrial=1"$addOptions
  #fullSim $1

  ####################

  #addOptionsLoc=" --trainCBEveryTrial=1"$addOptions
  #fullSim $1

  #addOptionsLoc=" --trainCBEveryTrial=1 --perfRwdMult=15"$addOptions
  #fullSim $1

  #addOptionsLoc=" --trainCBEveryTrial=1 --perfRwdMult=12"$addOptions
  #fullSim $1

  
  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
