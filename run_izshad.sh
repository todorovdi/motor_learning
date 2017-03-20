runfile="./izshad_prl"
plotfile="universal.plot.py"
pdfdir=output_for_paper
experimentName=izshad
ini="$experimentName.ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=2     #makes <more or less> random seed

if [ $# -eq 2 ] & [ $2 -eq 0 ]; then
  make izshad_prl
fi

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

if [ $# -ne 0 ]; then
  perturb "--ini=$ini $addOptions" 1 1 $1 
  args1=$pdfSuffix

  #python "$plotfile" "$args1" "$args2"
fi
