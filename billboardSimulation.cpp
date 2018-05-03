#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <regex>
#include <iterator>
#include <climits>
#include <string>
#include <cstdio>
#include <random>
#include <windows.h>

using namespace std;

typedef int address;
typedef double tokenType;
typedef time_t timeType;
typedef double constType;

class Entity;
class Contract;

unordered_map<address, Entity *> entityList;
unordered_map<address, Contract *> contractList;
bool transfer(address from, address to, tokenType value);

class Entity {
private:

public:
	address addr;
	tokenType deposit;
	tokenType valuation;
	string ad;

	Entity() {
		for (;;) {
			addr = (address)rand();
			if (entityList.find(addr) == entityList.end() && contractList.find(addr) == contractList.end()) {
				break;
			}
		}
		deposit = 0;
		entityList[addr] = this;
	}

	~Entity() {

	}
};

class Contract {
private:

public:
	address addr;
	tokenType deposit;

	Contract() {
		for (;;) {
			addr = (address)rand();
			if (contractList.find(addr) == contractList.end() && entityList.find(addr) == entityList.end()) {
				break;
			}
		}
		deposit = 0;
		contractList[addr] = this;
	}

	~Contract() {

	}
};

class Billboard : public Contract {
private:
	address owner;
	address ader;

public:
	tokenType rigidPrice;
	tokenType currentPrice;
	tokenType lastPrice;
	timeType expireTime;
	timeType termInSeconds;
	bool isIdle;
	string ad;
	bool isFirstAder;

	constType REWARDORNOT = 0;
	constType REWARDFACTOR = 3;
	constType REVENUEFACTOR = 3;
	constType PRICEDROPFACTOR = 2;
	constType PRICERISEFACTORU = 11;
	constType PRICERISEFACTORD = 10;
	double RMINFACTOR = pow(1.1, 9.)/10.;

	Billboard(address sender, timeType termInSeconds) {
		owner = sender;
		rigidPrice = 1;
		currentPrice = rigidPrice;
		expireTime = time(0);
		this->termInSeconds = termInSeconds;
		isIdle = true;
		isFirstAder = true;
		deposit = 0;
		ader = -1;
		ad = "";
	}

	bool Offer(address sender, tokenType value, string newAd) {
		if (!isIdle && time(0) >= expireTime) {
			return false;
		}

		if (value >= currentPrice) {
			tokenType previousDeposit = deposit;
			if (!transfer(sender, addr, value)) {
				return false;
			}

			isIdle = false;
			tokenType toOwner = 0;
			if (!isFirstAder) {
				tokenType delta = value-lastPrice;
				toOwner = delta/REVENUEFACTOR+delta*(1-REWARDORNOT)/REWARDFACTOR;
				transfer(addr, ader, lastPrice+delta*REWARDORNOT/REWARDFACTOR);
			} else {
				toOwner = value/REWARDFACTOR+value/REVENUEFACTOR;
				isFirstAder = false;
			}
			ader = sender;
			ad = newAd;
			transfer(addr, owner, toOwner+previousDeposit);
			lastPrice = value;
			currentPrice = value*PRICERISEFACTORU/PRICERISEFACTORD;

			expireTime = time(0)+termInSeconds;
			return true;
		} else {
			return false;
		}
	}

	bool Expire(address sender) {
		if (time(0) < expireTime) {
			return false;
		}
		isIdle = true;
		isFirstAder = true;
		ad = "";
		currentPrice /= PRICEDROPFACTOR;
		if (currentPrice < rigidPrice) {
			currentPrice = rigidPrice;
		}
		tokenType reward = deposit/2;
		transfer(addr, sender, reward);

		expireTime = time(0)+termInSeconds;
		return true;
	}

	address GetAderAddr() {
		return ader;
	}

	~Billboard() {

	}
};

bool transfer(address from, address to, tokenType value) {
	if (entityList.find(from) != entityList.end()) {
		if (entityList[from]->deposit >= value) {
			entityList[from]->deposit -= value;
		} else {
			return false;
		}
	} else if (contractList.find(from) != contractList.end()) {
		if (contractList[from]->deposit >= value) {
			contractList[from]->deposit -= value;
		} else {
			return false;
		}
	} else {
		return false;
	}

	if (entityList.find(to) != entityList.end()) {
		entityList[to]->deposit += value;
	} else if (contractList.find(to) != contractList.end()) {
		contractList[to]->deposit += value;
	} else {
		return false;
	}

	return true;
}

