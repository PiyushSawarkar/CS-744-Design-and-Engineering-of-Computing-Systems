
# importing the required module
import matplotlib.pyplot as plt
import csv
x=[]
y=[]
with open('results.csv', newline='') as f:
    reader = csv.reader(f)
    data = list(reader)
#print(data)
#exit(0)
for i in data:
	x.append(i[0])	
	y.append(i[1])
  
# plotting the points 
plt.plot(x, y)
  
# naming the x axis
plt.xlabel('Load')
# naming the y axis
plt.ylabel('Avg. Throughput')
  
# giving a title to my graph
plt.title('Avg. Throughput vs Load Plot')
  
# function to show the plot
plt.show()
