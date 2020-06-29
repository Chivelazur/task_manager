import matplotlib.pyplot as plt
import csv

plt.style.use('seaborn-whitegrid')
x = []
ws, wm, rs, rm, np = [], [], [], [], []

n = '10'

with open('manager_performance_vs_thread.csv', 'r') as csvfile:
    plots = csv.DictReader(csvfile, delimiter=',')
    for row in plots:
        x.append(float(row['thread_count']))
        ws.append(float(row['worst_single_' + n]))
        wm.append(float(row['worst_multi_' + n]))
        rs.append(float(row['random_single_' + n]))
        rm.append(float(row['random_multi_' + n]))
        np.append(float(row['no_parents_' + n]))

fig, ax = plt.subplots(1, 1)

fig.legend(loc='lower center', labelspacing=0., ncol=3)
ax.set_title('Task manager performance: ' + n + ' tasks')
ax.set_ylabel('Time, sec')
ax.set_xlabel('Thread count')
ax.plot(x, ws, label='Worst single', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, wm, label='Worst multi', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, rs, label='Random single', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, rm, label='Random multi', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, np, label='No parents', marker='o', markersize=3, linewidth=1.2)

ax.axvline(x=4,  color='r', linewidth=1.2, linestyle='dashed')
ax.text(5, 8, 'Proc threads = 4', color='r', rotation=90)
ax.text(20, 4.8, 'Total task run = 5 sec', color='b')
ax.axhline(y=5,  color='b', linewidth=1.2, linestyle='dashed')
ax.legend(loc='center left', bbox_to_anchor=(0.80, 0.85), fancybox=True)

plt.show()
