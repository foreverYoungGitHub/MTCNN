import matplotlib.pyplot as plt
import numpy as np


data = np.loadtxt('./result/record-cpu.txt')
x = np.linspace(0, 30, data.size)
data_mean = [np.mean(data) for i in x]
with plt.style.context('ggplot'):
    plt.plot(x, data, label='Data_CPU')
    plt.plot(x, data_mean, label='Mean_CPU')
    plt.text(-1, 0.28, 0.2813, fontsize=12)

data = np.loadtxt('./result/record-gpu.txt')
x = np.linspace(0, 30, data.size)
data_mean = [np.mean(data) for i in x]
with plt.style.context('ggplot'):
    plt.plot(x, data, label='Data_GPU')
    plt.plot(x, data_mean, label='Mean_GPU')
    plt.text(-1, 0.02, 0.0209, fontsize=12)

plt.show()