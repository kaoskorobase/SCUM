SCUMSlider : SCUMControl {
	// copy/paste
	copyEvent { ^this.value }
	pasteEvent { | obj | if (obj.isKindOf(Number)) { this.valueAction = obj } }

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\step, \thumbSize, \steady]
	}
	step { ^this.getProperty(\step) }
	step_ { |v| this.setProperty(\step, v) }
	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |v| this.setProperty(\thumbSize, v) }
	steady { ^this.getProperty(\steady) }
	steady_ { |v| this.setProperty(\steady, v) }

	// rk control values (experimental)
	controlValue_ { arg ctl;
		this.value = ctl.input;
		this.action = { ctl.input = this.value };
		this.addResource(
			ActionListener(ctl, \synch, { { this.value = ctl.input }.defer })
		);
	}
}

SCUMPad : SCUMControl {
	*propertyKeys {
		^super.propertyKeys ++ [\step, \steady]
	}
	
	value { ^this.prGetPointProperty(\x, \y) }
	value_ { |v| this.prSetPointProperty(\x, \y, v.asPoint) }
	valueAction_ { |v|
		var changed;
		v = v.asPoint;
		if ((this.setProperty(\x, v.x) === true) | (this.setProperty(\y, v.y) === true)) {
			this.doAction;
			this.changed(\value);
		}
	}

	x { ^this.getProperty(\x) }
	x_ { |v| this.setProperty(\x, v) }
	y { ^this.getProperty(\y) }
	y_ { |v| this.setProperty(\y, v) }
	pos { ^this.prGetPointProperty(\x, \y) }
	pos_ { |v| this.prSetPointProperty(\x, \y, v.asPoint) }
	xStep { ^this.getProperty(\xStep) }
	xStep_ { |v| this.setProperty(\xStep, v) }
	yStep { ^this.getProperty(\yStep) }
	yStep_ { |v| this.setProperty(\yStep, v) }
	step { ^this.prGetPointProperty(\xStep, \yStep) }
	step_ { |v| this.prSetPointProperty(\xStep, \yStep, v.asPoint) }
}

SCUMTable : SCUMControl {
	var <value;

	initDefaults {
		super.initDefaults;
		value = Signal.new;
		this.update;
	}

	value_ { |v|
		value = v.asSignal;
		this.update;
	}
	update {
		this.setProperty(\value, value)
	}
	do { | function |
		function.value(value);
		this.update;
	}
	doMsg { | msg ... args |
		value.performList(msg, args);
		this.update;
	}

	// PRIVATE
	prDestroyed {
		value = Signal.new;
		super.prDestroyed;
	}
}

// EOF