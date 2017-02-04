file galea_dbg
break learn.cc:361 if experimentPhase == ADAPT1
run
disp {*y@50}
disp {sc,R}
disp {endpt_x,endpt_y,x_cb_target,y_cb_target}
disp {endpt_x-x_cb_target,endpt_y-y_cb_target}
disp { * wcb[0]@6,  * wcb[1]@6}
