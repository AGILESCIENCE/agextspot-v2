import matplotlib
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
from matplotlib.font_manager import FontProperties

import numpy as np
import sys

performance_file_input = sys.argv[1]

header = []
threshold = []
acc = []
k = []
f = []
tpr = []
fpr = []



with open(performance_file_input) as file:
    rows = file.readlines()

header = rows[0].strip().split()[0:5]

# Data for plotting
for row in rows[1:]:  #skipping the first element
    tmp = row.strip().split()
    threshold.append(float(tmp[0]))
    acc.append(float(tmp[1]))
    k.append(float(tmp[2]))
    f.append(float(tmp[3]))
    tpr.append(float(tmp[4]))
    fpr.append(float(tmp[5]))





assert len(tpr) == len(fpr)


roc_area = 0
fpr_reversed = list(reversed(fpr))
tpr_reversed = list(reversed(tpr))

#print fpr_reversed
#print tpr_reversed

#print fpr_reversed
for i in range(1, len(fpr_reversed)):
    #print 'i=',i,' fpr[i]=',fpr_reversed[i],' fpr[i-1]=',fpr_reversed[i-1], 'tpr[i]=',tpr_reversed[i], 'tpr[i-1]=', tpr_reversed[i-1]
    roc_area += tpr_reversed[i-1]*(fpr_reversed[i]-fpr_reversed[i-1]) + (fpr_reversed[i]-fpr_reversed[i-1]) * (tpr_reversed[i]-tpr_reversed[i-1])/2
print "ROC AREA: ", roc_area #""" ATTENZIONE!! """




#fig, ax = plt.subplots()
# Note that using plt.subplots below is equivalent to using
# fig = plt.figure() and then ax = fig.add_subplot(111)
fig = plt.figure()
ax = fig.add_subplot(121)

ax.plot(threshold, acc)
ax.plot(threshold, k)
ax.plot(threshold, f)
ax.plot(threshold, tpr)
ax.plot(threshold, fpr)

ax.set(xlabel='Classification Threshold', ylabel=' ',
       title=' ')
ax.grid()




roc = fig.add_subplot(122)
roc.plot(fpr,tpr)
roc.plot(fpr,fpr)
roc.set(xlabel='False Positive Rate', ylabel='True Positive Rate',
       title='ROC curve')
roc.grid()



# Legend

fontP = FontProperties()
fontP.set_size('small')
ax.legend(['Accuracy', 'K statistic', 'F measure', 'True Positive Rate', 'False Positive Rate'],
            loc='upper left',
            bbox_to_anchor=(-0.05, 1.15),
            fancybox=True,prop={'size': 10},ncol=3,)#

#fig.savefig("test.png")
plt.show()
