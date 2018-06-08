from pylab import *

x = []
y1 = []
y2 = []
with open('noRewardBidder.txt', 'r') as f:
	lines = f.readlines()

	for line in lines:
		splits = line.split()
		pair = map(float, splits)
		x.append(pair[0])
		y1.append(pair[1])
		y2.append(pair[2])
		print pair

plot(x, y2, 'b^-', label='v1 (Vickrey)')
plot(x, y1, 'ro-', label='v2 (proportional refund)')
title('Vickrey vs Kick-out v2 on Owner\'s Revenue\nValuation drawn from Normal Distribution(100, 30)')
xlabel('Auction Period(% of A Lease Term)')
# xlabel('Length of Simulation')
ylabel('Owner\'s Revenue')
legend()
show()

# plot(x, y1, 'r', label='type')
# plot(x, y1, 'ro')
# title('Revenue of Vickrey / Revenue of v2\nValuation drawn from Normal Distribution(100, 30)')
# xlabel('Auction Period(% of A Lease Term)')
# xlabel('Length of Simulation')
# ylabel('Revenue Ratio(%)')
# show()