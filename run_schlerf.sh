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
  #uodControls=9$useOldData
  uodControls=$useOldData
  uodPatients=$useOldData
  uodPatientsAC=9

  #$ao="$1 --cbLRateUpdAbsErr_threshold=$errThr"
  aocontrol="$1"
  aoNoAC="$2"
  ao="--cbLRateIsConst=0$2"

  runSim "$aocontrol --cbLRate=$normRate --finalNoiseAmpl=$controlNoise" $uodControls
  controls=$argSim

  runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$controlNoise$aocontrol" $uodControls
  controlsAC=$argSim

  ################################

  #runSim "$aoNoAC --cbLRate=$redRate"  $uodPatients
  #patientsRedRate=$argSim

  #runSim "$aoNoAC --cbLRate=$normRate --cbStateDepr=$incDepr" $uodPatients
  #patientsIncDepr=$argSim

  #runSim "$aoNoAC --cbLRate=$normRate --finalNoiseAmpl=$cbNoise" $uodPatients
  #patientsIncNoise=$argSim

  #runSim "$aoNoAC --cbLRate=$incRate --cbStateDepr=$incDepr --cbLRate=$incRate" \
  #  $uodPatients
  #patientsIncDeprRate=$argSim

  #runSim "$aoNoAC --cbLRate=$incRate --finalNoiseAmpl=$cbNoise --cbStateDepr=$incDepr" $uodPatients
  #patientsIncNoiseDeprRate=$argSim

  #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.06 --cbLRate=$normRate$aoNoAC" $uodPatients
  #patientsIncNoiseDepr=$argSim

  #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.08 --cbLRate=$normRate$aoNoAC" $uodPatients
  #patientsIncNoiseDepr=$argSim

  runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.16 --cbLRate=$normRate$aoNoAC" $uodPatients
  patientsIncNoiseDepr=$argSim

  #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.14 --cbLRate=$normRate$aoNoAC" $uodPatients

  #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.12 --cbLRate=$normRate$aoNoAC" $uodPatients

  #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.1 --cbLRate=$normRate$aoNoAC" $uodPatients

  python $plotfile \
    "$controls" "$patientsIncNoiseDepr" \
  "---plotfname=${experimentName}_noAC_incrNoiseDepr16_$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncNoiseDepr" \
  #"---plotfname=${experimentName}_noAC_incrNoiseDepr14_$ao"

  ################

  #runSim "$ao --acOptimalRateMult=$redMult" $uodPatientsAC
  #patientsACredRate=$argSim

  #runSim "$ao --cbLRateUpdAbsErr_threshold=0.0095" $uodPatientsAC
  #patientsACredErrThr=$argSim

  #runSim "$ao --cbLRateUpdAbsErr_threshold=0.009" $uodPatientsAC
  #patientsACredErrThr_=$argSim

  #runSim "$ao --cbLRateUpdSpdUp=1.02" $uodPatientsAC
  #patientsACredRate2=$argSim

  # grad adapt is ok, abrupt smaller than real data
  #runSim "$ao --cbLRateUpdSpdUp=1.3 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  #patientsACincNoiseRedRate2=$argSim

  #runSim "$ao --cbLRateUpdSpdUp=1.2 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  # grad adapt is ok, abrupt smaller than real data
  #runSim "$ao --cbLRateUpdSpdUp=1.4 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  ## don't know what happens there, strange result
  #runSim "$ao --cbLRateUpdSpdDown=8" $uodPatientsAC
  #patientsACincRate3=$argSim

  # grad adapt is ok, abrupt smaller than real data
  #runSim "$ao --cbLRateUpdSpdDown=4 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  #patientsACincNoiseIncRate3=$argSim

  # grad adapt is ok, abrupt smaller than real data
  #runSim "$ao --cbLRateUpdSpdDown=3 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  #runSim "$ao --cbLRateUpdSpdUp=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  #patientsACincNoiseIncRate2=$argSim

  #runSim "$ao --cbLRateUpdSpdUp=8" $uodPatientsAC
  #patientsACincRate2=$argSim

  #runSim "$ao --cbLRateUpdAbsErr_threshold=0.05" $uodPatientsAC
  #patientsACincErrThr=$argSim

  # more difficult to increase learn rate
  #runSim "$ao --acLowThrMult=0.5" $uodPatientsAC
  #patientsACredLowThr=$argSim

  ## easier to reduce learn rate
  #runSim "$ao --acLowThrMult=0.6 --acThrMult=3" $uodPatientsAC
  #patientsACredThr=$argSim

  #runSim "$ao --cbStateDepr=$incDepr " $uodPatientsAC
  #patientsACincDepr=$argSim

  #runSim "$ao --finalNoiseAmpl=0.026" $uodPatientsAC
  #patientsACincNoise=$argSim

  runSim "$ao --cbStateDepr=0.1 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  runSim "$ao --cbStateDepr=0.13 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  patientsACincNoiseIncDepr=$argSim

  # kills all
  #runSim "$ao --acLowThrMult=1 --acThrMult=6 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  runSim "$ao --acLowThrMult=2 --acThrMult=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  runSim "$ao --acLowThrMult=3 --acThrMult=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  runSim "$ao --acLowThrMult=4 --acThrMult=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
  runSim "$ao --acLowThrMult=5 --acThrMult=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  #runSim "$ao --acLowThrMult=0.8 --acThrMult=6 --finalNoiseAmpl=$cbNoise" $uodPatientsAC

  #runSim "$ao --acOptimalRateMult=$incMult --cbStateDepr=$incDepr" \
  #  $uodPatientsAC
  #patientsACIncDeprRate=$argSim

  #runSim "$ao --acOptimalRateMult=$incMult --finalNoiseAmpl=$cbNoise --cbStateDepr=$incDepr" \
  #  $uodPatientsAC
  #patientsACIncNoiseDeprRate=$argSim

  ##################################

  #python $plotfile \
  #  "$controls" "$controlsAC" "$patientsRedRate" "$patientsACredMult" "$patientsIncDepr" "$patientsIncNoise" \
  #"---plotfname=${experimentName}_$ao"

  #python $plotfile \
  #  "$controls" "$patientsRedRate" \
  #"---plotfname=${experimentName}_noAC_redRate$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncNoise" \
  #"---plotfname=${experimentName}_noAC_incrNoise_$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncDepr" \
  #"---plotfname=${experimentName}_noAC_incrDepr_$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncDeprRate" \
  #"---plotfname=${experimentName}_noAC_incrDeprRate_$ao"

  python $plotfile \
    "$controls" "$patientsIncNoiseDepr" \
  "---plotfname=${experimentName}_noAC_incrNoiseDepr_$ao"

  #python $plotfile \
  #  "$controls" "$patientsIncNoiseDeprRate" \
  #"---plotfname=${experimentName}_noAC_incrNoiseDeprRate_$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACredRate" \
  #"---plotfname=${experimentName}_AC_redRate$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACredRate2" \
  #"---plotfname=${experimentName}_AC_redRate2$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincRate3" \
  #"---plotfname=${experimentName}_AC_incRate3$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincRate2" \
  #"---plotfname=${experimentName}_AC_incRate2$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincNoiseRedRate2" \
  #"---plotfname=${experimentName}_AC_incNoiseRedRate2$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincNoiseIncRate2" \
  #"---plotfname=${experimentName}_AC_incNoiseIncRate2$ao"

  python $plotfile \
    "$controlsAC" "$patientsACincNoiseIncRate3" \
  "---plotfname=${experimentName}_AC_incNoiseIncRate3$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincNoise" \
  #"---plotfname=${experimentName}_AC_incrNoise$ao"

  python $plotfile \
    "$controlsAC" "$patientsACincNoise" \
  "---plotfname=${experimentName}_AC_incrNoiseIncDepr$ao"

  python $plotfile \
    "$controlsAC" "$patientsACincNoiseRedThr" \
  "---plotfname=${experimentName}_AC_incNoiseRedThr$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACredErrThr" \
  #"---plotfname=${experimentName}_AC_redErrThr$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACredErrThr_" \
  #"---plotfname=${experimentName}_AC_redErrThr_$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACincErrThr" \
  #"---plotfname=${experimentName}_AC_incErrThr$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACredLowThr" \
  #"---plotfname=${experimentName}_AC_redLowThr$ao"

  python $plotfile \
    "$controlsAC" "$patientsACredThr" \
  "---plotfname=${experimentName}_AC_redThr$ao"

  #python $plotfile \
  #  "$controlsAC" "$patientsACIncDeprRate" \
  #"---plotfname=${experimentName}_AC_incDeprRate$ao"

  #python  "$co$useOldDatantrolsAC" "$patientsACIncNoiseDeprRate" \
  #"---plotfname=${experimentName}_AC_incNoiseDeprRate$ao"

  #python $plot0.7file "$patientsRedRate" "$patientsIncNoise" \
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
               #$useOldData
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
 
  incRateUpdSpd=3
  # just make bigger noise for CB and increase degradation

  controlNoise=0.03
  cbNoise=0.06
  #runAll " --randomTgt_all=0"

  #runAll ""

  #runAll " --finalNoiseAmpl=0.030"
  #runAll " --finalNoiseAmpl=0.015"

  cbNoise=0.018
  controlNoise=0.012
  redMult=0.1
  incMult=0.4

  normRate=2
  redRate=0.3
  incDepr=0.08
  #runAll "" "_normRate=2"

  redRateUpdSpd=1.2
  normRate=1
  normMult=0.2
  redRate=0.2
  incDepr=0.16
  #runAll "" "_normRate=1"
  #runAll "" "--finalNoiseAmpl=$cbNoise"
  runAll "" ""

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
