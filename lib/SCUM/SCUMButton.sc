SCUMToggle : SCUMControl
{
	initDefaults {
		super.initDefaults;
		this.setKey(nil, keySpace, #{ |v| v.value = v.value.not });
		this.setKey(nil, keyCR, #{ |v| v.value = true });
		this.setKey(nil, keyBackSpace, #{ |v| v.value = false });
	}
	// events
// 	defaultKeyDownAction { | evt |
// 		evt.ifKey(keySpace, { this.value = this.value.not; ^this });
// 		evt.ifNl({ this.value = true; ^this });
// 		evt.ifKey(keyBackSpace, { this.value = false; ^this });
// 		super.defaultKeyDownAction(evt);
// 	}
}

SCUMBang : SCUMControl
{
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\flashTime]
	}
	flashTime { ^this.getProperty(\flashTime) }
	flashTime_ { |v| this.setProperty(\flashTime, v) }
}

SCUMButton : SCUMControl
{
	var <font, <states;

	initDefaults {
		super.initDefaults;
		this.xFill = 1;
		this.font = SCUMDesktop.font;
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\font, \padding, \states]
	}
	font_ { |v| font = v; this.setProperty(\font, v) }
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v) }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v) }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint) }
	textAlignment { ^this.getProperty(\textAlignment) }
	textAlignment_ { |v| this.setProperty(\textAlignment, v) }
	states_ { | array |
		// text fgColor bgColor
		states = array;
		this.setProperty(\states, states);
	}
}

SCUMChoice : SCUMControl
{
	var <font, <states;

	initDefaults {
		this.font = SCUMDesktop.font;
		this.fgColor = SCUMDesktop.fgColor;
		this.bgColor = SCUMDesktop.bgColor;
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\font, \padding, \states]
	}
	font_ { |v| font = v; this.setProperty(\font, v) }
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v) }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v) }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint) }
	textAlignment { ^this.getProperty(\textAlignment) }
	textAlignment_ { |v| this.setProperty(\textAlignment, v) }
	states_ { | array |
		states = array;
		this.setProperty(\states, states);
	}
}

// EOF