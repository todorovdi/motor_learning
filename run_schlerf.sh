runfile="./pert_prl"
plotfile="universal_plot.py"
# plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=schlerf
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

#make pert
#./pert --ini=$ini --recalibrateArmCortControl=1 --nsessions=1

runSim()
{
  addOptionsLoc="$1$addOptions"
  perturbSimple "$addOptionsLoc" $nsess $2
  argSim=$pdfSuffix

  ./beep.sh
  sleep 0.1s
  ./beep.sh
}

runAll()
{
  uodControls=$useOldData
  uodPatients=$useOldData 
  uodPatientsAC=$useOldData 

  #$ao="$1 --cbLRateUpdAbsErr_threshold=$errThr"
  ao="$1"

  runSim "$ao --cbLRate=$normRate --finalNoiseAmpl=$controlNoise" $uodControls
  controls=$argSim

  runSim "$ao --cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise" $uodControls
  controlsAC=$argSim

  ################################

  runSim "$ao --cbLRate=$redRate --finalNoiseAmpl=$controlNoise"  $uodPatients
  patientsRedRate=$argSim

  runSim "$ao --cbLRate=$normRate --cbStateDepr=$incDepr --finalNoiseAmpl=$controlNoise" $uodPatients
  patientsIncDepr=$argSim

  runSim "$ao --cbLRate=$normRate --finalNoiseAmpl=$cbNoise" $uodPatients
  patientsIncNoise=$argSim

  #runSim "$ao --cbLRate=$incRate --cbStateDepr=$incDepr --cbLRate=$incRate --finalNoiseAmpl=$controlNoise" \
  #  $uodPatients
  #patientsIncDeprRate=$argSim

  #runSim "$ao --cbLRate=$incRate --finalNoiseAmpl=$cbNoise --cbStateDepr=$incDepr --finalNoiseAmpl=$controlNoise" \
  #  $uodPatients
  #patientsIncNoiseDeprRate=$argSim

  ################

  runSim "$ao --acOptimalRateMult=$redMult --cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise" $uodPatientsAC
  patientsACredRate=$argSim

  runSim "$ao --cbLRateUpdSpdUp=$redRateUpdSpd --cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise" $uodPatientsAC
  patientsACredRate2=$argSim

  #runSim "$ao --cbLRateUpdSpdUp=$incRateUpdSpd --cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise" $uodPatientsAC
  #patientsACincRate2=$argSim

  runSim "$ao --cbStateDepr=$incDepr  --cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise" $uodPatientsAC
  patientsACincDepr=$argSim

  runSim "$ao --cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  patientsACincNoise=$argSim

  #runSim "$ao --acOptimalRateMult=$incMult --cbLRateIsConst=0 --cbStateDepr=$incDepr --finalNoiseAmpl=$controlNoise" \
  #  $uodPatientsAC
  #patientsACIncDeprRate=$argSim

  #runSim "$ao --acOptimalRateMult=$incMult --cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbStateDepr=$incDepr" \
  #  $uodPatientsAC
  #patientsACIncNoiseDeprRate=$argSim

  ##################################

  #python $plotfile \
  #  "$controls" "$controlsAC" "$patientsRedRate" "$patientsACredMult" "$patientsIncDepr" "$patientsIncNoise" \
  #"---plotfname=${experimentName}_$ao"

  python $plotfile \
    "$controls" "$patientsRedRate" \
  "---plotfname=${experimentName}_noAC_redRate$ao$2"

  python $plotfile \
    "$controls" "$patientsIncNoise" \
  "---plotfname=${experimentName}_noAC_incrNoise_$ao$2"

  python $plotfile \
    "$controls" "$patientsIncDepr" \
  "---plotfname=${experimentName}_noAC_incrDepr_$ao$2"

  #python $plotfile \
  #  "$controls" "$patientsIncDeprRate" \
  #"---plotfname=${experimentName}_noAC_incrDeprRate_$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncNoiseDeprRate" \
  #"---plotfname=${experimentName}_noAC_incrNoiseDeprRate_$ao"

  python $plotfile \
    "$controlsAC" "$patientsACredRate" \
  "---plotfname=${experimentName}_AC_redRate$ao$2"

  python $plotfile \
    "$controlsAC" "$patientsACredRate2" \
  "---plotfname=${experimentName}_AC_redRate2$ao$2"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincRate2" \
  #"---plotfname=${experimentName}_AC_incRate2$ao"

  python $plotfile \
    "$controlsAC" "$patientsACincNoise" \
  "---plotfname=${experimentName}_AC_incrNoise$ao"

  python $plotfile \
    "$controlsAC" "$patientsACincDepr" \
  "---plotfname=${experimentName}_AC_incrDepr$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACIncDeprRate" \
  #"---plotfname=${experimentName}_AC_incDeprRate$ao"

  #python  "$controlsAC" "$patientsACIncNoiseDeprRate" \
  #"---plotfname=${experimentName}_AC_incNoiseDeprRate$ao"

  #python $plotfile "$patientsRedRate" "$patientsIncNoise" \
  #  "$patientsIncDepr" "$patientsIncDeprRate" \
  #  "$patientsIncNoiseDeprRate" \
  #  "$patientsACredRate" "$patientsACincNoise" \
  #  "$patientsACincDepr" "$patientsACIncDeprRate" \
  #  "$patientsACIncNoiseDeprRate" \
  #    "$controls" "$controlsAC" \
  #    "---plotfname=${experimentName}_patients_$ao" \
  #    "---onlyMainPlots"
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

  echo "Starting experiment $experimentName"

  addOptions=" --ini=$ini"
  #--cbStateDepr=0."
  addOptionsLoc=""$addOptions
  useOldData=$2   

  # =10 in article
  nsess=$1

  redSpd=1
  redMult=0.2

  redRate=0.5
  normRate=4
  incRate=7

  incMult=0.9
  redMult=0.2

  incDepr=0.35
 
  redRateUpdSpd=1.5
  incRateUpdSpd=3
  # just make bigger noise for CB and increase degradation

  controlNoise=0.03
  cbNoise=0.06
  #runAll " --randomTgt_all=0"

  #runAll ""

  #runAll " --finalNoiseAmpl=0.030"
  #runAll " --finalNoiseAmpl=0.015"

  cbNoise=0.028
  controlNoise=0.012
  redMult=0.1
  incMult=0.4

  normRate=2
  redRate=0.3
  incDepr=0.08
  runAll "" "_normRate=2"

  normRate=1
  redRate=0.2
  incDepr=0.16
  runAll "" "_normRate=1"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
