
# importing the required module
import matplotlib.pyplot as plt
import csv
x=[]
y=[]
with open('results.csv', newline='') as f:
    reader = csv.reader(f)
    data = list(reader)

print(data)

for i in data:
	x.append(int(i[0]))
	k=float(i[2]);	
	y.append(k)
print(x)
print(y)

# plotting the points 
plt.plot(x, y)
  
# naming the x axis
plt.xlabel('Load')
# naming the y axis
plt.ylabel('Avg. Response Time')
  
# giving a title to my graph
plt.title('Avg. Response Time vs Load Plot')
  
# function to show the plot
plt.show()
