SCUMWindow : SCUMBin
{
	var <title;

	*new { | function |
		^super.new(nil, function)
	}

	// initialization
	initDefaults {
		super.initDefaults;
		this.title = "Unnamed Window";
		this.setKey(modControl, $w, #{ |v| v.close });
		this.setKey(modControl, $q, #{ SCUMDesktop.closeAllWindows });
		this.setKey(modCommand, $., #{ thisProcess.stop });
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
			this.doAction;
		}
	}

	// events
	nextKeyHandler { ^SCUMDesktop }
// 	defaultKeyDownAction { | evt |
// 		if (evt.hasMod(modControl)) {
// 			evt.ifChar($w, { this.close; ^this });
// 			evt.ifChar($q, { SCUMDesktop.closeAllWindows; ^this });
// 		};
// 		if (evt.hasMod(modCommand)) {
// 			evt.ifChar($., { thisProcess.stop; ^this });
// 		};
// 		super.defaultKeyDownAction(evt);
// 	}

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