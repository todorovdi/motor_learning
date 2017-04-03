runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=gutierrez
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

  addOptions=""
  addOptionsLoc=" --rewardDist=0.04"$addOptions
  useOldData=$2   # 9 means don't plot


  perturbSimple "--ini=$ini --HD=1$addOptionsLoc" $1 $useOldData
  args_nonEB_HD=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=0$addOptionsLoc" $1 $useOldData
  args_EB_control=$pdfSuffix


  perturbSimple "--ini=$ini --percept_xrev1=0$addOptionsLoc" $1 $useOldData
  args_EB_control=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=0 --HD=1$addOptionsLoc" $1 $useOldData
  args_EB_HD=$pdfSuffix

  perturbSimple "--ini=$ini --percept_xrev1=0 --PD=1$addOptionsLoc" $1 $useOldData
  args_EB_PD=$pdfSuffix

  perturbSimple "--ini=$ini$addOptionsLoc" $1  $useOldData
  args_nonEB_control=$pdfSuffix

  perturbSimple "--ini=$ini --HD=1$addOptionsLoc" $1 $useOldData
  args_nonEB_HD=$pdfSuffix

  perturbSimple "--ini=$ini --PD=1$addOptionsLoc" $1  $useOldData
  args_nonEB_PD=$pdfSuffix

  python "$plotfile" "$args_nonEB_HD" "$args_nonEB_PD" "$args_nonEB_control" "$args_EB_HD" "$args_EB_PD" "$args_EB_control" 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
