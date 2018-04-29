#include <iostream>
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

using namespace std;

typedef int address;
typedef double tokenType;
typedef time_t timeType;
typedef int constType;

class Entity;
class Contract;

unordered_map<address, Entity *> entityList;
unordered_map<address, Contract *> contractList;
void transfer(address from, address to, tokenType value);

class Entity {
private:

public:
	address addr;
	tokenType deposit;

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

class Billboard : Contract {
private:
	address owner;
	address ader;

public:
	tokenType rigidPrice;
	tokenType currentPrice;
	tokenType lastPrice;
	timeType expireTime;
	bool isIdle;
	string ad;
	bool isFirstAder;

	constType REWARDFACTOR = 3;
	constType REVENUEFACTOR = 3;
	constType PRICEDROPFACTOR = 2;
	constType PRICERISEFACTORU = 11;
	constType PRICERISEFACTORD = 10;

	Billboard(address sender) {
		owner = sender;
		rigidPrice = 1;
		currentPrice = rigidPrice;
		expireTime = time(0);
		isIdle = true;
		isFirstAder = true;
		deposit = 0;
	}

	bool Offer(address sender, tokenType value, string newAd) {
		if (!isIdle && time(0) >= expireTime) {
			return false;
		}

		if (value >= currentPrice) {
			tokenType previousDeposit = deposit;

			isIdle = false;
			tokenType toOwner = 0;
			if (!isFirstAder) {
				tokenType delta = value-lastPrice;
				toOwner = delta/REVENUEFACTOR;
				transfer(addr, ader, lastPrice+delta/REWARDFACTOR);
			} else {
				toOwner = value/REWARDFACTOR+value/REVENUEFACTOR;
				isFirstAder = false;
			}
			ader = sender;
			ad = newAd;
			transfer(addr, owner, toOwner+previousDeposit);
			lastPrice = value;
			currentPrice = value*PRICERISEFACTORU/PRICERISEFACTORD;

			expireTime = time(0)+120;
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
		currentPrice /= PRICEDROPFACTOR;
		if (currentPrice < rigidPrice) {
			currentPrice = rigidPrice;
		}
		tokenType reward = deposit/2;
		transfer(addr, sender, reward);

		expireTime = time(0)+120;
		return true;
	}

	~Billboard() {

	}
};

void transfer(address from, address to, tokenType value) {
	if (entityList.find(from) != entityList.end()) {
		entityList[from]->deposit -= value;
	} else if (contractList.find(from) != contractList.end()) {
		contractList[from]->deposit -= value;
	} else {
		return;
	}

	if (entityList.find(to) != entityList.end()) {
		entityList[to]->deposit += value;
	} else if (contractList.find(to) != contractList.end()) {
		contractList[to]->deposit += value;
	} else {
		return;
	}
}

int main() {
	
	return 0;
}