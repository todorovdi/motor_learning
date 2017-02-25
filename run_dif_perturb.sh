mkdir -p output_pert
calc_dir=$HOME/tmp_out_calc 
mkdir -p $calc_dir
rm -f output_pert/*.dat
rm -f $calc_dir/*.dat

perturb()
{
  pdfSuffix="${1}_bg${2}_cb${3}"
  echo $pdfSuffix
  ./pert_prl --nsessions=$4 --learn_bg=$2 --learn_cb=$3 --pdfSuffix="$pdfSuffix" $1
  python pert.plot.py "$pdfSuffix"
  #rm -f $calc_dir/*.dat
}

perturbAllConfig()
{
  args="$2"
  perturb "$args" 1 1 $1
  perturb "$args" 0 1 $1
  perturb "$args" 1 0 $1
}

perturbAllConfig $1 "--action_change1=1 --cue_change1=1" 
perturbAllConfig $1 "--endpoint_rotation1=1" 
perturbAllConfig $1 "--endpoint_xreverse1=1" 
perturbAllConfig $1 "--endpoint_xreverse1=1 --cue_change1=1 --action_change1=1 --dirShift=180 --target_xreverse1=1" 
perturbAllConfig $1 "--target_rotation1=1" 
perturbAllConfig $1 "--force_field1=1" 
