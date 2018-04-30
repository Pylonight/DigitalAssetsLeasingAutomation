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

	constType REWARDFACTOR = 3;
	constType REVENUEFACTOR = 3;
	constType PRICEDROPFACTOR = 2;
	constType PRICERISEFACTORU = 11;
	constType PRICERISEFACTORD = 10;

	Billboard(address sender, timeType termInSeconds) {
		owner = sender;
		rigidPrice = 1;
		currentPrice = rigidPrice;
		expireTime = time(0);
		this->termInSeconds = termInSeconds;
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
			if (!transfer(sender, addr, value)) {
				return false;
			}

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

void listAllWallets(Entity &owner, vector<Entity *> &candidates, Billboard &billboard) {
	cout << "Owner: " << owner.deposit << endl;
	cout << "Alice: " << candidates[0]->deposit << endl;
	cout << "Boris: " << candidates[1]->deposit << endl;
	cout << "Cindy: " << candidates[2]->deposit << endl;
	cout << "David: " << candidates[3]->deposit << endl;
	cout << "BoardBalance: " << billboard.deposit << endl;
	cout << "CurrentPrice: " << billboard.currentPrice << endl;
	cout << "Billboard: " << billboard.ad << endl;
	cout << endl;
}

int main() {
	Entity owner;
	Billboard billboard(owner.addr, 2);
	vector<Entity *> candidates(4, NULL);
	for (int i = 0; i < candidates.size(); ++i) {
		candidates[i] = new Entity();
	}
	candidates[0]->deposit = 3;
	candidates[1]->deposit = 6;
	candidates[2]->deposit = 9;
	candidates[3]->deposit = 0;
	listAllWallets(owner, candidates, billboard);

	cout << billboard.Offer(candidates[0]->addr, 3, "Alice") << endl;
	listAllWallets(owner, candidates, billboard);

	cout << billboard.Offer(candidates[1]->addr, 6, "Boris") << endl;
	listAllWallets(owner, candidates, billboard);

	cout << billboard.Offer(candidates[2]->addr, 9, "Cindy") << endl;
	listAllWallets(owner, candidates, billboard);

	Sleep(2000);
	cout << billboard.Expire(candidates[3]->addr) << endl;
	listAllWallets(owner, candidates, billboard);
	return 0;
}