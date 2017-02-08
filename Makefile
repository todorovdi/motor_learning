addhCbonly = par.h params*
addcCbonly = reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                  
#addh = suppl.h learn.h file_export.h par.h params*
#addc = learn.cpp suppl.cc file_export.cc reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                                 
addh2 = galea.h BG_model.h suppl.h exporter.h environment.h arm.h motor_learning.h par.h params*
addc2 = galea.cpp learn.cpp BG_model.cpp CB_model.cpp arm.cpp motor_learning.cpp exporter.cpp environment.cpp suppl.cpp reach.cc TrajectoriesAnglesVelocityAcceleration.cc muscles.cc geometry_block.cc calc_feedback.cc ParWeight.cc WeightedInput.cc sp_cord.cc Activationfunction.cc                                                                 

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
#galea: galea.cc galea.h galea_export.h galea_export.cc $(addc) $(addh)
#	g++  -D BUILD_GALEA -O2 $(addc) galea.cc galea_export.cc -o   $@
#	g++  -D BUILD_GALEA -g $(addc) galea.cc galea_export.cc -o   $@_dbg
#
#force_field: force_field.cc force_field.h force_field_export.h force_field_export.cc $(addc) $(addh)
#	g++  -D BUILD_FORCE_FIELD -O2 $(addc) force_field.cc force_field_export.cc -o   $@
#	g++  -D BUILD_FORCE_FIELD -g $(addc) force_field.cc force_field_export.cc -o   $@_dbg

galea: $(addc2) $(addh2)
	g++ -std=c++11 -D BUILD_GALEA  -O2 $(addc2) -o   $@        -lboost_system -lboost_program_options
	g++ -std=c++11 -D BUILD_GALEA  -g $(addc2)  -o   $@_dbg     -lboost_system   -lboost_program_options

galea_prl: $(addc2) $(addh2)
	g++ -std=c++11 -D BUILD_GALEA -fopenmp -O2 $(addc2) -o   $@        -lboost_system -lboost_program_options
	g++ -std=c++11 -D BUILD_GALEA -fopenmp -g $(addc2)  -o   $@_dbg     -lboost_system   -lboost_program_options
