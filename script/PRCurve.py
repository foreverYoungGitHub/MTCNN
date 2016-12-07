import matplotlib.pyplot as plt
import numpy as np

def PRCurve(logfile):
    data = np.loadtxt(logfile)
    x = np.linspace(0, 30, data.size)
    with plt.style.context('ggplot'):
	plt.plot(x, data)

#main
PRCurve('/home/xileli/Documents/dateset/AFW/log/loss48c.log')
PRCurve('/home/xileli/Documents/dateset/AFW/log/loss48c_test.log')
PRCurve('/home/xileli/Documents/dateset/AFW/log/accuracy48c_test.log')
plt.show()

