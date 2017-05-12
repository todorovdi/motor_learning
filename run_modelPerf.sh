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

runAcTest()
{
  ao=" --ini=$ini --learn_bg=0"$1 
  runtri "$ao"
  args_percept_cue_change_nobg=$args_cue_change
  args_percept_small_rot_nobg=$args_small_rot
  args_percept_large_rot_nobg=$args_large_rot

  python $plotfile \
    "$args_percept_cue_change_nobg" "$args_percept_small_rot_nobg" "$args_percept_large_rot_nobg"
}

#runBGTest()
#{
#  ao=" --ini=$ini --learn_cb=0"$1
#  runtri "$ao"
#  args_percept_cue_change_nocb=$args_cue_change
#  args_percept_small_rot_nocb=$args_small_rot
#  args_percept_large_rot_nocb=$args_large_rot
#
#  python $plotfile \
#     "$args_percept_cue_change_nocb" "$args_percept_small_rot_nocb" \
#     "$args_percept_large_rot_nocb"
#}

runTables()
{
  runAcTest "$1"
  ################################  only CB
  ################################  only CB, no adapttive critique
  ao=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=1.5"$1 
  runtri "$ao"
  args_percept_cue_change_nobg_noac=$args_cue_change
  args_percept_small_rot_nobg_noac=$args_small_rot
  args_percept_large_rot_nobg_noac=$args_large_rot
  ################################  only CB, no adapttive critique, no degradation
  ao=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=1.5 --cbRateDepr=0"$1 
  runtri "$ao"
  args_percept_cue_change_nobg_noac_nodepr=$args_cue_change
  args_percept_small_rot_nobg_noac_nodepr=$args_small_rot
  args_percept_large_rot_nobg_noac_nodepr=$args_large_rot
  ################################      BG only
  ao=" --ini=$ini --learn_cb=0"$1
  runtri "$ao"
  args_percept_cue_change_nocb=$args_cue_change
  args_percept_small_rot_nocb=$args_small_rot
  args_percept_large_rot_nocb=$args_large_rot
  ################################    CB and BG, no adaptive critic, binary RWD
  ao=" --ini=$ini --cbLRateIsConst=1 --cbLRate=1.5"$1 
  runtri "$ao"
  args_percept_cue_change_noac=$args_cue_change
  args_percept_small_rot_noac=$args_small_rot
  args_percept_large_rot_noac=$args_large_rot
  ################################   CB and BG with adaptive critique, binary RWD
  ao=" --ini=$ini"$1 
  runtri "$ao"
  args_percept_cue_change=$args_cue_change
  args_percept_small_rot=$args_small_rot
  args_percept_large_rot=$args_large_rot
  ################################

  python $plotfile \
    "$args_percept_cue_change_nobg" "$args_percept_small_rot_nobg" "$args_percept_large_rot_nobg" \
    "$args_percept_cue_change_nobg_noac" "$args_percept_small_rot_nobg_noac" "$args_percept_large_rot_nobg_noac" \
    "$args_percept_cue_change_nobg_noac_nodepr" "$args_percept_small_rot_nobg_noac_nodepr" "$args_percept_large_rot_nobg_noac_nodepr" \
    "$args_percept_cue_change_nocb" "$args_percept_small_rot_nocb" "$args_percept_large_rot_nocb" \
    "$args_percept_cue_change" "$args_percept_small_rot" "$args_percept_large_rot"  \
    "$args_percept_cue_change_noac" "$args_percept_small_rot_noac" "$args_percept_large_rot_noac" 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

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
  nsess=$1

  tgt=45
  rotSmall=30
  rotLarge=70

  #runAcTest  " --acByUpdCoefThr=1 --acUpdCoefThr=0.001 --trainCBEveryTrial=1" 

  runTables ""


  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
