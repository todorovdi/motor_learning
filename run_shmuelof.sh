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
  if [ $runBEVE == '1' ];then
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    argsCBon=$pdfSuffix
  fi

  if [ $runBE == '1' ];then
    perturbSimple "$onlyBE$addOptionsLoc" $nsess $useOldData  
    argsCBoff=$pdfSuffix
  fi

  addOptionsLoc="$addTrials$ao"
  if [ $runLong == '1' ];then

    if [ $runBEVE == '1' ];then
      perturbSimple "$addOptionsLoc" $nsess $useOldData
      argsCBonLong=$pdfSuffix
    fi

    if [ $runBE == '1' ];then
    perturbSimple "$onlyBE$addOptionsLoc" $nsess $useOldData 
    argsCBoffLong=$pdfSuffix
    fi
  fi

  python "$plotfile" "$argsCBon" "$argsCBoff" "$argsCBonLong" "$argsCBoffLong" \
    "---plotfname=shmu_$1"
}

doTest()
{
  #ao="$addOptions --numPhases=4"$1
  ao="$addOptions --numPhases=$numPhasesTest"$1

  test2=""
  #addOptionsLoc=" --learn_cb1=0 --setRPre1=3"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #test2=$pdfSuffix

  addOptionsLoc="$onlyBE$ao"
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  testBE=$pdfSuffix

  addOptionsLoc="$ao"
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  testBEVE=$pdfSuffix

  python "$plotfile" "$testBEVE" "$testBE" "---plotfname=shmuTest_${1}_$addOptions"
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
  #onlyBE=" --learn_cb2=0 --errHistReset2=1"
  onlyBE=" --cueList2=0:14:1|0:66:0 --errHistReset2=1"

  addOptions="" 

  addOptionsLoc="$addOptions"
  #useOldData="1"
  useOldData=$2

  nsess=$1

  #runall=1

  runBE=1
  runBEVE=1
  runLong=0
  numPhasesTest=4

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

  #addOptions=" ----errHistReset2=1ini=shmuAC.ini --cbLRateUpdSpdDown=1.5 --absRwdSz2=5"
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


  #addOptions=" --ini=shmuAC.ini --numTrials0=0 --numTrials1=1"
  #doTest " --perfRewardSize=2     --rewardSize=1"

  addOptions=" --ini=shmuAC.ini --wmmaxFP=0.5"

  #if [ $? -eq 0 ]

  #doTest " --perfRewardSize=3 --rewardSize=1"
  #doTest " --perfRewardSize=3 --rewardSize=2"
  #doTest " --perfRewardSize=3 --rewardSize=3"
  #doTest " --perfRewardSize=3 --rewardSize=4"

  #doTest " --perfRewardSize=2 --rewardSize=1"
  #doTest " --perfRewardSize=2 --rewardSize=2"
  #doTest " --perfRewardSize=2 --rewardSize=3"
  #doTest " --perfRewardSize=2 --rewardSize=4"

  doTest " --perfRewardSize=1 --rewardSize=1"
  doTest " --perfRewardSize=1 --rewardSize=2"

  doTest " --perfRewardSize=0.75 --rewardSize=1"
  doTest " --perfRewardSize=0.75 --rewardSize=2"
    
  doTest " --perfRewardSize=0.5 --rewardSize=1"
  doTest " --perfRewardSize=0.5 --rewardSize=2"

  maxFP="0.5"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
