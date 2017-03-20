if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
elif [ $# -eq 1 ]; then
  echo "Please tell if keep existing dat files (0 or 1)"
else

  addOptions=""

  #./run_galea.sh $1 $2
  #./run_gutierrez.sh  $1 $2
  ./run_shmuelof.sh   $1 $2
  #./run_izshad.sh     $1 $2
  #./run_force_field.sh     $1 $2
fi
