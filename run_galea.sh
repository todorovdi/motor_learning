runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=galea
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

#make pert
#./pert --ini=$ini --recalibrateArmCortControl=1

if [ $# -ne 0 ]; then
  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
  echo "Starting experiment "$experimentName

  plotting=$2   # 9 means don't plot

  perturbSimple "--ini=$ini"$addOptions $1 $plotting
  args_sham=$pdfSuffix

  perturbSimple "--ini=$ini --CBtCDS1=1 --CBtCDS2=1"$addOptions $1  $plotting
  args_tCDS=$pdfSuffix

  python "$plotfile" "$args_sham" "$args_tCDS" 

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
