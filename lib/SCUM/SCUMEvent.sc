SCUMEvent : SCUM
{
	var <view, <isAccepted=true;

	isIgnored { ^isAccepted.not }

	accept { isAccepted = true }
	ignore { isAccepted = false }
}

SCUMInputEvent : SCUMEvent
{
	var <state;

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
}

SCUMKeyEvent : SCUMInputEvent
{
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
}

SCUMMouseEvent : SCUMInputEvent
{
	var <pos;

	*new { | view, state, x, y |
		^this.newCopyArgs(view, true, state, Point(x, y))
	}
}

SCUMScrollWheelEvent : SCUMMouseEvent
{
	var <delta;

	*new { | view, state, x, y, dx, dy |
		^this.newCopyArgs(view, true, state, Point(x, y), Point(dx, dy))
	}
}

SCUMContextMenuEvent : SCUMMouseEvent
{
}

// EOF