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

runPair()
{
  ao="$1$addOptions"
  perturbSimple "--ini=$ini$ao" $nsess $useOldData
  args_EBL=$pdfSuffix
  
  ao=" --learn_cb=0$addOptions"
  perturbSimple "--ini=$ini$ao" $nsess $useOldData 
  args_NEBL=$pdfSuffix

  python "$plotfile" "$args_EBL" "$args_NEBL" "---plotfname=izhad_multi_$1"
}

if [ $# -ne 0 ]; then
  delay="3.0s"
  if [ $useOldData == '0' ]; then
    echo "!!! ----- Really delete corresponding *.dat files?" 
    echo "!!! ----- You have $delay to stop the script"
    sleep $delay
  fi
  echo "Starting experiment "$experimentName

  nsess=$1

  #addOptions=" --cbLRateUpdAbsErr_threshold=0.004 --finalNoiseAmpl=0.005"
  #addOptions=" --cbLRateUpdAbsErr_threshold=0.004"

  runPair ""
  a_EBL=$args_EBL
  a_NEBL=$args_NEBL

  runPair " --finalNoiseAmpl=0"
  runPair " --cbStateDepr=0"
  runPair " --cbStateDepr=0 --finalNoiseAmpl=0"


  #python "$plotfile" "$a_EBL" "$a_NEBL"

  ./beep.sh
  sleep 0.1s
  ./beep.sh
  sleep 0.1s
  ./beep.sh
fi
