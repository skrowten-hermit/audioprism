import numpy as np
import os
from collections import OrderedDict



constref = "../../include/constref.h"
tempfile = "../../include/constref.tmp"
srcpath = "../../"
binpath = "../../../bin/x64_linux/"
clikfiles = ["../../inputs/defects/custom/8k/full/male_8k_wclick.wav",
             "../../inputs/defects/custom/8k/full/male_8k_wclicks.wav"]
nbfiles = ["../../inputs/defects/custom/8k/full/male_8k_wnoiseburst.wav",
           "../../inputs/defects/custom/8k/full/male_8k_wnoisebursts.wav"]
gapfiles = ["../../inputs/defects/custom/8k/full/male_8k_wbrokenaudio.wav"]
logfile = "../../source/test/scripts/defects.log"
rsltscript = "./../../source/test/scripts/defecttune.sh"



def write_vals(param, val):
    print "Writing values to constref.h....\n"
    tgtfile = open(constref, "r+")
    tmpfile = open(tempfile, "w+")
    for line in tgtfile:
        if "#define " + param in line:
            newline = "#define " + param + "_THRESHOLD " + str(val) + "\n"
            tmpfile.write(newline)
        else:
            tmpfile.write(line)
    tgtfile.close()
    tmpfile.close()
    os.system("mv " + tempfile + " " + constref + "\n")


def run_make(scen, num):
    print "Running make file with " + str(scen) + " and " + str(num) + "\n"
    os.system("cd " + srcpath + "; make all\n")


def exec_ap(inputdeffile, paramdict):
    print "Executing AudioPrism with the input defect file....\n"
    cmd = "cd " + binpath + "; ./audioprism-d -v -s " + inputdeffile + " | " + \
          rsltscript + " " + dictoinfo(paramdict) + " " + logfile + "\n"
    os.system(cmd)


def dictoinfo(indict):
    info = ""
    for key, val in indict.iteritems():
        if not (key == list(indict.keys())[-1]):
            info += key + "=" + str(val) + ","
        else:
            info += key + "=" + str(val)
    return info


def filldict(vallist):
    params['APCDD'] =  vallist[0]
    params['APCDP'] =  vallist[1]
    params['APCDS'] =  vallist[2]
    params['APNBD'] =  vallist[3]
    params['APNBS'] =  vallist[4]
    params['APDD'] =  vallist[5]
    params['APDE'] =  vallist[6]
    params['APDS'] =  vallist[7]



APCDD = 30.0
APCDP = 10
APCDS = -50
APNBD = 8
APNBS = -50
APDD = 8.0
APDE = -60.0
APDS = -50

pkeys = ['APCDD', 'APCDP', 'APCDS', 'APNBD', 'APNBS', 'APDD', 'APDE', 'APDS']
pvals = [APCDD, APCDP, APCDS, APNBD, APNBS, APDD, APDE, APDS]
params = OrderedDict(list(zip(pkeys, pvals)))

APCDD_MIN = 5.0
APCDD_MAX = 55.0
APCDP_MIN = 1
APCDP_MAX = 31
APCDS_MIN = -70
APCDS_MAX = -30
APNBD_MIN = -8
APNBD_MAX = 20
APNBS_MIN = -70
APNBS_MAX = -30
APDD_MIN = 1.0
APDD_MAX = 20.0
APDE_MIN = -80.0
APDE_MAX = -40.0
APDS_MIN = -70
APDS_MAX = -30

APCDD = APCDD_MIN
APCDP = APCDP_MIN
APCDS = APCDS_MIN
APNBD = APNBD_MIN
APNBS = APNBS_MIN
APDD = APDD_MIN
APDE = APDE_MIN
APDS = APDS_MIN

counterc = 0
countern = 0
counterd = 0

for APCDD in np.arange(APCDD_MIN, APCDD_MAX + 0.5, 0.5):
    print "Current APCDD : " + str(APCDD) + "\n"
    for APCDP in range(APCDP_MIN, APCDP_MAX + 1):
        print "Current APCDP for APCDD " + str(APCDD) + " : " + str(APCDP) + "\n"
        for APCDS in range(APCDS_MIN, APCDS_MAX + 1):
            counterc += 1
            print "Current APCDS for APCDD " + str(APCDD) + " and for APCDP " + str(APCDP) + " : " + str(APCDS) + "\n"
            pvlist = [APCDD, APCDP, APCDS, APNBD, APNBS, APDD, APDE, APDS]
            filldict(pvlist)
            write_vals("APCDD", APCDD)
            write_vals("APCDP", APCDP)
            write_vals("APCDS", APCDS)
            run_make("Click Detection", counterc)
            for file in clikfiles:
                exec_ap(file, params)
