Promise {
	var value, cond, isWaiting;
	
	*new {
		^this.newCopyArgs(nil, Condition.new, false)
	}

	// testing
	isFullfilled {
		^cond.test
	}
	canWait {
		^thisThread.isKindOf(Routine)
	}

	// accessing
	value {
		if (this.isFullfilled) {
			^value
		};
		if (this.canWait) {
			isWaiting = true;
			cond.wait;
			^value
		};
		Error("promised value unset").throw
	}
	value_ { | argValue |
		if (this.isFullfilled.not) {
			cond.test = true;
			value = argValue;
			if (isWaiting) {
				cond.signal;
			}
		}
	}
}
