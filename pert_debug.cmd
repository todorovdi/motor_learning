file pert_dbg
break BG_model.cpp:151 if DA>0
break pert.cpp:349 if sc<0.1 && experimentPhase==ADAPT1
run
disp *x@4
disp *w1[1]@10 
disp *d1@10
disp *w2[1]@10
disp *d2@10
disp *y@10
disp {xcur_real,ycur_real,xcur,ycur}
