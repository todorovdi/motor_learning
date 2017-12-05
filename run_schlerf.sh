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
  uodControlsAC=$useOldData 
  #uodControlsAC=9 
  uodPatientsAC=$useOldData
  #uodPatientsAC=9

  #$ao="$1 --cbMotVarEst=$errThr"
  aocontrol="$1"
  aoNoAC="$2"
  aoAC="--cbLRateIsConst=0$2"

  if [ $runControls == '1' ]; then

    if [ $runConstRate == '1' ]; then
      runSim "--cbLRate=$normRate$aocontrol" $uodControls
      controls=$argSim
    fi

    if [ $runVarRate == '1' ]; then
      runSim "--cbLRateIsConst=0$aocontrol" $uodControlsAC
      controlsAC=$argSim
    fi

  fi
  ################################
  
  if [ $runConstRate == '1' ]; then

    runSim "$aoNoAC --cbLRate=0.17"  $uodPatients
    patientsRedRate=$argSim

    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.16 --cbLRate=$normRate$aoNoAC" $uodPatients
    #patientsIncNoiseDepr=$argSim

    ##################################################
    ##################################################

    #runSim "$aoNoAC --cbLRate=$normRate --cbStateDepr=$IncDepr" $uodPatients
    #patientsIncDepr=$argSim

    #runSim "$aoNoAC --cbLRate=$normRate --finalNoiseAmpl=$cbNoise" $uodPatients
    #patientsIncNoise=$argSim

    #runSim "$aoNoAC --cbLRate=$IncRate --cbStateDepr=$IncDepr --cbLRate=$IncRate" \
    #  $uodPatients
    #patientsIncDeprRate=$argSim

    #runSim "$aoNoAC --cbLRate=$IncRate --finalNoiseAmpl=$cbNoise --cbStateDepr=$IncDepr" $uodPatients
    #patientsIncNoiseDeprRate=$argSim

    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.06 --cbLRate=$normRate$aoNoAC" $uodPatients
    #patientsIncNoiseDepr=$argSim

    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.08 --cbLRate=$normRate$aoNoAC" $uodPatients
    #patientsIncNoiseDepr=$argSim


    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.14 --cbLRate=$normRate$aoNoAC" $uodPatients

    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.12 --cbLRate=$normRate$aoNoAC" $uodPatients

    #runSim "--finalNoiseAmpl=$cbNoise --cbStateDepr=0.1 --cbLRate=$normRate$aoNoAC" $uodPatients

    python $plotfile \
      "$controls" "$patientsIncNoiseDepr" \
    "---plotfname=${experimentName}_noAC_IncNoiseDepr16_$aoAC"

    #python $plotfile \
    #  "$controls" "$patientsIncNoiseDepr" \
    #"---plotfname=${experimentName}_noAC_IncNoiseDepr14_$aoAC"

    python $plotfile \
      "$controls" "$patientsRedRate" \
    "---plotfname=${experimentName}_noAC_redRate$aoAC"

  fi

  ##########################################################
  ##########################################################
  ##########################################################
  ##########################################################

  if [ $runVarRate == '1' ]; then

    runSim "$aoAC --cbMotVarEst=0.0088 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
    patientsACredErrThrIncNoise=$argSim

    runSim "$aoAC --acOptimalRateMult=0.05" $uodPatientsAC
    patientsACredOptRate=$argSim

    runSim "$aoAC --cbMotVarEst=0.0075" $uodPatientsAC
    patientsACredErrThr=$argSim

    runSim "$aoAC --cbMotVarEst=0.0068" $uodPatientsAC
    patientsACredErrThr2=$argSim

    #runSim "$aoAC --cbLRateUpdSpdUp=1.4" $uodPatientsAC
    #patientsACredRate2=$argSim

    runSim "$aoAC --cbLRateUpdSpdUp=1.12 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
    patientsACIncNoiseRedUpRate=$argSim

    #runSim "$aoAC --cbLRateUpdSpdDown=3" $uodPatientsAC
    #patientsACIncRate3=$argSim

    runSim "$aoAC --cbLRateUpdSpdDown=1.7 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
    patientsACIncNoiseIncDownRate=$argSim

    #runSim "$aoAC --cbLRateUpdSpdUp=8 --finalNoiseAmpl=$cbNoise" $uodPatientsAC
    #patientsACIncNoiseIncRate2=$argSim

    #runSim "$aoAC --cbLRateUpdSpdUp=2" $uodPatientsAC
    #patientsACIncRate2=$argSim

    #runSim "$aoAC --cbMotVarEst=0.006" $uodPatientsAC
    #patientsACIncErrThr=$argSim

    #runSim "$aoAC --acLowThrMult=1.8" $uodPatientsAC
    #patientsACredLowThr=$argSim

    #runSim "$aoAC --acLowThrMult=0.6 --acThrMult=7" $uodPatientsAC
    #patientsACredThr=$argSim

    runSim "$aoAC --finalNoiseAmpl=$cbNoise" $uodPatientsAC
    patientsACIncNoise=$argSim

    runSim "$aoAC --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise" $uodPatientsAC
    patientsACIncNoiseIncErrThr=$argSim

    runSim "$aoAC --cbStateDepr=$IncDepr --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise" $uodPatientsAC
    patientsACIncNoiseIncErrThrIncDepr=$argSim

    #runSim "$aoAC --acOptimalRateMult=$IncMult --finalNoiseAmpl=$cbNoise --cbStateDepr=$IncDepr" \
    #  $uodPatientsAC
    #patientsACIncNoiseDeprRate=$argSim

    ##################################
    #######################################
    #######################################
    #######################################

    #python $plotfile \
    #  "$controls" "$controlsAC" "$patientsRedRate" "$patientsACredMult" "$patientsIncDepr" "$patientsIncNoise" \
    #"---plotfname=${experimentName}_$aoAC"

    #python $plotfile "$controlsAC" "$patientsACredErrThrIncNoise" \
    #"---plotfname=${experimentName}_AC_patientsACredErrThrIncNoise$aoAC"


    #python $plotfile \
    #  "$controls" "$patientsIncNoise" \
    #"---plotfname=${experimentName}_noAC_IncNoise_$aoAC"

    #python $plotfile \
    #  "$controls" "$patientsIncDepr" \
    #"---plotfname=${experimentName}_noAC_IncDepr_$aoAC"

    #python $plotfile \
    #  "$controls" "$patientsIncDeprRate" \
    #"---plotfname=${experimentName}_noAC_IncDeprRate_$aoAC"

    #python $plotfile \
    #  "$controls" "$patientsIncNoiseDepr" \
    #"---plotfname=${experimentName}_noAC_IncNoiseDepr_$aoAC"

    #python $plotfile \
    #  "$controls" "$patientsIncNoiseDeprRate" \
    #"---plotfname=${experimentName}_noAC_IncNoiseDeprRate_$aoAC"

    #######################################
    #######################################

    if [ $plot2plots == '1' ]; then
      python $plotfile \
        "$controlsAC" "$patientsACredOptRate" \
      "---plotfname=${experimentName}_AC_redOptRate$aoAC"

      #python $plotfile \
      #  "$controlsAC" "$patientsACredRate2" \
      #"---plotfname=${experimentName}_AC_redRate2$aoAC"

      #python $plotfile \
      #  "$controlsAC" "$patientsACIncRate3" \
      #"---plotfname=${experimentName}_AC_IncRate3$aoAC"

      #python $plotfile \
      #  "$controlsAC" "$patientsACIncRate2" \
      #"---plotfname=${experimentName}_AC_IncRate2$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncNoiseRedUpRate" \
      "---plotfname=${experimentName}_AC_IncNoiseRedUpRate$aoAC"

      #python $plotfile \
      #  "$controlsAC" "$patientsACIncNoiseIncRate2" \
      #"---plotfname=${experimentName}_AC_IncNoiseIncRate2$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncNoiseIncDownRate" \
      "---plotfname=${experimentName}_AC_IncNoiseIncDownRate$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncNoise" \
      "---plotfname=${experimentName}_AC_IncNoise$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncNoiseIncErrThrIncDepr" \
      "---plotfname=${experimentName}_AC_IncNoiseIncErrThrIncDepr$aoAC"

      #python $plotfile \
      #  "$controlsAC" "$patientsACIncNoiseRedThr" \
      #"---plotfname=${experimentName}_AC_IncNoiseRedThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACredErrThr" \
      "---plotfname=${experimentName}_AC_redErrThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACredErrThr2" \
      "---plotfname=${experimentName}_AC_redErrThr2$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncNoiseIncErrThr" \
      "---plotfname=${experimentName}_AC_IncNoiseIncErrThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncErrThr" \
      "---plotfname=${experimentName}_AC_IncErrThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACredLowThr" \
      "---plotfname=${experimentName}_AC_redLowThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACredThr" \
      "---plotfname=${experimentName}_AC_redThr$aoAC"

      python $plotfile \
        "$controlsAC" "$patientsACIncDeprRate" \
      "---plotfname=${experimentName}_AC_IncDeprRate$aoAC"

      python  "$controlsAC" "$patientsACIncNoiseDeprRate" \
      "---plotfname=${experimentName}_AC_IncNoiseDeprRate$aoAC"
    fi

    python $plotfile "$controlsAC" \
      "$patientsACIncNoiseIncErrThr" \
      "$patientsACIncNoiseIncErrThrIncDepr" \
      "$patientsACIncNoise" \
      "$patientsACredErrThrIncNoise" \
      "$patientsACredOptRate" \
      "$patientsACredErrThr" \
      "$patientsACIncNoiseRedUpRate" \
      "$patientsACIncNoiseIncDownRate" \
        "---plotfname=${experimentName}_allTogether_$aoAC" \
        "---onlyMainPlots"
      #"$patientsACredErrThr2" \
  fi
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

  IncDepr=0.35
 
  incRateUpdSpd=3
  # just make bigger noise for CB and increase degradation

  controlNoise=0.03
  cbNoise=0.06
  #runAll " --randomTgt_all=0"

  #runAll ""

  #runAll " --finalNoiseAmpl=0.030"
  #runAll " --finalNoiseAmpl=0.015"

  cbNoise=0.016
  controlNoise=0.0105
  redMult=0.1
  incMult=0.4

  normRate=2
  redRate=0.3
  IncDepr=0.08
  #runAll "" "_normRate=2"

  redRateUpdSpd=1.2
  normRate=1
  normMult=0.2
  redRate=0.2


  IncDepr=0.16
  cbNoise=0.013

  # run flags

  runConstRate=1
  runVarRate=0
  # plot comparison plots for var rate?
  plot2plots=0
  runControls=1
  runAll "" ""

  ###############

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3" $useOldData
  #controls1=$argSim

  #python $plotfile "$controls1" "$controls1" 

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.3 --acThrMult=3.5 --acOptimalRateMult=0.2" $useOldData
  #controls1=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.8" $useOldData
  #controls2=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.3 --cbLRateUpdSpdDown=1.01" $useOldData
  #controls3=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.2 --cbLRateUpdSpdDown=1.3" $useOldData
  #controls4=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise --cbLRateUpdSpdUp=1.25" $useOldData
  #controls1=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise --cbLRateUpdSpdDown=1.7" $useOldData
  #controls2=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise --cbLRateUpdSpdDown=1.5" $useOldData
  #controls3=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise --cbLRateUpdSpdDown=1.3" $useOldData
  #controls4=$argSim

  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise ----cbLRateUpdSpdUp=1.7 --cbLRateUpdSpdDown=1.7" $useOldData
  #controls5=$argSim


  #runSim "--cbLRateIsConst=0 --finalNoiseAmpl=$cbNoise --cbMotVarEst=$cbNoise" $useOldData
  #controls1=$argSim

  #runSim "--cbLRateIsConst=0 --acLowThrMult=2.5" $useOldData
  #controls2=$argSim

  ## reduces adaptation
  #runSim "--cbLRateIsConst=0 --acThrMult=2.8" $useOldData
  #controls3=$argSim

  # reduceds rate only
  #runSim "--cbLRateIsConst=0 --acOptimalRateMult=0.3" $useOldData
  #controls4=$argSim

  ## reduces adaptation
  #runSim "--cbLRateIsConst=0 --cbLRateUpdSpdDown=2.7" $useOldData
  #controls5=$argSim

  #python $plotfile "$controls1" "$controls2" "$controls3" "$controls4" "$controls5" 





  # controls ok but even +noise alone kills everything
  #add=" --cbLRateUpdSpdUp=2 --cbLRateUpdSpdDown=2"

  # controls ok, noise alone don't kill but reduces significantly
  #runAll "$add" "$add"

  #add2=" --acLowThrMult=2 --acThrMult=4"
  #runAll "$add2" "$add2"

  #add3=" --acLowThrMult=3 --acThrMult=5"
  #runAll "$add3" "$add3"

  #add4="$add2 --cbLRateUpdSpdUp=1.5 --cbLRateUpdSpdDown=2"
  #runAll "$add4" "$add4"

  # even controls die
  #add2=" --acLowThrMult=1.5 --acThrMult=2.2"
  #runAll "$add2" "$add2"

  #runAll "" ""

  # When noise only is increaed and motor variability is matched -- no decrease if no change degradation
  # when non matched -- decrease

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
