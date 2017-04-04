runfile="./pert_prl"
plotfile="perfCBOnly_plot.py"
# plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=perfCBOnly
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

#make pert
#./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1

if [ $# -ne 0 ]; then

  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
    
  #./inv `cat tmp` -n 1000 -T 1.0 -N 100 -minAngDeg 10 -maxAngDeg 170

  # default
  #./inv `cat tmp` -n 1000 -T 1.0 -N 100 -minAngDeg 0 -maxAngDeg 360 

  # n is timesteps
  # T is time
  # N is number of actions
  # minAngDeg  is start of the sector
  # maxAngDeg  is end of the sector

  echo "Starting experiment "$experimentName

  addOptions=" --ini=$ini"
  addOptionsLoc=""$addOptions
  useOldData=$2   # 9 means don't plot

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--force_field1=-2. --cue0=0 --cue1=0 --cue2=0"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_force_field=$pdfSuffix

  tgt=" --defTgt0=0 --defTgt1=80 --defTgt2=0"
  addOptionsLoc="--percept_rot1=80. --cue0=1 --cue1=2 --cue2=1"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--percept_xrev1=1 --cue0=0 --cue1=0 --cue2=0"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_xrev=$pdfSuffix

  tgt=" --defTgt0=0 --defTgt1=45 --defTgt2=0"
  addOptionsLoc="--percept_rot1=45. --cue0=1 --cue1=4 --cue2=1"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot=$pdfSuffix

  python "$plotfile" "$args_force_field" "$args_percept_rot" "$args_percept_xrev" "$args_percept_small_rot"

  addOptions=" --ini=$ini --cbRateDepr=0."
  addOptionsLoc=""$addOptions
  useOldData=$2   # 9 means don't plot

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--force_field1=-2. --cue0=0 --cue1=0 --cue2=0"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_force_field=$pdfSuffix

  tgt=" --defTgt0=0 --defTgt1=80 --defTgt2=0"
  addOptionsLoc="--percept_rot1=80. --cue0=1 --cue1=2 --cue2=1"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--percept_xrev1=1 --cue0=0 --cue1=0 --cue2=0"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_xrev=$pdfSuffix

  tgt=" --defTgt0=0 --defTgt1=45 --defTgt2=0"
  addOptionsLoc="--percept_rot1=45. --cue0=1 --cue1=4 --cue2=1"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot=$pdfSuffix

  python "$plotfile" "$args_force_field" "$args_percept_rot" "$args_percept_xrev" "$args_percept_small_rot"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
