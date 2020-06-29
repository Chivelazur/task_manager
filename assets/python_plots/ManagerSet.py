import matplotlib.pyplot as plt
import csv

plt.style.use('seaborn-whitegrid')
x = []
tc, tr = [], []

with open('manager_performance_vs_set.csv', 'r') as csvfile:
    plots = csv.DictReader(csvfile, delimiter=',')
    for row in plots:
        x.append(float(row['set_size']))
        tc.append( (float(row['time_spent'])-float(row['time_total'])) / float(row['time_total']))
        tr.append(float(row['time_total']))

fig, ax = plt.subplots(1, 1)

fig.legend(loc='lower center', labelspacing=0., ncol=3)
ax.set_title('Task manager performance: No parents equal test set (difference)')
ax.set_ylabel('Relative difference, %')
ax.set_xlabel('Set size')
ax.plot(x, tc, label='Task manager spent time', marker='o', markersize=3, linewidth=1.2)
#ax.plot(x, tr, label='Real total task runtime', marker='o', markersize=3, linewidth=1.2)
ax.axhline(y=0, color='k')
ax.axhline(y=0.01, color='k')
#ax.legend(loc='center left', bbox_to_anchor=(0.80, 0.65), fancybox=True)

plt.show()
