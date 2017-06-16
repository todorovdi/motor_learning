runfile="./pert"
plotfile="universal_plot.py"
# plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=reachTest
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

runtri()
{
  addOptionsLoc="--cue1=1"$1
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_cue_change=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$1
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_small_rot=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$1
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_large_rot=$pdfSuffix
}



if [ $# -ne 0 ]; then

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi

  echo "Starting experiment "$experimentName

  addOptions=" --ini=$ini"
  addOptionsLoc=""$addOptions
  useOldData=$2   # 9 means don't plot intermediate plots
  nsess=$1


  #perturbSimple "$addOptionsLoc" $nsess $useOldData

  #addOptionsLoc=" --finalNoiseAmpl=0.005"$addOptions
  #perturbSimple "$addOptionsLoc" $nsess $useOldData

  #addOptionsLoc=" --learn_bg=1 --habit2PMCdirectly=0 --nc=1 --numPhases=1 --numTrials1=300 --rewardDist=0.00001 --Rpre_coef=0.999"$addOptions
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  
  #addOptionsLoc=" --learn_bg=1 --habit2PMCdirectly=0 --nc=1 --numPhases=1 --numTrials1=300 --finalNoiseAmpl=0.005 --wmmaxFP=0 --setRPre0=0"$addOptions
  #perturbSimple "$addOptionsLoc" $nsess $useOldData

  addOptionsLoc=" --learn_bg=1 --habit2PMCdirectly=0 --nc=1 --numPhases=1 --numTrials1=300 --finalNoiseAmpl=0.005 --wmmaxFP=0 --setRPre0=0 --rewardDist=0.00001"$addOptions
  perturbSimple "$addOptionsLoc" $nsess $useOldData

  #addOptionsLoc=" --learn_bg=1 --habit2PMCdirectly=0 --nc=1 --numPhases=1 --numTrials1=300 --rewardDist=0.00001 --Rpre_coef=0.999 --finalNoiseAmpl=0.005"$addOptions
  #perturbSimple "$addOptionsLoc" $nsess $useOldData

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
