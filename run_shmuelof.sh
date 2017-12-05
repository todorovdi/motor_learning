pdfdir=output_for_paper
execname="pert_prl"
#execname="pert"
runfile="./$execname"
plotfile="universal_plot.py"
experimentName=shmuelof
ini="shmuAC.ini"

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

  if [ $runBE == '1' ];then
    addOptionsLoc="$onlyBE$ao"
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    testBE=$pdfSuffix
  fi

  if [ $runBEVE == '1' ];then
    addOptionsLoc="$ao"
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    testBEVE=$pdfSuffix
  fi

  python "$plotfile" "$testBEVE" "$testBE" "---plotfname=shmuTest_${1}"
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
  #onlyBE=" --cueList2=0:14:1|0:66:0 --errHistReset2=1"
  #onlyBE=" --cueList2=0:80:0"
  onlyBE=" --learn_cb2=0"

  addOptions="" 

  addOptionsLoc="$addOptions"
  #useOldData="1"
  useOldData=$2

  nsess=$1

  #runall=1

  runBE=1
  runBEVE=1
  runLong=1
  numPhasesTest=6

  ###########################

  addOptions=" --ini=$ini"

  #doSim  " --finalNoiseAmpl=0.008"
  # maybe best
  doSim  " --finalNoiseAmpl=0.008 --cbMotVarEst=0.008"
  #doSim  " --finalNoiseAmpl=0.008 --wmmax=0.25"
  # also quite ok
  #doSim  " --finalNoiseAmpl=0.008 --acOptimalRateMult=0.4"

  ##################################################
  ##################################################
  ##################################################
  ##################################################

  #doTest " --rewardSize=1.9 --perfRewardSize=1 --acLowThrMult=2 --acThrMult=1 --finalNoiseAmpl=0.008 --rewardDist=0.031 --debug_printAC=1"

  #doTest " --acLowThrMult=4 --acThrMult=8 --linearArm=1"
  #doTest " --acLowThrMult=2 --acThrMult=5 --perfRewardSize=0.65 --rewardSize=2 --wmmaxFP=0.1"
  #doTest " --acLowThrMult=2 --acThrMult=5 --perfRewardSize=0.65 --rewardSize=2 --wmmaxFP=0.2"
  #doTest " --acLowThrMult=2 --acThrMult=5 --perfRewardSize=0.65 --rewardSize=2 --wmmaxFP=0.3"
  #doTest " --acLowThrMult=3 --acThrMult=5 --perfRewardSize=1 --rewardSize=3"
  #doTest " --acLowThrMult=3 --acThrMult=5 --perfRewardSize=0.65 --rewardSize=2"
  #doTest " --acLowThrMult=4 --acThrMult=8"
  #doTest " --acLowThrMult=4 --acThrMult=8"
  #doTest " --acLowThrMult=4 --acThrMult=8 --criticExact=1"

  # BEVE loses correction in ASYMP too often
  #doTest " --rewardSize=2 --perfRewardSize=1"

  # bad
  #doTest " --acThrMult=4"
  #doTest " --cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3"
  #doTest " --perfRewardSize=0.65"

  # it works ok, well, not quite
  #doTest " --cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3 --perfRewardSize=0.65"


  #Bad
  #doSim  ""
  #doSim  " --finalNoiseAmpl=0.008 --wmmax=0.2"

  #doTest " --cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3 --perfRewardSize=0.65 --rewardSize=1.5"
  #doTest " --cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3 --perfRewardSize=0.45 --rewardSize=1.5"
  #doTest " --cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3 --perfRewardSize=0.45 --rewardSize=1.3"
  #doSim  " --rewardSize=2 --perfRewardSize=1"

  maxFP="0.5"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