struct TraceLog {
	bool logSwitch;

	TraceLog() {
		logSwitch = false;
	}

	void TurnOn() {
		logSwitch = true;
	}

	void TurnOff() {
		logSwitch = false;
	}

	~TraceLog() {

	}
};

template <typename T>
TraceLog &operator<<(TraceLog &o, T const &x) {
	if (o.logSwitch) {
		cout << x;
	}
	return o;
}

void listAllWallets(TraceLog &traceLog, Entity &owner, vector<Entity *> &candidates, Billboard &billboard) {
	traceLog << "Owner: " << owner.deposit << "\n";
	for (int i = 0; i < candidates.size(); ++i) {
		traceLog << candidates[i]->ad << ": " << candidates[i]->valuation << "\n";
	}
	traceLog << "BoardBalance: " << billboard.deposit << "\n";
	traceLog << "CurrentPrice: " << billboard.currentPrice << "\n";
	traceLog << "Billboard: " << billboard.ad << "\n";
	traceLog << "\n";
}

int main() {
	const string names[] = {"Alice", "Boris", "Cindy", "David", "Ethan", "Floyd", "Gauss",
							"Harry", "Issac", "Jacob", "Kevin", "Louis", "Menlo", "Naomi",
							"Oscar", "Pasai", "Quinn", "Renee", "Shawn", "Talia",
							"Uthel", "Venus", "Wayne", "Xueer", "Yousa", "Zelda"};

	srand(time(0));

	TraceLog traceLog;
	//traceLog.TurnOn();
	ofstream fout("noRewardBidder.txt");

	int nBidders = 10;
	default_random_engine generator(time(0));
	normal_distribution<double> distribution(100.0, 30.0);
	traceLog << distribution(generator) << "\n";

	int trialNumber = 10000;

	const double naiveModifiers[] = {1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5};
	// const double naiveModifiers[] = {1.1};
	const int naiveModifiersLength = sizeof(naiveModifiers)/sizeof(naiveModifiers[0]);

	for (int naive = 0; naive < naiveModifiersLength; ++naive) {
		tokenType netWorthPercentSum = 0;

		for (int trial = 0; trial < trialNumber; ++trial) {
			entityList.clear();
			contractList.clear();
			int nameIndex = 0;
			Entity owner;
			Billboard billboard(owner.addr, 2);
			vector<Entity *> candidates(nBidders, NULL);
			for (int i = 0; i < candidates.size(); ++i) {
				candidates[i] = new Entity();
				candidates[i]->valuation = distribution(generator);
				if (candidates[i]->valuation < 0) {
					candidates[i]->valuation = 0;
				}
				candidates[i]->deposit = 10000;
				candidates[i]->ad = names[nameIndex++];
			}
			listAllWallets(traceLog, owner, candidates, billboard);

			vector<Entity *> bidders(candidates.begin(), candidates.end());

			// select a guy willing to pay more
			int naiveBidderIndex = rand()%bidders.size();
			address naiveBidderAddr = bidders[naiveBidderIndex]->addr;
			traceLog << "Naive: " << bidders[naiveBidderIndex]->ad << " " << bidders[naiveBidderIndex]->valuation;

			// right-away
			while (bidders.size() > 1) {
				int thisBidderIndex = rand()%bidders.size();
				if (bidders[thisBidderIndex]->addr == billboard.GetAderAddr()) {
					continue;
				}
				if (billboard.currentPrice*billboard.RMINFACTOR >= bidders[thisBidderIndex]->valuation*(naiveBidderAddr == bidders[thisBidderIndex]->addr ? naiveModifiers[naive] : 1.)) {
					bidders.erase(bidders.begin()+thisBidderIndex);
					continue;
				}
				billboard.Offer(bidders[thisBidderIndex]->addr, billboard.currentPrice, bidders[thisBidderIndex]->ad);
				//
			}
			// formal-terms
			int soloTermCount = 1;
			tokenType averagePerTerm = billboard.lastPrice;
			tokenType firstTermPrice = billboard.lastPrice;
			// for (;;) {
			// 	if (billboard.currentPrice/(soloTermCount+1) <= averagePerTerm) {
			// 		++soloTermCount;
			// 		averagePerTerm = billboard.currentPrice/soloTermCount;
			// 		billboard.Offer(bidders[0]->addr, billboard.currentPrice, bidders[0]->ad);
			// 	} else {
			// 		break;
			// 	}
			// 	traceLog << "Owner: " << owner.deposit+billboard.deposit << " For " << soloTermCount << " Terms, Avg: " << averagePerTerm << "\n";
			// 	traceLog << "Ader net worth: " << bidders[0]->valuation*soloTermCount-billboard.lastPrice << "\n";
			// }

			traceLog << "\n";
			traceLog << bidders[0]->ad << ": " << bidders[0]->deposit << " \t" << bidders[0]->valuation << "\n";
			traceLog << "Owner: " << owner.deposit+billboard.deposit << " For " << soloTermCount << " Terms, Avg: " << averagePerTerm << "\n";
			traceLog << "Ader net worth: " << bidders[0]->valuation*soloTermCount-billboard.lastPrice << "\n";

			// extension-terms
			for (;;) {
				if (billboard.currentPrice <= billboard.lastPrice+bidders[0]->valuation) {
					++soloTermCount;
					averagePerTerm = billboard.currentPrice/soloTermCount;
					billboard.Offer(bidders[0]->addr, billboard.currentPrice, bidders[0]->ad);
				} else {
					break;
				}
				traceLog << "Owner: " << owner.deposit+billboard.deposit << " For " << soloTermCount << " Terms, Avg: " << averagePerTerm << "\n";
				traceLog << "Ader net worth: " << bidders[0]->valuation*soloTermCount-billboard.lastPrice << "\n";
			}
			traceLog << bidders[0]->ad << ": " << bidders[0]->valuation << "\n";
			traceLog << "Owner: " << owner.deposit+billboard.deposit << " For " << soloTermCount << " Terms, Avg: " << averagePerTerm << "\n";
			traceLog << "Ader net worth: " << bidders[0]->valuation*soloTermCount-billboard.lastPrice << "\n";

			tokenType secondHighestValuation = 0;
			for (int i = 0; i < candidates.size(); ++i) {
				if (secondHighestValuation < candidates[i]->valuation && candidates[i]->addr != bidders[0]->addr) {
					secondHighestValuation = candidates[i]->valuation;
				}
			}
			// cout << bidders[0]->valuation << "\t" << secondHighestValuation << "\t" << firstTermPrice << "\t" << soloTermCount << "\t" <<
			// 	averagePerTerm << "\t" << bidders[0]->valuation-averagePerTerm << "\t" << (bidders[0]->valuation-averagePerTerm)/bidders[0]->valuation*100 << "\n";

			netWorthPercentSum += (bidders[0]->valuation-averagePerTerm)/bidders[0]->valuation;

			// int moreTerms = 5;
			// traceLog << "\n";
			// traceLog << "Possible " << moreTerms <<  " more terms" << "\n";
			// for (int i = 0; i < moreTerms; ++i) {
			// 	billboard.Offer(bidders[0]->addr, billboard.currentPrice, bidders[0]->ad);
			// 	++soloTermCount;
			// 	averagePerTerm = billboard.lastPrice/soloTermCount;
			// 	traceLog << "Owner: " << owner.deposit+billboard.deposit << " For " << soloTermCount << " Terms, Avg: " << averagePerTerm << "\n";
			// 	traceLog << "Ader net worth: " << bidders[0]->valuation*soloTermCount-billboard.lastPrice << "\n";
			// }

			for (int i = 0; i < candidates.size(); ++i) {
				delete candidates[i];
			}
		}

		cout << naiveModifiers[naive] << "\t" << netWorthPercentSum/trialNumber*100 << "\n";
		fout << naiveModifiers[naive] << "\t" << netWorthPercentSum/trialNumber*100 << "\n";
	}

	fout.close();
	return 0;
}