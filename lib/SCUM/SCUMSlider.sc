SCUMSlider : SCUMControl {
	// initialization
	initDefaults {
		super.initDefaults;
		this
		.setKey(nil, $r, #{ | view | view.valueAction = 1.0.rand })
		.setKey(nil, $n, #{ | view | view.valueAction = 0.0 })
		.setKey(nil, $x, #{ | view | view.valueAction = 1.0 })
		.setKey(nil, $c, #{ | view | view.valueAction = 0.5 })
		.setKey(nil, $], #{ | view | view.increment })
		.setKey(nil, $[, #{ | view | view.decrement })
		.setKey(nil, SCUM.keyUp,   #{ | view | view.increment(1) })
		.setKey(nil, SCUM.keyDown, #{ | view | view.decrement(1) });
	}

	// access
	aspectRatio { ^this.subclassResponsibility }
	increment { | delta | ^this.valueAction = this.value + (delta * this.aspectRatio) }
	decrement { | delta | ^this.increment(delta.neg) }

	// copy/paste
	copyEvent { ^this.value }
	pasteEvent { | obj | if (obj.isKindOf(Number)) { this.valueAction = obj } }

	// events
	scrollWheel { | evt | this.decrement(evt.delta.y) }

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\step, \thumbSize, \steady, \orientation]
	}
	step { ^this.getProperty(\step) }
	step_ { |v| this.setProperty(\step, v) }
	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |v| this.setProperty(\thumbSize, v) }
	steady { ^this.getProperty(\steady) }
	steady_ { |v| this.setProperty(\steady, v) }
	orientation { ^this.getProperty(\orientation) }
	orientation_ { |v| this.setProperty(\orientation, v.asSymbol) }

	// rk control values (experimental)
	controlValue_ { arg ctl;
		this.value = ctl.input;
		this.action = { ctl.input = this.value };
		this.addResource(
			ActionListener(ctl, \synch, { { this.value = ctl.input }.defer })
		);
	}
}

SCUMVSlider : SCUMSlider {
	*viewClass { ^SCUMSlider }
	initDefaults {
		super.initDefaults;
		this.orientation = \vertical;
	}
	aspectRatio { ^this.bounds.height.reciprocal }
}

SCUMHSlider : SCUMSlider {
	*viewClass { ^SCUMSlider }
	initDefaults {
		super.initDefaults;
		this.orientation = \horizontal;
	}
	aspectRatio { ^this.bounds.width.reciprocal }
}

SCUMPad : SCUMControl {
	*propertyKeys {
		^super.propertyKeys ++ [\x, \y, \xStep, \yStep, \thumbSize, \steady]
	}
	
	// initialization
	initDefaults {
		super.initDefaults;
		this
		.setKey(nil, SCUM.keyUp,   #{ | view | view.increment })
		.setKey(nil, SCUM.keyDown, #{ | view | view.decrement });
	}

	// accessing
	value { ^this.prGetPointProperty(\x, \y) }
	value_ { |v| this.prSetPointProperty(\x, \y, v.asPoint) }
	valueAction_ { |v|
		var changed = false;
		v = v.asPoint;
		if (this.setProperty(\x, v.x) === true) { changed = true };
		if (this.setProperty(\y, v.y) === true) { changed = true };
		if (changed) {
			this.doAction;
			this.changed(\value);
		}
	}

	increment { | delta |
		var bounds;
		bounds = this.bounds;
		^this.valueAction =
		(bounds.width.reciprocal @ bounds.height.reciprocal) * delta + this.value;
	}
	decrement { | delta |
		^this.increment(0@0 - delta)
	}

	// events
	scrollWheel { | evt | this.decrement(evt.delta) }

	// properties
	x { ^this.getProperty(\x) }
	x_ { |v| this.setProperty(\x, v) }
	y { ^this.getProperty(\y) }
	y_ { |v| this.setProperty(\y, v) }
	xStep { ^this.getProperty(\xStep) }
	xStep_ { |v| this.setProperty(\xStep, v) }
	yStep { ^this.getProperty(\yStep) }
	yStep_ { |v| this.setProperty(\yStep, v) }
	step { ^this.prGetPointProperty(\xStep, \yStep) }
	step_ { |v| this.prSetPointProperty(\xStep, \yStep, v.asPoint) }
	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |v| this.setProperty(\thumbSize, v) }
	steady { ^this.getProperty(\steady) }
	steady_ { |v| this.setProperty(\steady, v) }
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

	style { ^this.getProperty(\style) }
	style_ { |v| this.setProperty(\style, v.asSymbol) }

	// PRIVATE
	prDestroyed {
		value = Signal.new;
		super.prDestroyed;
	}
}

// EOF