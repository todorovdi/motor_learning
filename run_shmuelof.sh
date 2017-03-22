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
#  addOptions="--wmmax_fake_prelearn=0.1 --rewardDist=0.05 --cbRateDepr=0.03 "$addOptions

#  #$useOldData="1"
#  perturb "--ini=$ini $addOptions " 1 1 $1 
#  argsNoRpre=$pdfSuffix
#
#  #$useOldData=$2
#  
#  perturb "--ini=$ini $onlyBE  $addOptions" 1 1 $1 
#  argsRpre=$pdfSuffix
#
#  python "$plotfile" "$argsNoRpre" "$argsRpre"
#
#  #$useOldData="1"
#  perturb "--ini=shmuelof_ext.ini $addOptions" 1 1 $1 
#  argsNoRpreLong=$pdfSuffix
#
#  #$useOldData=$2
#  perturb "--ini=shmuelof_ext.ini $onlyBE  $addOptions" 1 1 $1 
#  argsRpreLong=$pdfSuffix
#
#  python "$plotfile" "$argsNoRpreLong" "$argsRpreLong"

  #addOptions="--fake_prelearn_tempWAmpl=1."

  onlyBE="--learn_cb2=0"
  addOptions="--wmmax_fake_prelearn=0.3 --rewardDist=0.06 --cbRateDepr=0.03 "
  perturb "--ini=shmuelof.ini $onlyBE $addOptions" 1 1 $1 
  args1=$pdfSuffix

  addOptions="--rewardDist=0.06 --cbRateDepr=0.03 "$addOptions
  perturb "--ini=shmuelof.ini $onlyBE $addOptions" 1 1 $1 
  args2=$pdfSuffix

  addOptions="--rewardDist=0.07 --cbRateDepr=0.03 "$addOptions
  perturb "--ini=shmuelof.ini $onlyBE $addOptions" 1 1 $1 
  args3=$pdfSuffix

  addOptions="--rewardDist=0.06 --cbRateDepr=0.01 "$addOptions
  perturb "--ini=shmuelof.ini $onlyBE $addOptions" 1 1 $1 
  args4=$pdfSuffix

  addOptions="--rewardDist=0.06 --cbRateDepr=0.03 --updateCBStateDist=0.6 "$addOptions
  perturb "--ini=shmuelof.ini $onlyBE $addOptions" 1 1 $1 
  args5=$pdfSuffix

  python "$plotfile" "$args1" "$args2" "$args3" "$args4" "$args5"
fi
