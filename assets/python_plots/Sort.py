import matplotlib.pyplot as plt
import csv

plt.style.use('seaborn-whitegrid')
x = []
worst, best, random = [], [], []

with open('sort_performance.csv', 'r') as csvfile:
    plots = csv.DictReader(csvfile, delimiter=',')
    for row in plots:
        x.append(float(row['set_size']))
        worst.append(float(row['worst_case_single']))
        random.append(float(row['random_single']))
        best.append(float(row['no_parents']))

fig, ax = plt.subplots(1, 1)

ax.set_title('Sort performance')
ax.set_xlabel('Set size')
ax.set_ylabel('Time, sec')
ax.plot(x, worst, label='Worst single', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, best, label='No parents', marker='o', markersize=3, linewidth=1.2)
ax.plot(x, random, label='Random single', marker='o', markersize=3, linewidth=1.2)
ax.text(5, 15, 'Compiler: MSVC v142, flag O2')
ax.text(5, 13, 'Proc: Intel Core i5-7200U 2.50GHz')

ax.legend()
plt.show()
