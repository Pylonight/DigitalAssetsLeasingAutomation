pragma solidity ^0.4.18;

contract Billboard {
	address private owner;		// owner's address
	uint public rigidPrice;		// the reserved price, which the floating price cannot drop below
	uint public currentPrice;	// current price to 
	uint public lastPrice;		// payment made by the current lessee
	uint public expireTime;		// expire time of current lease term
	bool public isIdle;			// whether the billboard is idle
	address private ader;		// address of current lessee
	string public ad;			// the ad, maybe a URL
	bool public isFirstAder;	// whether there is no former lessee before this guy
	uint public deposit;		// deposit in the contract

	uint public REWARDFACTOR = 3;	// reward makes up 1/3 of delta payment
	uint public REVENUEFACTOR = 3;	// revenue makes up 1/3 of delta payment
	// the leftover is kept as reserved deposit, in case price drops
	uint public PRICEDROPFACTOR = 2;// price drop by 1/2 upon expiration
	uint public PRICERISEFACTORU = 11;	// numerator of factor which is the smallest ratio to raise the price
	uint public PRICERISEFACTORD = 10;	// dividor of factor which is the smallest ratio to raise the price

	// initialization
	function Billboard()
	    public
	{
		owner = msg.sender;
		rigidPrice = 1000000000000000000; //1000000000000000000 wei = 1 ether
		currentPrice = rigidPrice;
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
			revert();
			return false;
			// isIdle = true;
			// isFirstAder = true;
			// currentPrice /= PRICEDROPFACTOR;
			// if (currentPrice < rigidPrice) {
			// 	currentPrice = rigidPrice;
			// }
		}

		// install the new ad, kick-out the current lessee
		if (msg.value >= currentPrice) {
			uint previousDeposit = deposit;

			isIdle = false;
			uint toOwner = 0;
			if (!isFirstAder) {
				// if there is a current lessee
				uint delta = msg.value-lastPrice;
				// this amount of tokens is transferred to owner
				// if there is deposit, then tranfer all deposit to owner
				// as we now have new deposit
				toOwner = delta/REVENUEFACTOR;
				// this amount of tokens is transferred to current lessee
				// lastPrice as payment back, delta/REWARDFACTOR as reward
				ader.transfer(lastPrice+delta/REWARDFACTOR);
				// deposit kept in case price drops
				deposit = msg.value-delta/REVENUEFACTOR-delta/REWARDFACTOR;
			} else {
				// if there is no current lessee
				// owner will get reward part along with revenue part
				toOwner = msg.value/REWARDFACTOR+msg.value/REVENUEFACTOR;
				// keep this amount as reserved in case price drops
				deposit = msg.value-toOwner;
				isFirstAder = false;
			}
			// install the new ad
			ader = msg.sender;
			ad = newAd;
			owner.transfer(toOwner+previousDeposit);
			lastPrice = msg.value;
			currentPrice = msg.value*PRICERISEFACTORU/PRICERISEFACTORD;

			// for experiment, lease term is 2 minutes long only
			expireTime = now + 2 minutes;
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
		isIdle = true;
		isFirstAder = true;
		currentPrice /= PRICEDROPFACTOR;
		// can never drop below reserved price, says 1 Ether
		if (currentPrice < rigidPrice) {
			currentPrice = rigidPrice;
		}
		// only reward this guy with half the deposit, in case future drop
		uint reward = deposit/2;
		msg.sender.transfer(reward);
		deposit -= reward;

		// for experiment, lease term is 2 minutes long only
		expireTime = now + 2 minutes;
		return true;
	}

	function()
		public
	{
		revert();
	}
}