runfile="./pert_prl"
plotfile="universal_plot.py"
pdfdir=output_for_paper
experimentName=force_field
ini="$experimentName.ini"
#plotfileArgs="--ini=$ini"

bg_on_cb_on=1
bg_off_cb_on=1
bg_on_cb_off=1

seed=0     #makes <more or less> random seed

if [ $# -eq 2 ] & [ $2 -eq 0 ]; then
  make pert_prl
fi

. ./run_dif_perturb.sh    # . means to source a script, so that it can use variables from the current script

if [ $# -ne 0 ]; then
  perturbAllConfig $1 "--ini=$ini $addOptions" 
fi
