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
  addOptionsLoc=""$addOptions
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  argsCBon=$pdfSuffix

  addOptionsLoc=" $addTrials$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  argsCBonLong=$pdfSuffix

  addOptionsLoc=""$addOptions
  perturbSimple "--ini=shmuelof.ini$onlyBE$addOptionsLoc" $nsess $useOldData  
  args1=$pdfSuffix

  addOptionsLoc=" $addTrials$addOptions"
  perturbSimple "--ini=shmuelof.ini$onlyBE$addOptionsLoc" $nsess $useOldData 
  args1Long=$pdfSuffix

  python "$plotfile" "$argsCBon" "$args1" "$argsCBonLong" "$args1Long"
}

if [ $# -ne 0 ]; then
  args1=""
  args2=""
  args3=""
  args4=""
  args5=""
   
  addTrials="--numTrials4=100"

  delay="4.0s"
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

  addOptions=" --cbLRateUpdAbsErr_threshold=0.01"
  doSim

  addOptions=" --wmmaxFP=0.2 --cbLRateUpdAbsErr_threshold=0.01" 
  doSim

  addOptions=" --cbLRateUpdAbsErr_threshold=0.05"
  doSim

  addOptions=" --wmmaxFP=0.2 --cbLRateUpdAbsErr_threshold=0.05" 
  doSim

  # TODO: recalibrate arm to -30, 90
  # maybe will have to modify deg2action  function
  # or recalibration in arm_inv.cc

#  python "$plotfile" "$argsCBon" "$args1" "$argsCBonLong" "$args1Long" 
  # python "$plotfile" "$argsCBon" "$args1" 
  # python "$plotfile" "$argsCBonLong" "$args1Long"
#  python "$plotfile" "$argsCBon" "$args5" 


  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
