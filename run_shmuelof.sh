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

  #$useOldData="1"
  perturb "--ini=$ini $addOptions --wmmax_fake_prelearn=0.1" 1 1 $1 
  argsNoRpre=$pdfSuffix

  perturb "--ini=$ini $addOptions --wmmax_fake_prelearn=0.1 $addTrials" 1 1 $1 
  argsNoRpreLong=$pdfSuffix

  $useOldData=$2
  onlyBE="--learn_cb2=0 --wmmax_fake_prelearn=0.1"
  perturb "--ini=$ini $onlyBE  $addOptions" 1 1 $1 
  argsRpre=$pdfSuffix

  onlyBE="--learn_cb2=0 --wmmax_fake_prelearn=0.1 $addTrials"
  perturb "--ini=$ini $onlyBE  $addOptions" 1 1 $1 
  argsRpreLong=$pdfSuffix


  #testing variations of parameters
#  perturb "--ini=$ini $onlyBE --rewardDist=0.05  $addOptions" 1 1 $1 
#  args3=$pdfSuffix
#
#  perturb "--ini=$ini $onlyBE --inhy=1.6 $addOptions" 1 1 $1 
#  args4=$pdfSuffix
#
#  perturb "--ini=$ini $onlyBE --lam2=0.17 $addOptions" 1 1 $1 
#  args5=$pdfSuffix

  python "$plotfile" "$argsNoRpre" "$argsRpre"
  python "$plotfile" "$argsNoRpreLong" "$argsRpreLong"
  #python "$plotfile" "$args1" "$args2" "$args3" "$args4" "$args5"
fi

#$runfile --nsessions=2 --ini=shmuelof.ini --learn_cb2=0 --resetCBState2=1 --seed=2 --pdfSuffix="shmuelof_--ini=shmuelof.ini --learn_cb2=0 --resetCBState2=1_bg1_cb1" --learn_cb=1 --learn_bg=1
