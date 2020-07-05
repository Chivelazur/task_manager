import matplotlib.pyplot as plt
import csv

plt.style.use('seaborn-whitegrid')


def sort():
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

    ax.set_title('Sort performance vs set size')
    ax.set_xlabel('Set size')
    ax.set_ylabel('Time, sec')
    ax.plot(x, worst, label='Worst single', marker='o', markersize=3, linewidth=1.2)
    ax.plot(x, best, label='No parents', marker='o', markersize=3, linewidth=1.2)
    ax.plot(x, random, label='Random single', marker='o', markersize=3, linewidth=1.2)

    ax.legend()
    plt.figure(1)
    fig.set_size_inches(11, 8)
    plt.savefig("sort.svg", format="svg")


def perf_thread(count, fign):
    x = []
    ws, wm, rs, rm, np = [], [], [], [], []
    n = count

    with open('performance_vs_thread.csv', 'r') as csvfile:
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
    ax.set_title('Performance vs thread count ' + n + ' tasks')
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

    plt.figure(fign)
    fig.set_size_inches(11, 8)
    plt.savefig("performance_vs_thread_" + n + ".svg", format="svg")


def perf_set_fixed_runtime():
    x = []
    tc, tr = [], []

    with open('performance_vs_set_size_fixed_total_runtime.csv', 'r') as csvfile:
        plots = csv.DictReader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row['set_size']))
            tc.append((float(row['difference_rel'])))

    fig, ax = plt.subplots(1, 1)

    fig.legend(loc='lower center', labelspacing=0., ncol=3)
    ax.set_title('Performance vs set size (fixed total task runtime)')
    ax.set_ylabel('Relative difference, %')
    ax.set_xlabel('Set size')
    ax.text(0, 0.25, 'Fixed total task runtime = 1000 s')
    ax.text(0, 0.24, 'No parents equal test set')
    ax.plot(x, tc, label='Task manager spent time', marker='o', markersize=3, linewidth=1.2)
    plt.figure(4)
    fig.set_size_inches(11, 8)
    plt.savefig("performance_vs_set_size_fixed_total_runtime.svg", format="svg")


def perf_set_fixed_task_dur():
    x = []
    tc, tr = [], []

    with open('performance_vs_set_size_fixed_task_duraton.csv', 'r') as csvfile:
        plots = csv.DictReader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row['set_size']))
            tc.append((float(row['difference_rel'])))

    fig, ax = plt.subplots(1, 1)

    fig.legend(loc='lower center', labelspacing=0., ncol=3)
    ax.set_title('Performance vs set size (fixed task duration)')
    ax.set_ylabel('Relative difference, %')
    ax.set_xlabel('Set size')
    ax.text(0, 0.370, 'Fixed task duration = 1 ms')
    ax.text(0, 0.364, 'No parents equal test set')
    ax.plot(x, tc, label='Task manager spent time', marker='o', markersize=3, linewidth=1.2)
    plt.figure(5)
    fig.set_size_inches(11, 8)
    plt.savefig("performance_vs_set_size_fixed_task_duraton.svg", format="svg")

def perf_task_dur():
    x = []
    tc, tr = [], []

    with open('performance_vs_task_duration.csv', 'r') as csvfile:
        plots = csv.DictReader(csvfile, delimiter=',')
        for row in plots:
            x.append(float(row['task_duration_ms']))
            tc.append((float(row['difference_rel'])))

    fig, ax = plt.subplots(1, 1)

    fig.legend(loc='lower center', labelspacing=0., ncol=3)
    ax.set_title('Performance vs task duration')
    ax.set_ylabel('Relative difference, %')
    ax.set_xlabel('Task duration, ms')
    ax.text(100, 0.21, 'Fixed set size = 1000 tasks')
    ax.text(100, 0.20, 'No parents equal test set')
    ax.plot(x, tc, label='Task manager spent time', marker='o', markersize=3, linewidth=1.2)
    plt.figure(6)
    fig.set_size_inches(11, 8)
    plt.savefig("performance_vs_task_duration.svg", format="svg")


sort()
perf_thread(count='10', fign=2)
perf_thread(count='100', fign=3)
perf_set_fixed_runtime()
perf_set_fixed_task_dur()
perf_task_dur()
plt.show()
