SCUMView : SCUMObject
{
	var <name, <parent;
	var <>action, <>keyDownAction, <>keyUpAction, keyDict;

	*new { | parent, function |
		^super.new.init(parent, function);
	}

	init { | argParent, function |
		var envir;
		parent = argParent.asView;
		this.prInit(parent, this.class.viewClass);
		this.prAddToParent;
		this.initDefaults;
		this.use(function);
	}
	initDefaults {
		this.fgColor = SCUMDesktop.fgColor;
		this.bgColor = SCUMDesktop.bgColor;
	}

	*viewClass { ^this }
	asView { ^this }

	// naming
	name_ { | argName |
		var oldName;
		oldName = name;
		name = if (argName.notNil, { argName.asString });
		if (parent.notNil) {
			parent.prRemoveChild(nil, oldName);
			parent.prAddChild(this, name);
		}
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
	doAction {
		action.value(this);
	}

	// events
	mouseDown { | evt | evt.ignore }
	mouseMove { | evt | }
	mouseUp { | evt | }
	scrollWheel { | evt | }
	contextMenu { | evt | }

	nextKeyHandler {
		^parent
	}
	defaultKeyDownAction { | evt |
		evt.ignore
	}
	keyDown { | evt |
		this.defaultKeyDownAction(evt);
		if (evt.isIgnored) {
			keyDownAction.value(evt);
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
			keyUpAction.value(evt);
		};
		if (evt.isIgnored) {
			this.nextKeyHandler.keyUp(evt);
		};
	}
	defaultKeyUpAction { | evt | evt.ignore }

	setKey { | mods, key, action |
		var dict0, dict1;
		dict0 = keyDict ?? { keyDict = IdentityDictionary.new };
		mods = mods.asArray.inject(0, {
			| cur, nxt |
			if (nxt.isKindOf(Symbol)) {
				nxt = (
					C: SCUM.modControl,
					M: SCUM.modCommand,
					S: SCUM.modShift,
					K: SCUM.modKeypad
				).at(nxt) ? 0;
			};
			cur | nxt
		});
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
				action.value(this);
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
	bounds    { ^this.getProperty(\bounds, Rect.new) }
	fgColor   { ^this.getProperty(\fgColor, Color.new) }
	fgColor_  { |v| this.setProperty(\fgColor, v) }
	bgColor   { ^this.getProperty(\bgColor, Color.new) }
	bgColor_  { |v| this.setProperty(\bgColor, v) }
	visible   { ^this.getProperty(\visible) }
	visible_  { |v| this.setPropertyChanged(\visible, v, { this.changed(\focus) }) }
	enabled   { ^this.getProperty(\enabled) }
	enabled_  { |v| this.setPropertyChanged(\enabled, v, { this.changed(\focus) }) }
	canFocus  { ^this.getProperty(\canFocus) }
	canFocus_ { |v| this.setPropertyChanged(\canFocus, v, { this.changed(\focus) }) }
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
	minSize    { ^this.getProperty(\minSize, Size.new) }
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
	prInit { | parent, class |
		_SCUM_View_New
		^this.primitiveFailed;
	}
	prAddToParent {
		parent.prAddChild(this);
	}
	prRemoveFromParent {
		parent.prRemoveChild(this, name);
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
		_SCUM_View_Raise
		^this.primitiveFailed
	}
	prLower {
		_SCUM_View_Lower
		^this.primitiveFailed
	}
	prGetPointProperty { | kx, ky |
		^Point(this.getProperty(kx), this.getProperty(ky))
	}
	prSetPointProperty { | kx, ky, v |
		this.setProperty(kx, v.x);
		this.setProperty(ky, v.y);
	}

	// PRIVATE
	// events coming from the window system
	prHandleFocus {
		this.changed(\focus);
	}
	prHandleMouseDown { | state, x, y |
		var evt;
		evt = SCUMMouseEvent(this, state, x, y);
		this.mouseDown(evt);
		^evt.isAccepted
	}
	prHandleMouseMove { | state, x, y |
		this.mouseMove(SCUMMouseEvent(this, state, x, y))
	}
	prHandleMouseUp { | state, x, y |
		this.mouseUp(SCUMMouseEvent(this, state, x, y))
	}
	prHandleScrollWheel { | state, x, y, dx, dy |
		this.scrollWheel(SCUMScrollWheelEvent(this, state, x, y, dx, dy))
	}
	prHandleContextMenu { | state, x, y |
		this.contextMenu(SCUMContextMenuEvent(this, state, x, y))
	}
	prHandleKeyDown { | state, char, key |
		this.keyDown(SCUMKeyEvent(this, state, char, key))
	}
	prHandleKeyUp { | state, char, key |
		this.keyUp(SCUMKeyEvent(this, state, char, key))
	}
}

// EOF