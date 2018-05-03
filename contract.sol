pragma solidity ^0.4.18;

contract Billboard {
	address private owner;		// owner's address
	uint public rigidPrice;		// the reserved price, which the floating price cannot drop below
	uint public currentPrice;	// current price to kick-out current lessee
	uint public lastPrice;		// payment made by the current lessee
	uint public realPrice;		// real payment current lessee should pay
	uint public expireTime;		// expire time of current lease term
	bool public isIdle;			// whether the billboard is idle
	address private ader;		// address of current lessee
	string public ad;			// the ad, maybe a URL
	bool public isFirstAder;	// whether there is no former lessee before this guy
	uint public deposit;		// deposit in the contract

	//uint public REWARDFACTOR = 3;	// reward makes up 1/3 of delta payment
	//uint public REVENUEFACTOR = 3;	// revenue makes up 1/3 of delta payment
	uint public RESERVEDFACTOR = 100;	// reserved makes up 1/100 of 
	// the leftover is kept as reserved deposit, in case price drops
	uint public PRICEDROPFACTOR = 2;// price drop by 1/2 upon expiration
	uint public PRICERISEFACTORU = 11;	// numerator of factor which is the smallest ratio to raise the price
	uint public PRICERISEFACTORD = 10;	// dividor of factor which is the smallest ratio to raise the price
	uint public TERMLENGTHINSECOND = 600;

	// initialization
	constructor()
	    public
	{
		owner = msg.sender;
		rigidPrice = 1000000000000000000; //1000000000000000000 wei = 1 ether
		currentPrice = rigidPrice;
		lastPrice = currentPrice;
		realPrice = 0;
		expireTime = now;
		isIdle = true;
		isFirstAder = true;
		deposit = 0;
	}

	// only callable when the billboard is occupied
	modifier onlyWhenOccupied() {
		require(!isIdle);
		_;
	}

	// only callable when the lease term ends
	modifier onlyAfterExpiration() {
		require(now >= expireTime);
		_;
	}

	modifier onlyOwnerCanCall() {
		require(msg.sender == owner);
		_;
	}

	// make an offer with newAd as the ad to install
	// returns whether it succeeds to rent the ad
	function Offer(string newAd)
		public
		payable
		returns(bool)
	{
		// the last lease term already ends
		// but no one calls the expire()
		if (!isIdle && now >= expireTime) {
			//revert();
			//return false;
			ader.transfer(lastPrice-realPrice);
			deposit = realPrice;

			isIdle = true;
			isFirstAder = true;
			ad = "";
			currentPrice /= PRICEDROPFACTOR;
			if (currentPrice < rigidPrice) {
				currentPrice = rigidPrice;
			}
			lastPrice = currentPrice;
			realPrice = 0;

			uint reward = deposit/RESERVEDFACTOR;
			owner.transfer(reward);
			deposit -= reward;

			expireTime = now+TERMLENGTHINSECOND;
		}

		// install the new ad, kick-out the current lessee
		if (msg.value >= currentPrice) {
			// uint previousDeposit = deposit;

			isIdle = false;
			uint toOwner = 0;
			// uint delta = msg.value-lastPrice;
			if (!isFirstAder) {
				// if there is a current lessee
				// this amount of tokens is transferred to owner
				// if there is deposit, then tranfer all deposit to owner
				// as we now have new deposit
				// toOwner = delta/REVENUEFACTOR;
				uint occupiedLength = now+TERMLENGTHINSECOND-expireTime;
				if (occupiedLength > TERMLENGTHINSECOND) {
					occupiedLength = TERMLENGTHINSECOND;
				}
				toOwner = realPrice*occupiedLength/TERMLENGTHINSECOND;
				// this amount of tokens is transferred to current lessee
				// lastPrice as payment back, delta/REWARDFACTOR as reward
				// ader.transfer(lastPrice+delta/REWARDFACTOR);
				ader.transfer(lastPrice-toOwner);
				// deposit kept in case price drops
				// deposit = delta-delta/REVENUEFACTOR-delta/REWARDFACTOR;
			} else {
				// if there is no current lessee
				// owner will get reward part along with revenue part
				// toOwner = msg.value/REWARDFACTOR+msg.value/REVENUEFACTOR;
				// keep this amount as reserved in case price drops
				// deposit = delta-toOwner;
				isFirstAder = false;
				toOwner = deposit;
			}
			// install the new ad
			ader = msg.sender;
			ad = newAd;
			owner.transfer(toOwner);
			deposit = msg.value;
			realPrice = lastPrice;
			lastPrice = msg.value;
			currentPrice = msg.value*PRICERISEFACTORU/PRICERISEFACTORD;

			// for experiment, lease term is 2 minutes long only
			expireTime = now+TERMLENGTHINSECOND;
			return true;
		} else {
			revert();
			return false;
		}
	}

	// call the contract to drop the price when a lease term ends
	// return whether it succeeds to make it expire
	function Expire()
		public
		onlyAfterExpiration
		returns(bool)
	{
		if (!isIdle) {
			ader.transfer(lastPrice-realPrice);
			deposit = realPrice;
		}
		isIdle = true;
		isFirstAder = true;
		ad = "";
		currentPrice /= PRICEDROPFACTOR;
		// can never drop below reserved price, says 1 Ether
		if (currentPrice < rigidPrice) {
			currentPrice = rigidPrice;
		}
		lastPrice = currentPrice;
		realPrice = 0;
		// only reward this guy with half the deposit, in case future drop
		uint reward = deposit/RESERVEDFACTOR;
		msg.sender.transfer(reward);
		deposit -= reward;

		// for experiment, lease term is 2 minutes long only
		expireTime = now+TERMLENGTHINSECOND;
		return true;
	}

	// TOREMOVE: shortcut to reset the bollboard only for experimental use
	function Reset()
		public
		onlyOwnerCanCall
		returns(bool)
	{
		currentPrice = rigidPrice;
		lastPrice = currentPrice;
		realPrice = 0;
		expireTime = now;
		isIdle = true;
		isFirstAder = true;
		ad = "";
		owner.transfer(deposit);
		deposit = 0;
		return true;
	}

	function()
		public
	{
		revert();
	}
}