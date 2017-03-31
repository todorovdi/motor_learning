runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=izshad
ini="$experimentName.ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

if [ $# -eq 2 ] & [ $2 -eq 0 ]; then
  make pert_prl
fi

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

if [ $# -ne 0 ]; then
  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
  echo "Starting experiment "$experimentName

  perturbSimple "--ini=$ini $addOptions" $1 
  args1=$pdfSuffix
  
  addOptions="learn_cb=0"
  perturbSimple "--ini=$ini $addOptions" $1 
  args2=$pdfSuffix

  python "$plotfile" "$args1" "$args2"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
