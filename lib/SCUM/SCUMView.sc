SCUMView : SCUMObject {
	var <name, <parent;
	var <>action, <>keyDownAction, <>keyUpAction, keyDict;
	
	*initClass {
		this.propertyDefaults.putAll((
			// appearance
			fgColor: { SCUM.desktop.fgColor },
			bgColor: { SCUM.desktop.bgColor },
			visible: true,
			enabled: true,
			canFocus: true,
			hasFocus: false,
			// layout
			alignment: 5,
			xExpand: 0,
			yExpand: 0,
			xFill: 0,
			yFill: 0,
			minSize: Size.new
		));
		ActionListener(SCUM, \connected, {
			SCUM.registerObjectMethod("/event", {Ê| obj name ... args |
				obj.tryPerform(("prHandle_" ++ name).asSymbol, *args);
			});
		});
	}
	
	*new { | parent, function |
		^super.new.initObject(parent, function);
	}
	*make { | function |
		if (~buildParent.isNil) {
			Error("missing ~buildParent for view").throw;
		};
		^this.new(~buildParent, function)
	}
	initObject { | argParent initFunction serverArgs |
		parent = argParent.asView;
		super.initObject(initFunction, [parent] ++ serverArgs);
		this.prAddToParent;
	}

	*viewClass { ^this }
	*actionProperty { ^nil }
	asView { ^this }

	// naming
	name_ { | argName |
		if (parent.notNil && name.notNil) {
			parent.prRemoveNamed(name);
		};
		name = argName !? { argName.asString };
		if (parent.notNil && name.notNil) {
			parent.prAddNamed(name, this);
		};
	}
	printOn { | stream |
		super.printOn(stream);
		if (name.notNil) {
			stream << "(\"" << name << "\")"
		}
	}

	// accessing
	window { ^if (parent.notNil) { parent.window } }

	show { this.visible = true }
	hide { this.visible = false }
	raise {
		this.prRaise;
		if (parent.notNil) {
			parent.prRaiseChild(this);
		}
	}
	lower {
		this.prLower;
		if (parent.notNil) {
			parent.prLowerChild(this);
		}
	}

	// action
	doAction { |aspect| action.value(this, aspect) }

	// events
	mouseDown { | evt | evt.ignore }
	mouseMove { | evt | }
	mouseUp { | evt | }
	scrollWheel { | evt | }
	contextMenu { | evt | }

	nextKeyHandler {
		^parent
	}
	defaultKeyDownAction { | evt | evt.ignore }
	keyDown { | evt |
		this.defaultKeyDownAction(evt);
		if (evt.isIgnored) {
			keyDownAction.value(this, evt);
		};
		if (evt.isIgnored) {
			this.invokeKeyBinding(evt);
		};
		if (evt.isIgnored) {
			this.nextKeyHandler.keyDown(evt);
		};
	}

	keyUp { | evt |
		this.defaultKeyUpAction(evt);
		if (evt.isIgnored) {
			keyUpAction.value(this, evt);
		};
		if (evt.isIgnored) {
			this.nextKeyHandler.keyUp(evt);
		};
	}
	defaultKeyUpAction { | evt | evt.ignore }

	setKey { | mods, key, action |
		var dict0, dict1;
		dict0 = keyDict ?? { keyDict = IdentityDictionary.new };
		mods = SCUMInputEvent.specToModMask(mods);
		if (key.isKindOf(Char)) {
			key = key.ascii
		};
		dict1 = dict0[mods];
		if (dict1.isNil) {
			dict1 = IdentityDictionary.new;
			dict0[mods] = dict1;
		};
		if (action.isNil) {
			dict1.removeAt(key);
			if (dict1.isEmpty) {
				dict0.removeAt(mods);
				if (dict0.isEmpty) {
					keyDict = nil;
				};
			};
		}{
			dict1.put(key, action);
		};
	}
	invokeKeyBinding { | evt |
		var action;
		if (keyDict.notNil) {
			action = keyDict.atFail(evt.state, { ^this }).atFail(evt.key, { ^this });
			if (action.notNil) {
				evt.accept;
				action.value(this, evt);
			}
		}
	}

	// copy/paste
	copyToClipboard { ^nil }
	canPaste { | object | ^false }
	pasteFromClipboard { | object |	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ #[
			// basic attributes
			\name,
			// behavior
			\visible, \enabled, \canFocus,
			// appearance
			\fgColor, \bgColor,
			// layout
			\alignment, \xExpand, \yExpand, \xFill, \yFill, \minSize
		];
	}
	bounds    { ^this.getProperty(\bounds, Rect) }
	fgColor   { ^this.getProperty(\fgColor, Color) }
	fgColor_  { |v| this.setProperty(\fgColor, v) }
	bgColor   { ^this.getProperty(\bgColor, Color) }
	bgColor_  { |v| this.setProperty(\bgColor, v) }
	visible   { ^this.getProperty(\visible) }
	visible_  { |v| this.setProperty(\visible, v) }
	enabled   { ^this.getProperty(\enabled) }
	enabled_  { |v| this.setProperty(\enabled, v) }
	canFocus  { ^this.getProperty(\canFocus) }
	canFocus_ { |v| this.setProperty(\canFocus, v) }
	hasFocus  { ^this.getProperty(\hasFocus) }

	// layout properties
	alignment  { ^this.getProperty(\alignment) }
	alignment_ { |v| this.setProperty(\alignment, v) }
	xExpand    { ^this.getProperty(\xExpand) }
	xExpand_   { |v| this.setProperty(\xExpand, v) }
	yExpand    { ^this.getProperty(\yExpand) }
	yExpand_   { |v| this.setProperty(\yExpand, v) }
	expand     { ^this.prGetPointProperty(\xExpand, \yExpand) }
	expand_    { |v| ^this.prSetPointProperty(\xExpand, \yExpand, v.asPoint) }
	xFill      { ^this.getProperty(\xFill) }
	xFill_     { |v| this.setProperty(\xFill, v) }
	yFill      { ^this.getProperty(\yFill) }
	yFill_     { |v| this.setProperty(\yFill, v) }
	fill       { ^this.prGetPointProperty(\xFill, \yFill) }
	fill_      { |v| this.prSetPointProperty(\xFill, \yFill, v.asPoint) }
	minSize    { ^this.getProperty(\minSize) }
	minSize_   { |v| this.setProperty(\minSize, v.asSize) }

	// layout
 	layout {
		^SCUMLayout(
			this.alignment,
			this.xExpand, this.yExpand,
			this.xFill, this.yFill,
			this.minSize
		)
	}
	layout_ { | layout |
		layout.applyTo(this)
	}

	updateLayout { | xFit=false, yFit=false |
		this.window.updateLayout(xFit, yFit)
	}

	// PRIVATE
	prAddToParent {
		parent.prAddChild(this);
	}
	prRemoveFromParent {
		parent.prRemoveChild(this);
		if (name.notNil) {
			parent.prRemoveNamed(name);
		};
	}
	prDestroyed {
		this.prRemoveFromParent;
// 		if (contextMenu.notNil) {
// 			contextMenu.destroy;
// 		};
		keyDict = nil;
		super.prDestroyed;
	}
	prRaise {
		SCUM.sendMsg("/raise", id)
	}
	prLower {
		SCUM.sendMsg("/lower", id)
	}
	prGetPointProperty { | kx, ky |
		^Point(this.getProperty(kx), this.getProperty(ky))
	}
	prSetPointProperty { | kx, ky, v |
		this.perform(kx.asSetter, v.x);
		this.perform(ky.asSetter, v.y);
	}

	// PRIVATE
	// events coming from the window system
	prHandle_focus { | flag |
		this.putProperty(\hasFocus, flag != 0);
		if (this.hasFocus) {
			this.window.showFocusFor(1);
		};
		this.changed(\focus);
	}
	prHandle_mouseDown { | state x y |
		var evt;
		evt = SCUMMouseEvent(this, state, x, y);
		this.mouseDown(evt);
		^evt.isAccepted
	}
	prHandle_mouseMove { | state x y |
		this.mouseMove(
			SCUMMouseEvent(this, state, x, y)
		)
	}
	prHandle_mouseUp { | state x y |
		this.mouseUp(
			SCUMMouseEvent(this, state, x, y)
		)
	}
	prHandle_scrollWheel { | state x y dx dy |
		this.scrollWheel(
			SCUMScrollWheelEvent(this, state, x, y, dx, dy)
		)
	}
	prHandle_contextMenu { | state x y |
		this.contextMenu(
			SCUMContextMenuEvent(this, state, x, y)
		)
	}
	prHandle_keyDown { | state char key |
		this.keyDown(
			SCUMKeyEvent(this, state, char.clip(0, 255).asAscii, key)
		)
	}
	prHandle_keyUp { | state char key |
		this.keyUp(
			SCUMKeyEvent(this, state, char.clip(0, 255).asAscii, key)
		)
	}
}

// EOF