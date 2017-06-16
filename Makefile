addhCbonly = par.h params*
addcCbonly = reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                  

addh = pert.h BG_model.h suppl.h exporter.h environment.h arm.h motor_learning.h par.h 
addc = pert.cpp learn.cpp BG_model.cpp CB_model.cpp arm.cpp motor_learning.cpp exporter.cpp environment.cpp suppl.cpp reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                                 

addh2 = BG_model.h suppl.h exporter.h environment.h arm.h motor_learning.h par.h percept.h
addc2 = learn.cpp BG_model.cpp CB_model.cpp arm.cpp motor_learning.cpp exporter.cpp environment.cpp suppl.cpp reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc arm_inv.cc Force_MN.cc Inverse_muscles.cc percept.cpp

addc_recalib = arm_inv.cc TrajectoriesAnglesVelocityAcceleration.cc geometry_block.cc muscles.cc Inverse_muscles.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc Force_MN.cc suppl.cpp

pert: $(addc2) $(addh2)  pert.cpp pert.h 
	g++ -std=c++11 -D BUILD_PERT  -O2 $(addc2) pert.cpp -o   $@        -lboost_system 
	g++ -std=c++11 -D BUILD_PERT  -g $(addc2) pert.cpp -o   $@_dbg     -lboost_system   

pert_recalib: learn.cpp suppl.h par.h $(addc_recalib)
	g++ -D RECALIB -O2 learn.cpp $(addc_recalib) -o   $@   

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
