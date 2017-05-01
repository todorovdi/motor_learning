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

  nsess=$1

  addOptions=" --cbLRateUpdAbsErr_threshold=0.004"

  addOptionsLoc=" --rewardDist=0.03$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  args0=$pdfSuffix

  addOptionsLoc="$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  args1=$pdfSuffix
  
  addOptionsLoc=" --learn_cb=0$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData 
  args2=$pdfSuffix

  #python "$plotfile" "$args1" "$args2"

  addOptions=" --cbLRateUpdAbsErr_threshold=0.004 --finalNoiseAmpl=0.005"

  addOptionsLoc="$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  args3=$pdfSuffix

  addOptionsLoc=" --learn_cb=0$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData 
  args5=$pdfSuffix

  addOptions=" --cbLRateUpdAbsErr_threshold=0.004 --finalNoiseAmpl=0.002"

  addOptionsLoc="$addOptions"
  perturbSimple "--ini=$ini$addOptionsLoc" $nsess $useOldData
  args4=$pdfSuffix

  python "$plotfile" "$args0" "$args1" "$args3" "$args4" "$args2" "$args5"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
