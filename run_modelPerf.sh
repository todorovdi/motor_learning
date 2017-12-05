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
    "---plotfname=CritTest_$1"
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

  addOptionsLoc="--cue1=1 --percept_xrev1=1 --cbLRateIsConst=1 --cbLRate=${cblc}"$ao
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
  ao=" --ini=$ini --learn_bg=0 --cbLRateIsConst=1 --cbLRate=$cblc --cbStateDepr=0"$1 
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

# Pyclewn gdb --args "pert_dbg --cbLRateIsConst=1 --learn_bg=0 --cue1=1 --ini=modelPerf.ini"
# Pyclewn gdb --args pert_dbg "--cbLRateIsConst=1 --learn_bg=0 --cue1=1 --ini=modelPerf.ini"
runForPaper()
{
  #params="--cbLRate=$cblc --cbStateDepr=$depr"
  #ao="$addOptions --learn_bg=0 --cbLRateIsConst=1 $params"$1 

  strongAo=" --numTrials1=100 --cbLRate=2"

  #####################   CB for small adaptation

  if [ $run_mildPert_cbOnly == '1' ]; then
    ao="$addOptions --learn_bg=0 --cbLRateIsConst=1"$1 

    addOptionsLoc="--cue1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    shiftSmall=$pdfSuffix

    addOptionsLoc="--cue1=2 --endpoint_rotation1=30"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rotSmall=$pdfSuffix

    python $plotfile  "$shiftSmall" "$rotSmall" "---plotfname=mildPert_cbOnly"
  fi

  ####################   CB failure strong pert, should include W plot

  ##addOptionsLoc="--cue1=4 --cbStateDepr=0"$ao
  ##perturbSimple "$addOptionsLoc" $nsess $useOldData

  ##addOptionsLoc="--cue1=2 --endpoint_rotation1=-90 --cbStateDepr=0"$ao
  ##perturbSimple "$addOptionsLoc" $nsess $useOldData
  ###rot90=$pdfSuffix
  if [ $run_strongPert_cbOnly == '1' ]; then
    ao="$addOptions --learn_bg=0 --cbLRateIsConst=1$strongAo$1" 

    addOptionsLoc="--cue1=4"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    shift90=$pdfSuffix

    addOptionsLoc="--cue1=2 --endpoint_rotation1=-90"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rot90=$pdfSuffix

    #addOptionsLoc="--cue1=4 --cbLRate=2"$ao
    #perturbSimple "$addOptionsLoc" $nsess $useOldData
    #shift90_=$pdfSuffix

    #addOptionsLoc="--cue1=2 --endpoint_rotation1=-90 --cbLRate=2"$ao
    #perturbSimple "$addOptionsLoc" $nsess $useOldData
    #rot90_=$pdfSuffix

    # guti NEBL
    addOptionsLoc="--cue1=1 --percept_xrev1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    xrev=$pdfSuffix

    python $plotfile "$shift90" "$rot90" "$xrev" "$shift90_" "$rot90_" "---plotfname=strongPert_cbOnly$1"
  fi

  #################### different rotations reaching plots
  if [ $run_difRotations == '1' ]; then
    ao="$addOptions --learn_bg=0 --cbLRateIsConst=1 --cbLRate=3.5 --cbStateDepr=0"$1 

    addOptionsLoc="--numTrials0=0 --numTrials2=0 --endpoint_rotation1=-60"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rot1=$pdfSuffix

    addOptionsLoc="--numTrials0=0 --numTrials2=0 --endpoint_rotation1=-75"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rot2=$pdfSuffix

    addOptionsLoc="--numTrials0=0 --numTrials2=0 --endpoint_rotation1=-85"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rot3=$pdfSuffix

    python $plotfile "$rot1" "$rot2" "$rot3" "---plotfname=difRotations_cbOnly$1"
  fi
  ################## BG + CB noCrit shift 
  if [ $run_mildPert_noCrit == '1' ]; then
    ao="$addOptions --cbLRateIsConst=1"$1 

    addOptionsLoc="--cue1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    mildPert=$pdfSuffix

    python $plotfile "$mildPert" "$mildPert" "---plotfname=mildPert_noCrit$1"
  fi

  ############### BG + CB noCrit strong pert 
  if [ $run_strongPert_noCrit == '1' ]; then
    ao="$addOptions --cbLRateIsConst=1$strongAo$1" 

    addOptionsLoc="--cue1=2 --endpoint_rotation1=-90"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    strongPert1=$pdfSuffix

    addOptionsLoc="--cue1=1 --percept_xrev1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    strongPert2=$pdfSuffix

    python $plotfile "$strongPert1" "$strongPert2" "---plotfname=strongPert_noCrit$1"
  fi

  ################### BG + CB with Crit, strong pert 
  if [ $run_strongPert_Crit == '1' ]; then
    ao="$addOptions$strongAo$1" 

    addOptionsLoc="--cue1=2 --endpoint_rotation1=-90$ao"
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    strongPert1=$pdfSuffix

    addOptionsLoc="--cue1=1 --percept_xrev1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    strongPert2=$pdfSuffix

    python $plotfile "$strongPert1" "$strongPert2" "---plotfname=strongPert_Crit$1"
  fi

  ################### BG + CB with Crit for CB, shift
  ###ao="$addOptions --cbStateDepr=$depr$1 --cbLRate=0.2 --acOptimalRateMult=0.1" 
  #ao="$addOptions$1" 
  ##ao="$addOptions --cbStateDepr=$depr$1 --cbLRate=0.2" 

  if [ $run_mildPert_Crit == '1' ]; then
    ao="$addOptions$1"
    addOptionsLoc="--cue1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    mildPert=$pdfSuffix

    python $plotfile "$mildPert" "$mildPert" "---plotfname=mildPert_Crit$1"
  fi

  #################### CB only with Crit shift and rotation

  if [ $run_mildPert_CritCBOnly == '1' ]; then
    ao="$addOptions --learn_bg=0"$1 

    addOptionsLoc="--cue1=1"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    shiftSmall=$pdfSuffix

    addOptionsLoc="--cue1=2 --endpoint_rotation1=30"$ao
    perturbSimple "$addOptionsLoc" $nsess $useOldData
    rotSmall=$pdfSuffix

    python $plotfile  "$shiftSmall" "$rotSmall" "---plotfname=mildPert_CritcbOnly"
  fi

  # Gutierrez pic goes here
}

