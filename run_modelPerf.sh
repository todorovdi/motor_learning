runfile="./pert_prl"
plotfile="universal_plot.py"
# plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=modelPerf
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

  # n is timesteps
  # T is time
  # N is number of actions
  # minAngDeg  is start of the sector
  # maxAngDeg  is end of the sector

  echo "Starting experiment "$experimentName

  addOptions=" --ini=$ini"
  #--cbRateDepr=0."
  addOptionsLoc=""$addOptions
  useOldData=$2   # 9 means don't plot intermediate plots


  # --ini=modelPerf.ini --learn_bg=0 --defTgt1=45 --cue1=1

  tgt=45
  rotSmall=30
  rotLarge=70
  ################################  only CB
  addOptions=" --ini=$ini --learn_bg=0" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change_nobg=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot_nobg=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot_nobg=$pdfSuffix

  ################################  only CB, no adapttive critique
  addOptions=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=1.5" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change_nobg_noac=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot_nobg_noac=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot_nobg_noac=$pdfSuffix

  ################################  only CB, no adapttive critique, no degradation
  addOptions=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=1.5 --cbRateDepr=0" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change_nobg_noac_nodepr=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot_nobg_noac_nodepr=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot_nobg_noac_nodepr=$pdfSuffix

  ################################      BG only

  addOptions=" --ini=$ini --learn_cb=0" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change_nocb=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot_nocb=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot_nocb=$pdfSuffix

  ################################    CB and BG, no adaptive critic, binary RWD

  addOptions=" --ini=$ini --cbLRateIsConst=1 --cbLRate=1.5" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change_noac=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot_noac=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot_noac=$pdfSuffix

  ################################   CB and BG with adaptive critique, binary RWD
  addOptions=" --ini=$ini" 

  addOptionsLoc="--cue1=1"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_cue_change=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotSmall"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_small_rot=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge"$addOptions
  perturbSimple "$addOptionsLoc" $1 $useOldData
  args_percept_large_rot=$pdfSuffix


  ################################

  #addOptionsLoc="--force_field1=-2."$addOptions
  #perturbSimple "$addOptionsLoc" $1 $useOldData
  #args_force_field=$pdfSuffix

  python $plotfile \ 
    "$args_percept_cue_change_nobg" "$args_percept_small_rot_nobg" "$args_percept_large_rot_nobg" \
    "$args_percept_cue_change_nobg_noac" "$args_percept_small_rot_nobg_noac" "$args_percept_large_rot_nobg_noac" \ 
    "$args_percept_cue_change_nocb" "$args_percept_small_rot_nocb" "$args_percept_large_rot_nocb" \
    "$args_percept_cue_change_noac" "$args_percept_small_rot_noac" "$args_percept_large_rot_noac" \
    "$args_percept_cue_change" "$args_percept_small_rot" "$args_percept_large_rot" 

  #python3 "$plotfile" "$args_force_field" "$args_endpoint_rotation" "$args_percept_xrev" "$args_percept_small_rot"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
