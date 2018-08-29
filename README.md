# Code for the simulation of interactions between basal ganglia and cerebellum during motor adaptation

## Reference

Goes with manuscript "The interplay between cerebellum and basal ganglia in motor adaptation: a modeling study"
Dmitrii I. Todorov, Robert A. Capps, William H. Barnett, Elizaveta M. Latash, Taegyo Kim, Khaldoun C. Hamade, Sergey N. Markin, Ilya A. Rybak, Yaroslav I. Molkov

## Descr

Here you find some code is in C++ (the model itself), python 2.7 (plotting) and bash scripts (running model and plotting in batches)
There are also .ini files that control the behavior

In general the code implements a simulation environment -- one specifies most of the parameters in .ini file to define an experimental context.
One can also override some of parameters from .ini files if one passes a command-line argument to the model

you'll need python 2.7,matplotlib,scipy,numpy,  boost lib for c++
you'll need to create output directories for the plots (that specified in .ini files) manually

bash scripts
  run_shmuelof.sh
  run_schlerf.sh
  run_gutierrez.sh
are used to reproduce behavioral experiments from corresponding papers (see article for this correspondence) and generate figures
  run_modelPerf.sh is for 3-phase contexts, also generates figures


The general architecture follows the idea of "model as participant" -- there is some code
that implements what is supposed to be happening inside human brain and body ( BG_model.cpp, CB_model.cpp, arm.cpp, motor_learning.cpp ) 
and some other code that implements some sort of virtual reality -- reproduction of the experimental environment and its interactions with the participant ( like driving force in non-autonomous ODE, it determines which targets/cues are shown, what visuomotor perturbations are applied )

On the plotting side, there are some general purpose plotting code and there are *_addplot.py 
files that produce plots for a particular experiment

## License

This code is released under the terms of the GNU General Public License v3.0.

### For inquiries 
Please contact todorovdi@gmail.com
