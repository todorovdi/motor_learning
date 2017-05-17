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
  if [ $runall == '1' ];then
    addOptionsLoc=""$addOptions
    perturbSimple "--ini=shmuelof.ini$onlyBE$addOptionsLoc" $nsess $useOldData  
    args1=$pdfSuffix

    addOptionsLoc=" $addTrials$addOptions"
    perturbSimple "--ini=shmuelof.ini$onlyBE$addOptionsLoc" $nsess $useOldData 
    args1Long=$pdfSuffix
  fi

  addOptionsLoc=""$addOptions
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  argsCBon=$pdfSuffix

  addOptionsLoc=" $addTrials$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  argsCBonLong=$pdfSuffix

  python "$plotfile" "$argsCBon" "$args1" "$argsCBonLong" "$args1Long" \
    "---plotfile=shmu_$addOptions"
}

doTest()
{
  ao=" --ini=shmuTest.ini --numPhases=3"$1

  addOptionsLoc=" --learn_cb1=0 --setRPre1=3"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  test2=$pdfSuffix

  addOptionsLoc=" --learn_cb2=0"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  test3=$pdfSuffix

  addOptionsLoc=""$ao
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
   
  addTrials="--numTrials4=100"

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
  echo "Starting experiment "$experimentName

  onlyBE=" --learn_cb2=0"

  addOptions="" 

  addOptionsLoc=""$addOptions
  #useOldData="1"
  useOldData=$2

  nsess=$1

  runall=1

  ###########################

  addOptions=" --cbRateDepr=0.018"
  doSim

  #addOptions=" --cbRateDepr=0.02"
  #doSim

  #addOptions=" --cbRateDepr=0.01"
  #doSim

  #addOptions=" --cbRateDepr=0.03"
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
