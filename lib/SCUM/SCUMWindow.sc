SCUMWindow : SCUMBin
{
	var <title;
	var showFocusTask, showFocusCond;
	var inAction=false;

	*new { | function |
		^super.new(nil, function)
	}

	// initialization
	initDefaults {
		showFocusCond = Condition.new;
		super.initDefaults;
		this.title = "Unnamed Window";
		this
		.setKey(modControl, $w, #{ | view | view.close })
		.setKey(modControl, $q, #{ SCUMDesktop.closeAllWindows })
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
				"SCUMWindow: close called in close action, destroying window!".warn;
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
		}, clock: AppClock);
	}
	showFocusFor { | seconds |
		var cond;
		cond = Condition.new;
		this.showFocusUntil(cond);
		AppClock.play(Routine({
			seconds.wait;
			cond.unhang;
		}))
	}

	// events
	nextKeyHandler { ^SCUMDesktop }
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
	title_ { |v|
		title = v.asString;
		this.setProperty(\title, "SuperCollider: " ++ title)
	}
	initialSize { ^this.getProperty(\initialSize, Size.new) }
	initialSize_ { |v| this.setProperty(\initialSize, v.asSize) }
	screenBounds { ^this.getProperty(\screenBounds, Rect.new) }
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
	updateLayout { | xFit=false, yFit=false |
		_SCUM_Window_UpdateLayout
		^this.primitiveFailed
	}

	// PRIVATE
	prAddToParent {
		SCUMDesktop.prRetain(this);
	}
	prRemoveFromParent {
		SCUMDesktop.prRelease(this);
	}
	prHandleClose {
		this.close;
	}
}

// EOF