SCUMDesktop : SCUMObject {
	var resources /* FIXME: already in SCUMObject! */, <>keyDownAction, <>keyUpAction;
	classvar <>currentCopy;

	*initClass {
		this.propertyDefaults.putAll((
			fgColor: Color(0.2, 0.2, 0.2, 1),
			bgColor: Color(0.6, 0.6, 0.6, 1),
			focusColor: Color(0.1, 0.1, 0.7, 1),
			font: Font("Helvetica", 12)
		));
	}
		
	// windows
	windows {
		^resources.selectMsg(\isKindOf, SCUMWindow)
	}
	closeAllWindows {
		this.windows.doMsg(\close)
	}

	// events
	keyDown { | evt |
		keyDownAction.value(this, evt)
	}
	keyUp { | evt |
		keyUpAction.value(this, evt)
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ #[
			\size, \fgColor, \bgColor, \focusColor, \font
		];
	}
	size { ^this.prQueryProperty(\bounds, Size) }
	fgColor { ^this.getProperty(\fgColor) }
	fgColor_ { |v| this.setProperty(\fgColor, v) }
	bgColor { ^this.getProperty(\bgColor) }
	bgColor_ { |v| this.setProperty(\bgColor, v) }
	focusColor { ^this.getProperty(\focusColor) }
	focusColor_ { |v| this.setProperty(\focusColor, v) }
	font { ^this.getProperty(\font) }
	font_ { |v| this.setProperty(\font, v) }

	// PRIVATE
	prInit {
		var bundle;
		this.prInitID(0);
		resources = [];
		SCUM.makeBundle(nil) {
			this.class.propertyDefaults.keys.do { | key |
				SCUM.sendMsg(*this.setPropertyMsg(key, this.class.propertyDefault(key)));
			}
		}
	}
	prDestroyed {
		var list;
		list = resources.copy;
		resources = [];
		list.doMsg(\prDestroyed);
		super.prDestroyed;
	}
	prRetain { | obj |
		resources = resources.add(obj);
	}
	prRelease { | obj |
		resources.remove(obj);
	}
}

// EOF