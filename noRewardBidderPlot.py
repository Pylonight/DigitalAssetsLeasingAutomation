from pylab import *

x = []
y = []
with open('noRewardBidder.txt', 'r') as f:
	lines = f.readlines()

	for line in lines:
		splits = line.split()
		pair = map(float, splits)
		x.append((pair[0]-1)*100)
		y.append(pair[1])
		print pair

plot(x, y, 'r--', label='type')
plot(x, y, 'ro-')
title('Winner\'s Revenue vs Ambition Modifier\nNormal Distribution(100, 30), Without Reward')
xlabel('Ambition Modifier(%)')
ylabel('Winner\'s Revenue(%)')
show()