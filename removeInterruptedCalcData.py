import universal_plot as up
import plotparams as pp
import stdplots as stdp
import numpy as np
import os
import sys
import re
import fnmatch

#if len(sys.argv) == 1:
#    print 'not enough params! '
#    sys.exit()
#
#dat_basename = sys.argv[1]

pp.paramsInit('shmuelof.ini')

#matchStr = '*' + dat_basename + '*_arm*.dat'
matchStr = '*_arm*.dat'
fnames = []
for filename in os.listdir(pp.out_dir):
    if fnmatch.fnmatch(filename, matchStr):
        fnames.append(pp.out_dir+filename)

if len(fnames) == 0:
    print("no files found")

for fname in fnames:
    try:
        up.paramsInitFromArmFname(fname)
    except IOError:
        os.remove(fname)
        continue

    n = pp.phaseBegins[-1]
    armData = stdp.armFileRead(fname)
    nrows,ncols = np.shape(armData) 
    if nrows != n:
        print("------- Warning: wrong length of data file table, maybe calc was terminated too early -> ",fname)
        ree = '(.*).dat'
        basename = os.path.basename(fname)
        name = re.match(ree,basename).group(1)  #re.search(ree,fnames[0])
        fnamePatToRemove = fname.replace("_arm","*")
        #print "------- about to delete ",fnamePatToRemove
        for filename in os.listdir(pp.out_dir):
            filename = pp.out_dir + filename
            if fnmatch.fnmatch(filename, fnamePatToRemove):
                try:
                    os.remove(filename)
                    print("------- deleting",filename)
                except OSError as e:
                    print(str(e))
