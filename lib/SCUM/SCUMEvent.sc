SCUMEvent : SCUM {
	var <view, <isAccepted=true;

	isIgnored { ^isAccepted.not }

	accept { isAccepted = true }
	ignore { isAccepted = false }

	properties {
		^[\view, view, \accepted, isAccepted]
	}

	printOn { | stream |
		super.printOn(stream);
		stream.put($();
		this.properties.printItemsOn(stream);
		stream.put($));
	}
}

SCUMInputEvent : SCUMEvent {
	var <state;

	// converting
	*specToModMask { | spec |
		^spec.asArray.inject(0, { | cur, nxt |
			if (nxt.isKindOf(Symbol)) {
				nxt = (
					C: SCUM.modControl,
					M: SCUM.modCommand,
					S: SCUM.modShift,
					K: SCUM.modKeypad
				).at(nxt) ? 0;
			};
			cur | nxt
		})
	}
	*modMaskToSpec { | mask |
		var spec;
		if (mask & modControl) { spec = spec.add(\C) };
		if (mask & modCommand) { spec = spec.add(\M) };
		if (mask & modShift)   { spec = spec.add(\S) };
		if (mask & modKeypad)  { spec = spec.add(\K) };
		^spec
	}

	// access
	stateSpec {
		^this.class.modMaskToSpec(state)
	}

	// testing
	hasMod { | modifier |
		^(state & modifier) == modifier
	}
	hasAllMods { | modifiers |
	}
	hasAnyMod { | modifiers |
	}

	// control structures
	ifMod { | modifier, trueFunc, falseFunc |
		^if (this.hasMod(modifier), trueFunc, falseFunc)
	}

	properties {
		^super.properties ++ [\state, this.stateSpec]
	}
}

SCUMKeyEvent : SCUMInputEvent {
	var <char, <key;

	*new { | view, state, char, key |
		^this.newCopyArgs(view, true, state, char, key)
	}

	ifChar { | argChar, trueFunc, falseFunc |
		^if (char == argChar, trueFunc, falseFunc)
	}
	ifChars { | array, trueFunc, falseFunc |
		^if (array.includes(char), trueFunc, falseFunc)
	}
	ifCharNoCase { | argChar, trueFunc, falseFunc |
		^if (char.toUpper == argChar.toUpper, trueFunc, falseFunc)
	}
	ifCharsNoCase { | array, trueFunc, falseFunc |
		^if (array.inject(true, { |c| char.toUpper == c.toUpper }),
			trueFunc, falseFunc
		)
	}
	ifKey { | argKey, trueFunc, falseFunc |
		^if (key == argKey, trueFunc, falseFunc)
	}
	ifKeys { | array, trueFunc, falseFunc |
		^if (array.includes(key), trueFunc, falseFunc)
	}
	ifNl { | trueFunc, falseFunc |
		if ((key == 16r0003) || (key == keyLF) || (key == keyCR),
			trueFunc, falseFunc
		);
	}

	properties {
		^super.properties ++ [\char, char, \key, key.asHexString]
	}
}

SCUMMouseEvent : SCUMInputEvent {
	var <pos;

	*new { | view, state, x, y |
		^this.newCopyArgs(view, true, state, Point(x, y))
	}

	properties {
		^super.properties ++ [\pos, pos]
	}
}

SCUMScrollWheelEvent : SCUMMouseEvent {
	var <delta;

	*new { | view, state, x, y, dx, dy |
		^this.newCopyArgs(view, true, state, Point(x, y), Point(dx, dy))
	}
	properties {
		^super.properties ++ [\delta, delta]
	}
}

SCUMContextMenuEvent : SCUMMouseEvent
{
}

// EOF