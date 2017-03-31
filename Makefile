addhCbonly = par.h params*
addcCbonly = reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                  

addh = pert.h BG_model.h suppl.h exporter.h environment.h arm.h motor_learning.h par.h 
addc = pert.cpp learn.cpp BG_model.cpp CB_model.cpp arm.cpp motor_learning.cpp exporter.cpp environment.cpp suppl.cpp reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                                 

addh2 = BG_model.h suppl.h exporter.h environment.h arm.h motor_learning.h par.h 
addc2 = learn.cpp BG_model.cpp CB_model.cpp arm.cpp motor_learning.cpp exporter.cpp environment.cpp suppl.cpp reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc arm_inv.cc Force_MN.cc Inverse_muscles.cc                                                              

#learn: learn.cc reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc   par.h
#	g++  -O2  learn.cc reach.cc TrajectoriesAnglesVelocityAcceleration.cc  muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc  -o    $@
#
#cb: cb.cc $(addcCbonly) $(addhCbonly)
#	g++ -O2 $(addcCbonly) cb.cc -o $@
#	g++ -g  $(addcCbonly) cb.cc -o $@_dbg
#
#cb_orig: reach_cb_orig.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc   par.h
#	g++  -O2  reach_cb_orig.cc TrajectoriesAnglesVelocityAcceleration.cc  muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc  -o    $@
#
#cb_orig_dbg: reach_cb_orig.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc   par.h
#	g++  -g  reach_cb_orig.cc TrajectoriesAnglesVelocityAcceleration.cc  muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc  -o    $@
#
#pert: $(addc) $(addh)
#	g++ -std=c++11 -D BUILD_PERT  -O2 $(addc2) old_pert.cpp -o   $@        -lboost_system -lboost_program_options
#	g++ -std=c++11 -D BUILD_PERT  -g $(addc2) old_pert.cpp -o   $@_dbg     -lboost_system   -lboost_program_options
#
#pert_prl: $(addc) $(addh)
#	g++ -std=c++11 -D BUILD_PERT -D PARALLEL -fopenmp -O2 $(addc2) pert.cpp -o   $@        -lboost_system -lboost_program_options
##	g++ -std=c++11 -D BUILD_GALEA -fopenmp -g $(addc2)  -o   $@_dbg     -lboost_system   -lboost_program_options


pert: $(addc2) $(addh2)  pert.cpp pert.h 
	g++ -std=c++11 -D BUILD_PERT  -O2 $(addc2) pert.cpp -o   $@        -lboost_system 
	g++ -std=c++11 -D BUILD_PERT  -g $(addc2) pert.cpp -o   $@_dbg     -lboost_system   

pert_prl: $(addc2) $(addh2) pert.cpp pert.h
	g++ -std=c++11 -D BUILD_PERT -D PARALLEL -fopenmp -O2 $(addc2) pert.cpp -o   $@  -lboost_system 
#	g++ -std=c++11 -D BUILD_GALEA -fopenmp -g $(addc2)  -o   $@_dbg     -lboost_system   -lboost_program_options


#
#izshad: $(addc2) $(addh2) izshad.h izshad.cpp
#	g++ -std=c++11 -D BUILD_IZSHAD -O2 $(addc2) izshad.cpp -o   $@    -lboost_system 
#	g++ -std=c++11 -D BUILD_IZSHAD -g $(addc2) izshad.cpp -o   $@_dbg  -lboost_systems
##
#izshad_prl: $(addc2) $(addh2) izshad.h izshad.cpp
#	g++ -std=c++11 -D BUILD_IZSHAD -D PARALLEL -fopenmp -O2 $(addc2) izshad.cpp -o   $@        -lboost_system 
#	#g++ -std=c++11 -D BUILD_IZSHAD -D PARALLEL -fopenmp -g  $(addc2) izshad.cpp -o   $@_dbg    -lboost_system 
