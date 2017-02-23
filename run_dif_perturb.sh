mkdir -p output_galea
calc_dir=$HOME/tmp_out_calc 
mkdir -p $calc_dir
rm -f output_galea/*.dat
rm -f $calc_dir/*.dat

pdfSuffix="action_and_cue_change_bgon_cbon"
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=1 --pdfSuffix=$pdfSuffix     --action_change1=1 --cue_change1=1
python galea.plot.py #$pdfSuffix
rm -f $calc_dir/*.dat

pdfSuffix="endpoint_rotation1_bgon_cbon"
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=1 --pdfSuffix=$pdfSuffix      --endpoint_rotation1=1 
python galea.plot.py #$pdfSuffix
rm -f $calc_dir/*.dat

pdfSuffix="endpoint_xreverse1_bgon_cbon"
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=1 --pdfSuffix=$pdfSuffix      --endpoint_xreverse1=1
python galea.plot.py #$pdfSuffix
rm -f $calc_dir/*.dat

#pdfSuffix="target_rotation1_bgon_cbon"
#./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=1 --pdfSuffix=$pdfSuffix      --target_rotation1=1
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat
#
#pdfSuffix="force_field1_bgon_cbon"
#./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=1 --pdfSuffix=$pdfSuffix     --force_field1=-4.
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat

###################

pdfSuffix="action_and_cue_change_bgon_cboff"
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=0 --pdfSuffix=$pdfSuffix      --action_change1=1 --cue_change1=1    
python galea.plot.py #$pdfSuffix                                                                    
rm -f $calc_dir/*.dat                                                                                          
pdfSuffix="endpoint_rotation1_bgon_cboff"                                                           
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=0 --pdfSuffix=$pdfSuffix       --endpoint_rotation1=1    
python galea.plot.py #$pdfSuffix                                                                    
rm -f $calc_dir/*.dat                                                                               

pdfSuffix="endpoint_xreverse1_bgon_cboff"                                                           
./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=0 --pdfSuffix=$pdfSuffix       --endpoint_xreverse1=1    
python galea.plot.py #$pdfSuffix                                                                    
rm -f $calc_dir/*.dat                                                                   

#pdfSuffix="target_rotation1_bgon_cboff"
#./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=0 --pdfSuffix=$pdfSuffix      --target_rotation1=1
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat
#
#pdfSuffix="force_field1_bgon_cboff"                                                                 
#./galea_prl --nsessions=$1 --learn_bg=1 --learn_cb=0 --pdfSuffix=$pdfSuffix      --force_field1=-4.         
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat

##################

pdfSuffix="action_and_cue_change_bgoff_cbon"
./galea_prl --nsessions=$1 --learn_bg=0 --learn_cb=1 --pdfSuffix=$pdfSuffix      --action_change1=1 --cue_change1=1    
python galea.plot.py #$pdfSuffix                                                                               
rm -f $calc_dir/*.dat                                                                                          
                                                                                                               
pdfSuffix="endpoint_rotation1_bgoff_cbon"                                                                       
./galea_prl --nsessions=$1 --learn_bg=0 --learn_cb=1 --pdfSuffix=$pdfSuffix       --endpoint_rotation1=1               
python galea.plot.py #$pdfSuffix                                                                               
rm -f $calc_dir/*.dat                                                                                          
                                                                                                               
pdfSuffix="endpoint_xreverse1_bgoff_cbon"                                                                       
./galea_prl --nsessions=$1 --learn_bg=0 --learn_cb=1 --pdfSuffix=$pdfSuffix       --endpoint_xreverse1=1               
python galea.plot.py #$pdfSuffix                                                                               
rm -f $calc_dir/*.dat                                                                                          

#pdfSuffix="target_rotation1_bgoff_cbon"
#./galea_prl --nsessions=$1 --learn_bg=0 --learn_cb=1 --pdfSuffix=$pdfSuffix      --target_rotation1=1
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat
#                                                                                                               
#pdfSuffix="force_field1_bgoff_cbon"                                                                             
#./galea_prl --nsessions=$1 --learn_bg=0 --learn_cb=1 --pdfSuffix=$pdfSuffix      --force_field1=-4.                    
#python galea.plot.py #$pdfSuffix
#rm -f $calc_dir/*.dat


#python galea.plot.py
