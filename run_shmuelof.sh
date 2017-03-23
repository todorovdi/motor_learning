execname="pert_prl"
runfile="./$execname"
plotfile="universal.plot.py"
pdfdir=output_for_paper
experimentName=shmuelof
ini="$experimentName.ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed


. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

if [ $# -ne 0 ]; then
  args1=""
  args2=""
  args3=""
  args4=""
  args5=""
   
  addTrials="--numTrials4=100"

#  onlyBE="--learn_cb2=0 --resetCBState2=1"
#  addOptionsLoc="--wmmax_fake_prelearn=0.1 --rewardDist=0.05 --cbRateDepr=0.03 "$addOptions

#  #$useOldData="1"
#  perturb "--ini=$ini $addOptionsLoc " 1 1 $1 
#  argsNoRpre=$pdfSuffix
#
#  #$useOldData=$2
#  
#  perturb "--ini=$ini $onlyBE  $addOptionsLoc" 1 1 $1 
#  argsRpre=$pdfSuffix
#
#  python "$plotfile" "$argsNoRpre" "$argsRpre"
#
#  #$useOldData="1"
#  perturb "--ini=shmuelof_ext.ini $addOptionsLoc" 1 1 $1 
#  argsNoRpreLong=$pdfSuffix
#
#  #$useOldData=$2
#  perturb "--ini=shmuelof_ext.ini $onlyBE  $addOptionsLoc" 1 1 $1 
#  argsRpreLong=$pdfSuffix
#
#  python "$plotfile" "$argsNoRpreLong" "$argsRpreLong"

  #addOptionsLoc="--fake_prelearn_tempWAmpl=1."

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi

  onlyBE="--learn_cb2=0"

  addOptions=" --wmmax_fake_prelearn=0.1 --cbRateDepr=0.03 " 
  addOptionsLoc=""$addOptions

  #useOldData="1"
  useOldData=$2
  perturb "--ini=$ini $addOptionsLoc" 1 1 $1 $useOldData
  #genPdfSuffix "--ini=$ini $addOptionsLoc" 1 1 $1
  argsCBon=$pdfSuffix

  #useOldData=$2

  addOptionsLoc="--rewardDist=0.06"$addOptions
  perturb "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
  #genPdfSuffix "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
  args1=$pdfSuffix

#  addOptionsLoc="--resetRPre0=0"$addOptions
#  perturb "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
#  args2=$pdfSuffix
#
#  addOptionsLoc="--fake_prelearn_tempWAmpl=0"$addOptions
#  perturb "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
#  args3=$pdfSuffix
#
#  addOptionsLoc="--fake_prelearn_tempWAmpl=1."$addOptions
#  perturb "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
#  args4=$pdfSuffix
#
#  addOptionsLoc="--rewardDist=0.08"$addOptions
#  perturb "--ini=shmuelof.ini $onlyBE $addOptionsLoc" 1 1 $1 
#  args5=$pdfSuffix

#  python "$plotfile" "$argsCBon" "$args1" "$args2" "$args3" "$args4" "$args5"
  python "$plotfile" "$argsCBon" "$args1" 
#  python "$plotfile" "$argsCBon" "$args5" 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
