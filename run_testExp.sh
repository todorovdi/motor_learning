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
  args_nonEB_control=$pdfSuffix

  # don't turn off CB too early
  #perturbSimple " --endpoint_rotation1=30 --endpoint_rotation2=30 --setRPre1=0 --cue1=0 --numTrials1=60 --numTrials2=80 --learn_cb2=0$addOptionsLoc" $nsess  $useOldData
  #args_enableBG=$pdfSuffix

  python "$plotfile" "$args_EB" "$args_nonEB_control" "$args_enableBG"
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

  #addOptions=" --ini=$ini --rwdGradePower=0.7 --cbLRate=1. --lam2=0.18 --lam1=0.18"
  #addOptions=" --ini=$ini"

  #addOptionsLoc=" --perfRwdMult=5$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=7$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=10$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=15$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=20$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=40$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=60$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=80$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=100$addOptions"
  #fullSim

  #######################################

  addOptions=" --ini=$ini --cbLRate=0.8"
  #addOptions=" --ini=$ini --perfRwdErrChange_threshold=0.001"
  #addOptions=" --ini=$ini --setRPre0=0.1 --setRPre1=0. --setRPre2=0.1 --perfRwdErrChange_threshold=0.001"

  addOptionsLoc=" $addOptions --perfRwdMult=9 --wmmaxFP=0.5"
  fullSim

  #addOptionsLoc=" $addOptions --perfRwdMult=8 --wmmaxFP=0.4"
  #fullSim

  #addOptionsLoc=" $addOptions --perfRwdMult=8 --wmmaxFP=0.5"
  #fullSim

  #addOptionsLoc=" $addOptions --perfRwdMult=10 --wmmaxFP=0.4"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=0.01$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=0.05$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=0.1$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=0.3$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=1$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=5$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=10$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=20$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=8$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=15$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=25$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=30$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=35$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=5$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=7$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=10$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=15$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=20$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=40$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=60$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=80$addOptions"
  #fullSim

  #addOptionsLoc=" --perfRwdMult=100$addOptions"
  #fullSim

  ########################################

  #addOptionsLoc=" --rewardSize=0.5$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=1.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=5.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=10.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=20.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=40.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=50.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=60.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=70.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=80.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=90.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=100.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=110.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=120.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=130.$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=150$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=180$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=150 --lam2=1.8$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=180 --lam2=1.6$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=120 --lam2=1.8$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=130 --lam2=1.8$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=120 --lam2=1.6$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=130 --lam2=1.6$addOptions"
  #fullSim

  #addOptionsLoc=" --rewardSize=110$addOptions"
  #fullSim


###
# fdfdsfd  ()   []  fdsfdsfdname = 'fds'   d 

  #addOptionsLoc=" --cbLRateUpdTwoErrThreshold=1$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdTwoErrThreshold=1 --perfRwdMult=12$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdTwoErrThreshold=1 --perfRwdMult=16$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdTwoErrThreshold=1 --perfRwdMult=20$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdTwoErrThreshold=1 --perfRwdMult=24$addOptions"
  #fullSim

#############################

  #addOptionsLoc=" --cbLRateUpdSpdUp=2.5 --cbLRateUpdAbsErr_threshold=0.$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdSpdUp=3 --cbLRateUpdAbsErr_threshold=0.$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdSpdDown=1.9 --cbLRateUpdAbsErr_threshold=0.$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdSpdDown=1.7 --cbLRateUpdAbsErr_threshold=0.$addOptions"
  #fullSim

  #addOptionsLoc=" --cbLRateUpdSpdDown=1.4 --cbLRateUpdAbsErr_threshold=0.$addOptions"
  #fullSim

###################################

  #addOptionsLoc=" --wmmaxFP=0.1$addOptions"
  #fullSim

  #addOptionsLoc=" --wmmaxFP=0.2$addOptions"
  #fullSim

  #addOptionsLoc=" --wmmaxFP=0.3$addOptions"
  #fullSim

  #addOptionsLoc=" --wmmaxFP=0.4$addOptions"
  #fullSim

  #addOptionsLoc=" --wmmaxFP=0.8$addOptions"
  #fullSim

#############################################

  
  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