runCritic()
{
  ao="$1$addOptions"

  addOptionsLoc="--cue1=2 --endpoint_rotation1=5"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  mildPert=$pdfSuffix

  addOptionsLoc="--cue1=2 --endpoint_rotation1=20"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  mildPert2=$pdfSuffix

  addOptionsLoc="--cue1=1"$ao
  perturbSimple "$addOptionsLoc" $nsess $useOldData
  noPert=$pdfSuffix

  #addOptionsLoc="--cue1=2 --endpoint_rotation1=-80"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #strongPert=$pdfSuffix

  #addOptionsLoc="--cue1=1 --percept_xrev1=1"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #strongPert2=$pdfSuffix

  #addOptionsLoc="--cue1=1 --percept_xrev1=1 --cbLHistReset1=1"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #strongPert2_=$pdfSuffix

  #python $plotfile "$noPert" "$mildPert" "$strongPert" "$strongPert2" "$strongPert2_" "---plotfname=test_$1_n=$nsess"
  python $plotfile "$noPert" "$mildPert" "$mildPert2" "---plotfname=sch_$1_n=$nsess"
}

if [ $# -ne 0 ]; then

  #delay="3.0s"
  delay="0.1s"
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

  nsess=$1
  useOldData=$2   # 9 means don't plot intermediate plots

  addOptions=" --ini=$ini"
  #--cbStateDepr=0."
  addOptionsLoc=""$addOptions

  #####################################
  run_mildPert_cbOnly=1
  run_strongPert_cbOnly=1
  run_difRotations=1
  run_mildPert_noCrit=1
  run_strongPert_noCrit=1
  run_strongPert_Crit=1
  run_mildPert_Crit=1
  run_mildPert_CritCBOnly=1

  runForPaper ""
  #####################################

  #run_mildPert_cbOnly=0
  #run_strongPert_cbOnly=0
  #run_difRotations=0
  #run_mildPert_noCrit=0
  #run_strongPert_noCrit=0
  #run_strongPert_Crit=0
  #run_mildPert_Crit=0
  #run_mildPert_CritCBOnly=1

  #runForPaper " --acLowThrMult=1. --acThrMult=2.4"

  #ao="$addOptions --learn_bg=0 --cbLRateUpdSpdDown=1.8 --numPhases=2 --linearArm=1"
  ##ao="$addOptions --learn_bg=0 --cbLRateUpdSpdDown=1.8 --numTrials0=0 --linearArm=1 --debug_printCrit=1"
  #ao="$addOptions --learn_bg=0 --cbLRateUpdSpdDown=1.8 --numPhases=2"

  ##addOptionsLoc=$ao" --cue1=1 --finalNoiseAmpl=0 --criticPredictDepr=1"
  ##perturbSimple "$addOptionsLoc" $nsess $useOldData
  ##baselineTest1=$pdfSuffix

  ##addOptionsLoc=$ao" --cue1=1 --finalNoiseAmpl=0"
  ##perturbSimple "$addOptionsLoc" $nsess $useOldData
  ##baselineTest1=$pdfSuffix

  #addOptionsLoc=$ao" --endpoint_rotation1=30 --finalNoiseAmpl=0 --criticPredictDepr=1"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest1=$pdfSuffix

  #addOptionsLoc=$ao" --endpoint_rotation1=30 --finalNoiseAmpl=0"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest2=$pdfSuffix

  ##addOptionsLoc=$ao" --cue1=1 --finalNoiseAmpl=0 --cbStateDepr=0"
  ##perturbSimple "$addOptionsLoc" $nsess $useOldData
  ##baselineTest1=$pdfSuffix


  #addOptionsLoc=$ao" --endpoint_rotation1=30 --finalNoiseAmpl=0 --criticPredictDepr=1 --cbRetrainSpeedup=1"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest1Sp=$pdfSuffix

  #addOptionsLoc=$ao" --endpoint_rotation1=30 --finalNoiseAmpl=0 --cbRetrainSpeedup=1"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest2Sp=$pdfSuffix


  #python $plotfile "$baselineTest1" "$baselineTest1Sp" "$baselineTest2" "$baselineTest2Sp" "---plotfname=baseline_n=$1"

  #######################


  #addOptionsLoc=$ao" --acLowThrMult=1.5 --acThrMult=2"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest2=$pdfSuffix

  #addOptionsLoc=$ao" --acLowThrMult=1.7 --acThrMult=2"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest3=$pdfSuffix

  #addOptionsLoc=$ao" --acLowThrMult=1.9 --acThrMult=2"
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #baselineTest4=$pdfSuffix

  #python $plotfile "$baselineTest1" "$baselineTest2" "$baselineTest3" "$baselineTest4" "---plotfname=baseline_n=$1"



  #0.6, 2 = 0.8, 2 lower baseline
  #but 0.8, 1.5 = 0.6, 1.5 rate dies completely
  # with higher acThrMult we never kick the bad condition at all in the baseline

  #cblc=0.9
  #depr=0.04

  #perturbSimple "$addOptionsLoc" $nsess $useOldData
 






  #runForPaper " --linearArm=1"
  #runForPaper " --cbRetrainSpeedup=1"
  #runForPaper " --linearArm=1"

  #addOptionsLoc="$addOptions --cue1=1 --learn_bg=0"$ao

  #ao="$addOptions --learn_bg=0 --debug_printAC=1 --numTrials0=2 --numTrials2=2" 
  #addOptionsLoc="--cue1=2 --endpoint_rotation1=30"$ao
  #perturbSimple "$addOptionsLoc" $nsess $useOldData
  #mildPert=$pdfSuffix

  #ao="$addOptions --learn_bg=0" 
  #ao="$addOptions --learn_bg=0 --numTrials0=2 --numTrials2=2" 
  ao="$addOptions --learn_bg=0 --numTrials0=2 --numTrials2=2 --finalNoiseAmpl=0 --debug_printAC=1" 
  #ao=" $addOptions --cbStateDepr=0 --learn_bg=0 --numTrials0=2 --numTrials2=2 --finalNoiseAmpl=0 --debug_printAC=1" 
  #ao="$addOptions --learn_bg=0 --numTrials0=2 --numTrials2=2 --debug_printAC=1" 

  ao="$addOptions --learn_bg=0 --numTrials2=2 --finalNoiseAmpl=0 --debug_printAC=1" 
  ao="$addOptions --learn_bg=0 --numTrials2=2 --debug_printAC=1" 
  #ao=" $addOptions --learn_bg=0 --debug_printAC=1" 

  tgt=45
  rotSmall=-30
  rotLarge=-70
  cblc=1.5

  #runAcTest ""
  #runCBfault ""

  cblc=5
  #runCBfault ""

  cblc=1.5

  ao=" --ini=$ini"


  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
