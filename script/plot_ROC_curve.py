import matplotlib.pyplot as plt


listX=[]
listY=[]
listLeg=[]
file = open("/Users/Young/projects/MTCNN/result/FDDB_evaluation/FDDB-folds/tempDiscROC.txt")

with plt.style.context('ggplot'):
    plt.figure("DiscRoc")
    plt.xlabel("False Positive Per Frame")
    plt.ylabel("True Positive Rate")
    for point in file:
        xy = point.split()
        if (float(xy[1]) < 8000):
            listX.append(int(xy[1])) # / float(2844))
            listY.append(xy[0])
    plt.plot(listX,listY)
    plt.legend(listLeg, prop={'size': 9})

listY=[]
listX=[]
listLeg=[]



#======================================


file = open("/Users/Young/projects/MTCNN/result/FDDB_evaluation/FDDB-folds/tempContROC.txt")


with plt.style.context('ggplot'):
    plt.figure("ContRoc")
    plt.xlabel("False Positive Per Frame")
    plt.ylabel("True Positive Rate")
    for point in file:
        xy = point.split()
        if (float(xy[1]) < 3000):
            listX.append(int(xy[1])) # / float(2844))
            listY.append(xy[0])
    plt.plot(listX,listY)
    plt.legend(listLeg, prop={'size': 10})



plt.show()