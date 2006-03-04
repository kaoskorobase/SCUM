SCUMSlider : SCUMControl {
	var <value;
	
	*initClass {
		this.propertyDefaults.putAll((
			value: 0.0,
			step: 0.0,
			thumbSize: 5,
			steady: false
		));
	}

	// initialization
	initDefaults {
		super.initDefaults;
		value = this.class.propertyDefault(\value);
		this
			.setKey(nil, $r, #{ | view | view.value = 1.0.rand })
			.setKey(nil, $n, #{ | view | view.value = 0.0 })
			.setKey(nil, $x, #{ | view | view.value = 1.0 })
			.setKey(nil, $c, #{ | view | view.value = 0.5 })
			.setKey(nil, $], #{ | view | view.increment })
			.setKey(nil, $[, #{ | view | view.decrement })
			.setKey(nil, SCUM.keyUp,   #{ | view | view.increment })
			.setKey(SCUM.modShift, SCUM.keyUp,   #{ | view | view.increment(10) })
			.setKey(nil, SCUM.keyDown, #{ | view | view.decrement })
			.setKey(SCUM.modShift, SCUM.keyDown, #{ | view | view.decrement(10) });
	}

	// access
	increment { | delta=1 |
		var step;
		if ((step = this.step) == 0) {
			step = 0.001;
		};
		^this.value_(this.value + (delta * step))
	}
	decrement { | delta=1 | ^this.increment(delta.neg) }

	// copy/paste
	copyEvent { ^this.value }
	pasteEvent { | obj | if (obj.isKindOf(Number)) { this.value = obj } }

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

	value_ { |v|
		v = v.asFloat.clip(0.0, 1.0).round(this.step);
		this.setPropertyChanged(\value, v) { value = v };
	}
	
	// rk control values (experimental)
	/*
	controlValue_ { arg ctl;
		this.value = ctl.input;
		this.action = { ctl.input = this.value };
		this.addResource(
			ActionListener(ctl, \synch, { { this.value = ctl.input }.defer })
		);
	}
	*/
	prSet_value { |v| this.value_(v) }
}

SCUMVSlider : SCUMSlider {
	/*
	aspectRatio { ^this.bounds.height.reciprocal }
	*/
}

SCUMHSlider : SCUMSlider {
	/*
	aspectRatio { ^this.bounds.width.reciprocal }
	*/
}

SCUMPad : SCUMControl {
	var <x, <y;
	
	*propertyKeys {
		^super.propertyKeys ++ [\x, \y, \xStep, \yStep, \thumbSize, \steady]
	}
	
	*initClass {
		this.propertyDefaults.putAll((
			x: 0.0,
			y: 0.0,
			xStep: 0.0,
			yStep: 0.0,
			thumbSize: 5,
			steady: false
		));
	}
	
	// initialization
	initDefaults {
		super.initDefaults;
		x = this.class.propertyDefault(\x);
		y = this.class.propertyDefault(\y);
		this
			.setKey(nil, SCUM.keyUp,   #{ | view | view.increment })
			.setKey(nil, SCUM.keyDown, #{ | view | view.decrement });
	}

	// accessing
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
	value { ^Point(x, y) }
	x_ { |v|
		v = v.asFloat.clip(0.0, 1.0).round(this.step);
		this.setPropertyChanged(\x, v) { x = v };
	}
	y_ { |v|
		v = v.asFloat.clip(0.0, 1.0).round(this.step);
		this.setPropertyChanged(\y, v) { y = v };
	}
	step { ^this.getProperty(\step) }
	step_ { |v| this.setProperty(\step, v) }
	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |v| this.setProperty(\thumbSize, v) }
	steady { ^this.getProperty(\steady) }
	steady_ { |v| this.setProperty(\steady, v) }
	
	prSet_value { | x y |
		this.doMuted {
			this.x_(x);
			this.y_(y);
		};
		this.doAction(\value)
	}
}

SCUMTable : SCUMControl {
	var <value;

	*initClass {
		this
			.addProperty(\value, Signal.new)
			.addProperty(\step, 0.0)
			// one of [\lines, \filled, \waveform]
			.addProperty(\style, \lines)
	}
	
	initDefaults {
		super.initDefaults;
		value = this.class.propertyDefault(\value);
		this.prUpdate;
	}

	value_ { |v|
		var step;
		value = v.asSignal.clip(0.0, 1.0);
		// FIXME: fix RawArray-series!!
		if ((step = this.step) != 0) {
			value = value.collectAs({ |x| x.round(step) }, value.class);
		};
		this.prUpdate;
	}
	step { ^this.getProperty(\step) }
	step_ { |v| this.setProperty(\step, v.asFloat) }
	style { ^this.getProperty(\style) }
	style_ { |v| this.setProperty(\style, v.asSymbol) }

	// PRIVATE
	prDestroyed {
		value = this.class.propertyDefault(\value);
		super.prDestroyed;
	}
	prUpdate {
		var stream;
		stream = CollStream.on(Int8Array.new(value.size * 4));
		value.do { |v| stream.putFloat(v) };
		this.prSetServerProperty(\value, stream.contents);
		this.doActionUnlessMuted(\value);
	}
	prSet_value { | x y |
		//[thisMethod.name, x, y].postln;
		value.put(x, y);
		this.doActionUnlessMuted(\value);
	}
	prSet_valueRange { | start end startValue inc |
		//[thisMethod.name, start, end, startValue, inc].postln;
		value.overWrite(Signal.series(end - start, startValue, inc), start);
		this.doActionUnlessMuted(\value)
	}
}

// EOF