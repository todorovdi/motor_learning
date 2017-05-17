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
    "$args_percept_cue_change_nobg" "$args_percept_small_rot_nobg" "$args_percept_large_rot_nobg" \
    "---plotfname=ACTest_$1"
}

runCBfault()
{
  ao=" --ini=$ini --numTrials1=100$1" 

  addOptionsLoc="--endpoint_rotation1=$rotLarge --learn_cb=0"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_large_rot_nocb=$pdfSuffix

  addOptionsLoc="--endpoint_rotation1=$rotLarge --cbLRateIsConst=1 --cbLRate=${cblc}"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_large_rot=$pdfSuffix

  addOptionsLoc="--percept_xrev1=1 --cbLRateIsConst=1 --cbLRate=${cblc}"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  args_rev=$pdfSuffix

  python $plotfile \
    "$args_large_rot_nocb" "$args_large_rot" "$args_rev" \
    "---plotfname=CBfault_rot${rotLarge}_cblc${cblc}_$1"
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
  ao=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=$cblc"$1 
  runtri "$ao"
  args_percept_cue_change_nobg_noac=$args_cue_change
  args_percept_small_rot_nobg_noac=$args_small_rot
  args_percept_large_rot_nobg_noac=$args_large_rot
  ################################  only CB, no adapttive critique, no degradation
  ao=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=$cblc --cbRateDepr=0"$1 
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
  ao=" --ini=$ini --cbLRateIsConst=1 --cbLRate=$cblc"$1 
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
    "$args_percept_cue_change_noac" "$args_percept_small_rot_noac" "$args_percept_large_rot_noac" \
  "---plotfname=perfTable_${rotLarge}_$1" \
  "---onlyMainPlots"

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
  cblc=1.5

  #runAcTest ""
  #runCBfault ""

  cblc=5
  #runCBfault ""

  cblc=1.5
  runTables ""

  cblc=5
  #runCBfault " --wmmaxFP=0.7 --cbRateDepr=0.16"
  #runCBfault ""

  cblc=10
  #runCBfault " --numTrials1=150"

  cblc=5
  rotLarge=85
  #runCBfault " --numTrials1=150"

  cblc=1.5
  rotLarge=85
  #runCBfault " --numTrials1=150"

  #runCBfault ""
  #runTables ""
  rotLarge=80

  #runCBfault ""

  rotLarge=85
  #runCBfault ""
  #runTables ""
  
  #runCBfault " --rewardDist=0.07 --cbRateDepr = 0"
  #runCBfault " --rewardDist=0.07 --wmmaxFP=0.7"
  #runCBfault " --rewardDist=0.07"

  #runCBfault " --rewardDist=0.07"
  #runCBfault " --rewardDist=0.05"
  #runCBfault ""


  #runAcTest ""

  #runAcTest " --cbInitShiftSz=0.65"
  #runAcTest " --cbInitShiftSz=0.05"
  #runAcTest " --cbInitShiftSz=0.35"


  #runTables ""

  # works bad as after begin reset to small value, lambda cannot rise back
  #runTables " --acInstantUpd=1 --acThrMult=1."  

  # same thing, even worse -- the diff between expected correction and the real one is bigger (as expected is lower)
  #runTables " --acInstantUpd=1 --acThrMult=1. --cbLRate=0.04 --acUpdCoefThr=0.002"  
  #runTables " --acInstantUpd=1 --acThrMult=1. --cbLRate=0.04"  
  #runTables " --acInstantUpd=1 --acThrMult=1. --acUpdCoefThr=0.002"  
  #runTables " --acInstantUpd=1 --acThrMult=1. --acUpdCoefThr=0.004"  

  #runTables " --acInstantUpd=1 --acThrMult=1. --acUpdCoefThr=0.002 --cbLRate=0.42"  

  #runTables " --acThrMult=1."
  #  --cbLRateUpdTwoErrThreshold=0

  # works but with more noise
  #runTables " --acThrMult=1."

  # small pert ok, large -- to small CB turn off 
  #runTables " --acThrMult=1. --cbLRateUpdSpdUp=20000."


  #perturbSimple "--ini=modelPerf.ini --learn_bg=0 --acUpdCoefThr=0.0013 --cue1=1 --acInstantUpd=1 --acThrMult=1 --cbLRate=0.02" $nsess $useOldData
  #Pyclewn gdb --args pert_dbg --ini=modelPerf.ini --learn_bg=0 --cue1=1 --acInstantUpd=1 --acThrMult=1 --cbLRate=0.42 --seed=1


  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
