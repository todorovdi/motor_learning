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
  #--cbRateDepr=0."
  addOptionsLoc=""$addOptions
  useOldData=$2   # 9 means don't plot

  #tgt=" --defTgt0=0 --defTgt1=0 --defTgt2=0"
  #addOptionsLoc="--percept_rot1=70. --cue0=1 --cue1=7 --cue2=1"$tgt$addOptions
  #perturbSimple "$addOptionsLoc" $1 $useOldData
  #args_percept_rot=$pdfSuffix

  #tgt=" --defTgt0=0 --defTgt1=0 --defTgt2=0"
  #addOptionsLoc="--percept_rot1=70. --cue0=1 --cue1=7 --cue2=1"$tgt$addOptions
  #perturbSimple "$addOptionsLoc" $1 $useOldData
  #args_percept_rot=$pdfSuffix

  #tgt=" --defTgt0=290 --defTgt1=290 --defTgt2=290"
  #addOptionsLoc="--percept_rot1=70. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  #perturbSimple "$addOptionsLoc" $1 $useOldData
  #args_percept_rot=$pdfSuffix

  #tgt=" --defTgt0=270 --defTgt1=270 --defTgt2=270 --actPrelearnAng3=270"
  #addOptionsLoc="--percept_rot1=90. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  #perturbSimple "$addOptionsLoc" $1 $useOldData
  #args_percept_rot=$pdfSuffix
  

  angg=-90
  rott=90
  angg2=$((angg+rott))
  tgt=" --defTgt_all=$angg"
  #tgt=" --defTgt0=$angg --defTgt1=$angg --defTgt2=$angg --actPrelearnAng3=$angg --actPrelearnAng4=$angg2"
  addOptions=" --ini=$ini --numTrials1=80" 

  addOptionsLoc="--percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  addOptionsLoc=" --cbLRate = 3. --percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  addOptionsLoc=" --cbLRate = 1. --percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  addOptions=" --ini=$ini --cbRateDepr=0.0" 
  addOptionsLoc="--percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  addOptionsLoc=" --cbLRate = 3. --percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  addOptionsLoc=" --cbLRate = 1. --percept_rot1=$rott. --cue0=3 --cue1=4 --cue2=3"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  #####################################################

  tgt=" --defTgt0=-40 --defTgt1=-40 --defTgt2=-40"
  addOptionsLoc="--percept_rot1=80. --cue0=3 --cue1=4 --cue2=3 --actPrelearnAng3=-40 --actPrelearnAng4=40"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_rot=$pdfSuffix

  tgt=" --defTgt0=0 --defTgt1=0 --defTgt2=0"
  addOptionsLoc="--percept_rot1=45. --cue0=1 --cue1=2 --cue2=1"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot=$pdfSuffix

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--percept_xrev1=1 --cue0=5 --cue1=6 --cue2=5"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_xrev=$pdfSuffix

  tgt=" --defTgt0=90 --defTgt1=90 --defTgt2=90"
  addOptionsLoc="--force_field1=-2. --cue0=0 --cue1=0 --cue2=0"$tgt$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_force_field=$pdfSuffix

  python3 "$plotfile" "$args_force_field" "$args_percept_rot" "$args_percept_xrev" "$args_percept_small_rot"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
