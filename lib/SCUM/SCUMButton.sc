SCUMToggle : SCUMControl {
	var <value;
	
	*initClass {
		this.addProperty(\value, false)
	}
	initDefaults {
		super.initDefaults;
		value = this.class.propertyDefault(\value);
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
	value_ { |v|
		this.setPropertyChanged(\value, v) { value = v }
	}
	prSet_value { |v| this.value_(v != 0) }
}

SCUMBang : SCUMControl {
	*initClass {
		this.propertyDefaults.put(\flashTime, 0.2)
	}
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\flashTime]
	}
	flashTime { ^this.getProperty(\flashTime) }
	flashTime_ { |v| this.setProperty(\flashTime, v.asFloat) }
	
	// PRIVATE
	prSet_value {
		this.doActionUnlessMuted(\bang)
	}
}

SCUMButton : SCUMControl {
	var <value;
	
	*initClass {
		this.propertyDefaults.putAll((
			value: 0,
			font: { SCUM.desktop.font },
			xPadding: 0,
			yPadding: 0,
			textAlignment: 5,
			states: #[]
		));
	}

	initDefaults {
		super.initDefaults;
		value = this.class.propertyDefault(\value);
	}
	
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\font, \padding, \states]
	}
	value_ { |v|
		v = v.wrap(0, this.states.size - 1);
		this.setPropertyChanged(\value, v, onlyIfChanged: false) { value = v }
	}
	font { ^this.getProperty(\font) }
	font_ { |v| this.setProperty(\font, v); ^v }
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v); ^v }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v); ^v }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint); ^v }
	textAlignment { ^this.getProperty(\textAlignment) }
	textAlignment_ { |v| this.setProperty(\textAlignment, v); ^v }
	states { ^this.getProperty(\states) }
	states_ { |v|
		// [[text fgColor bgColor], [...]]
		v = v.asArray.collect { |state| SCUMButtonState(*state) };
		this.setProperty(\states, v);
	}
	
	// PRIVATE
	prSet_value { |v| this.value_(v) }
}

SCUMChoice : SCUMControl {
	var <value;
	
	*initClass {
		this.propertyDefaults.putAll((
			value: 0,
			font: { SCUM.desktop.font },
			xPadding: 0,
			yPadding: 0,
			textAlignment: 5,
			states: #[]
		));
	}

	initDefaults {
		super.initDefaults;
		value = this.class.propertyDefault(\value);
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\font, \padding, \states]
	}
	value_ { |v|
		v = v.asInteger.clip(0, this.states.size);
		this.setPropertyChanged(\value, v) { value = v }
	}
	font { ^this.getProperty(\font) }
	font_ { |v| this.setProperty(\font, v) }
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v) }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v) }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint) }
	textAlignment { ^this.getProperty(\textAlignment) }
	textAlignment_ { |v| this.setProperty(\textAlignment, v) }
	states { ^this.getProperty(\states) }
	states_ { |v| this.setProperty(\states, v) }
	
	// PRIVATE
	prSet_value { |v| this.value_(v) }
}

// EOF