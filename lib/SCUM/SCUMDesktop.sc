SCUMDesktop : SCUMObject
{
	classvar instance, objects;
	classvar <font, <>keyDownAction, <>keyUpAction;
	classvar <>currentCopy;

	*initClass {
		instance = this.new.prInit;
		objects = [];

		this.fgColor = Color(0.2, 0.2, 0.2);
		this.bgColor = Color(0.6, 0.6, 0.6);
		this.focusColor = Color(0.1, 0.1, 0.7);
		this.font = Font("Helvetica", 12);

		UI.registerForShutdown({ instance.prDestroyed });
	}

	// windows
	*windows {
		^objects.selectMsg(\isKindOf, SCUMWindow)
	}
	*closeAllWindows {
		this.windows.doMsg(\close)
	}

	// events
	*keyDown { | evt |
		evt.ifMod(modCommand, {
			evt.ifChar($c, { currentCopy = this.copyEvent; ^this });
			evt.ifChar($v, { if (currentCopy.notNil) { evt.view.pasteEvent(currentCopy) } ^this });
		});
		keyDownAction.value(evt)
	}
	*keyUp { | evt |
		keyUpAction.value(evt)
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ #[
			\size, \fgColor, \bgColor, \focusColor, \font
		];
	}
	*size { ^instance.getProperty(\bounds, Size.new) }
	*fgColor { ^instance.getProperty(\fgColor, Color.new) }
	*fgColor_ { |v| instance.setProperty(\fgColor, v) }
	*bgColor { ^instance.getProperty(\bgColor, Color.new) }
	*bgColor_ { |v| instance.setProperty(\bgColor, v) }
	*focusColor { ^instance.getProperty(\focusColor, Color.new) }
	*focusColor_ { |v| instance.setProperty(\focusColor, v) }
	*font_ { |v| font = v; instance.setProperty(\font, v) }

	// PRIVATE
	prInit {
		_SCUM_Desktop_New
	}
	prDestroyed {
		var list;
		list = objects.copy;
		objects = [];
		list.doMsg(\prDestroyed);
	}
	*prRetain { | obj |
		objects = objects.add(obj);
	}
	*prRelease { | obj |
		objects.remove(obj);
	}
}

// EOF