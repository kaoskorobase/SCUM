SCUMWindow : SCUMBin {
	var showFocusTask, showFocusCond;
	var inAction=false;

	*initClass {
		this.propertyDefaults.putAll((
			title: "a SCUMBag",
			initialSize: Size(10, 10),
			resizable: true,
			fullscreen: false,
			decorated: true,
			modal: false,
			showFocus: false
		));
	}
	*new { | function |
		^super.new(nil, function)
	}
	*make { | function |
		^this.new(function)
	}

	// initialization
	initDefaults {
		super.initDefaults;
		this.title = this.class.propertyDefault(\title);
		this
			.setKey(modDefault, $w, #{ | view | view.close })
			.setKey(modDefault, $q, #{ \disconnect.postln; SCUM.disconnect })
			.setKey(modCommand, $., #{ thisProcess.stop });
	}

	// SCView compat
	onClose { ^action }
	onClose_ { | function | action = function }
	front { this.show }

	// access
	window { ^this }
	close {
		if (action.isNil) {
			this.destroy;
		}{
			if (inAction) {
				this.message("SCUMWindow: close called in close action, destroying window!");
				this.destroy;
			}{
				inAction = true;
				{ this.doAction }.protect { inAction = false };
			};
		}
	}

	// focus
	showFocusUntil { | condition |
		if (showFocusTask.notNil) {
			showFocusTask.stop;
		};
		showFocusTask = Routine.run({
			this.showFocus = true;
			condition.hang;
			if (this.isValid) {
				this.showFocus = false;
			};
			showFocusTask = nil;
		});
	}
	showFocusFor { | seconds |
		var cond;
		cond = Condition.new;
		this.showFocusUntil(cond);
		SystemClock.play(Routine({
			seconds.wait;
			cond.unhang;
		}))
	}

	// events
	nextKeyHandler { ^SCUM.desktop }
	defaultKeyDownAction { | evt |
		if (this.showFocus.not && { evt.hasMod(modControl) } && { evt.char == $f }) {
			showFocusCond = Condition({ this.showFocus.not });
			this.showFocusUntil(showFocusCond);
			evt.accept;
		}
	}
	defaultKeyUpAction { | evt |
		if (this.showFocus && showFocusCond.notNil && { evt.char == $f }) {
			showFocusCond.unhang;
			showFocusCond = nil;
			evt.accept;
		}
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ #[\title, \initialSize, \resizable, \fullscreen, \decorated, \modal];
	}
	title { ^this.getProperty(\title) }
	title_ { |v| this.setProperty(\title, v.asString) }
	initialSize { ^this.getProperty(\initialSize) }
	initialSize_ { |v| this.setProperty(\initialSize, v.asSize) }
	screenBounds { ^this.prQueryProperty(\screenBounds, Rect) }
	resizable { ^this.getProperty(\resizable) }
	resizable_ { |v| this.setProperty(\resizable, v) }
	fullscreen { ^this.getProperty(\fullscreen) }
	fullscreen_ { |v| this.setProperty(\fullscreen, v) }
	decorated { ^this.getProperty(\decorated) }
	decorated_ { |v| this.setProperty(\decorated, v) }
	modal { ^this.getProperty(\modal) }
	modal_ { |v| this.setProperty(\modal, v) }
	showFocus { ^this.getProperty(\showFocus) }
	showFocus_ { |v| this.setProperty(\showFocus, v) }

	// layout
	updateLayout { | xFit(false) yFit(false) |
		SCUM.sendMsg("/w_update", id, xFit, yFit)
	}

	// PRIVATE
	prAddToParent {
		SCUM.desktop.prRetain(this);
	}
	prRemoveFromParent {
		SCUM.desktop.prRelease(this);
	}
	prHandle_close {
		this.close;
	}
}

// EOF