execname="pert_prl"
runfile="./$execname"
plotfile="universal_plot.py"
experimentName=shmuelof
ini="$experimentName.ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

# make pert
# ./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1

doSim()
{
  ao="$addOptions$1"

  addOptionsLoc="$ao"

  perturbSimple "$addOptionsLoc" $nsess $useOldData
  argsCBon=$pdfSuffix

  perturbSimple "$onlyBE$addOptionsLoc" $nsess $useOldData  
  argsCBoff=$pdfSuffix

  if [ $runall == '1' ];then
    addOptionsLoc="$addTrials$ao"

    perturbSimple "$addOptionsLoc" $nsess $useOldData
    argsCBonLong=$pdfSuffix

    perturbSimple "$onlyBE$addOptionsLoc" $nsess $useOldData 
    argsCBoffLong=$pdfSuffix
  fi

  python "$plotfile" "$argsCBon" "$argsCBoff" "$argsCBonLong" "$argsCBoffLong" \
    "---plotfname=shmu_$1"
}

doTest()
{
  ao="$addOptions --numPhases=4"$1

  test2=""
  #addOptionsLoc=" --learn_cb1=0 --setRPre1=3"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #test2=$pdfSuffix

  addOptionsLoc="$onlyBE$ao"
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  test3=$pdfSuffix

  addOptionsLoc="$ao"
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  test1=$pdfSuffix

  python "$plotfile" "$test1" "$test2" "$test3" 
}

if [ $# -ne 0 ]; then
  args1=""
  args2=""
  args3=""
  args4=""
  args5=""

  delay="3.0s"
  delay="0.01s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
  echo "Starting experiment "$experimentName

  addTrials=" --numTrials4=100"
  onlyBE=" --learn_cb2=0 errHistReset2=1"

  addOptions="" 

  addOptionsLoc="$addOptions"
  #useOldData="1"
  useOldData=$2

  nsess=$1

  runall=1

  ###########################

  addOptions=" --ini=$ini"

  # works ok for runall=0
  #addOptions=" --ini=shmuAC.ini --cbLRateUpdSpdDown=1.5"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=1"

  #doSim " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=0.5"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=1"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=2"

  #addOptions=" --ini=shmuAC.ini --cbLRateUpdSpdDown=1.5 --gradedReward=0"


  #  --absRwdSz2=3

  #addOptions=" --ini=shmuAC.ini --cbLRateUpdSpdDown=1.5 --absRwdSz2=5"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=0.5 --rewardSize=1.5"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=0.5 --rewardSize=2.5"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=1   --rewardSize=3"

  #addOptions=" --ini=shmuAC.ini --cbLRateUpdSpdDown=1.5 --absRwdSz2=3"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=0.5 --rewardSize=1.5"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=0.5 --rewardSize=2.5"
  #doSim " --wmmaxFP=0.1 --perfRewardSize=1   --rewardSize=3"

  ########################################################
  ########################################################
  ########################################################

  #acOptimalRateMult = 0.6 
  #addOptions=" --ini=shmuAC.ini --cbStateDepr=0.08 --acOptimalRateMult=0.2"
  addOptions=" --ini=shmuAC.ini"
  # the best one
  # we do give rwd for performance in error clamp
  doSim " --perfRewardSize=2   --rewardSize=1"

  ########################################################
  ########################################################
  ########################################################
  ########################################################

  #doSim " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=1"

  #doSim " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=2"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=4   --rewardSize=2"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=6   --rewardSize=2"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=8   --rewardSize=2"

  #doSim " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=2"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=1"


  # somewhat worse
  #doSim " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=0.5"
  #doSim " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=1"

  ########################################################
  ########################################################
  ########################################################

  #doSim " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=2.5"

  #addOptions=" --ini=shmuAC.ini --cbLRateUpdSpdDown=1.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=0.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=0.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=0.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=0.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=0.5"
  #                     
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=1"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=1"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=1"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=1"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=1"
  #                     
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=1.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=1.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=1.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=1.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=1.5"
  #                     
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=2"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=2"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=2"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=2"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=2"
  #                     
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=2.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=2.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=2.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=2.5"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=2.5"
  #                     
  #doTest " --wmmaxFP=0.05 --perfRewardSize=0.5 --rewardSize=3"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1   --rewardSize=3"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=1.5 --rewardSize=3"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=2   --rewardSize=3"
  #doTest " --wmmaxFP=0.05 --perfRewardSize=3   --rewardSize=3"

  #doSim " --wmmaxFP=0.1 --perfRewardSize=1.3 --rewardSize=1"

  #doTest " --gradedReward=0 --wmmaxFP=0.1 --perfRewardSize=0.1 --rewardSize=2"   
  #doTest " --gradedReward=0 --wmmaxFP=0.1 --perfRewardSize=0.3 --rewardSize=2"   

  #doTest " --gradedReward=0 --wmmaxFP=0.1 --perfRewardSize=0.5 --rewardSize=1"
  #doTest " --gradedReward=0 --wmmaxFP=0.1 --perfRewardSize=1 --rewardSize=1"
  #doTest " --gradedReward=0 --wmmaxFP=0.1 --perfRewardSize=2 --rewardSize=1"

  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.1 --rewardSize=2"   
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.3 --rewardSize=2"   

  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.5 --rewardSize=1"
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=1 --rewardSize=1"
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=2 --rewardSize=1"

  #doSim " --cbStateDepr=0.018"
  #doSim " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.1 --rewardSize=2"   -- better but explore for BE only group during 45 deg -- thus wrong level in error clamp

  #doSim " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.3 --rewardSize=2"
  #doSim " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.7 --rewardSize=2"
  #doSim " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=0.3 --rewardSize=2"

  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=1 --rewardSize=2"
  #doSim " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=1 --rewardSize=2"

  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=2 --rewardSize=3"
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=3 --rewardSize=3"

  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=2 --rewardSize=2"
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=3 --rewardSize=2"
  #doTest " --gradedReward=0 --cbStateDepr=0.1 --perfRewardSize=4 --rewardSize=2"

  #addOptions=" --cbStateDepr=0.02"
  #doSim

  #addOptions=" --cbStateDepr=0.01"
  #doSim

  #addOptions=" --cbStateDepr=0.03"
  #doSim

  #addOptions=" --wmmaxFP=0.2 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.15 --vectorErrTgtBorder=0"
  #doSim


  #addOptions=" --wmmaxFP=0.04 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.05"
  #doSim

  #addOptions=" --wmmaxFP=0.05 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.06 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.06"
  #doSim

  #addOptions=" --wmmaxFP=0.07 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.08 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.09 --vectorErrTgtBorder=0"
  #doSim

  #addOptions=" --wmmaxFP=0.07"
  #doSim

  #addOptions=" --wmmaxFP=0.08"
  #doSim

  #addOptions=" --wmmaxFP=0.09"
  #doSim


  #addOptions=" --wmmaxFP=0.09"
  #doSim

  #addOptions=" --wmmaxFP=0.08"
  #doSim

  #addOptions=" --wmmaxFP=0.07"
  #doSim

  #########################################

  #doTest " --wmmaxFP=0.4"
  #doTest " --wmmaxFP=0.2"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
