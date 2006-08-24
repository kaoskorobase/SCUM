SCUMLabel : SCUMView {
	var <text, <font;

	initClass {
		this.propertyDefaults.putAll((
			font: { SCUM.desktop.font },
			textAlignment: 5,
			xPadding: 5,
			yPadding: 5,
			frozen: false // better name?
		));
	}
	
	initDefaults {
		super.initDefaults;
		this.canFocus = false;
		this.xFill = 1;
	}

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\font, \padding, \text, \textAlignment, \border]
	}
	font_ { |v| font = v; this.setProperty(\font, v) }
	text_ { |v| text = v.asString; this.setProperty(\text, text) }
	textAlignment { ^this.getProperty(\textAlignment) }
	textAlignment_ { |v| this.setProperty(\textAlignment, v) }
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v) }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v) }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint) }
	frozen { ^this.getProperty(\frozen) }
	frozen_ { |v| this.setProperty(\frozen, v != 0); }
	minSize_ { |v|
		if (v.isKindOf(String)) {
			this.setProperty(\minSize, v);
		}{
			super.minSize_(v);
		}
	}
}

SCUMTextEntry : SCUMLabel {
	var <value, <>setBoth=true, <editColor;
	var fgColorSave, keyString;

	initDefaults {
		super.initDefaults;
		editColor = Color.red;
		value = this.defaultValue;
		this.canFocus = true;
		ActionListener(this, \focus, #{ |view|
			if (view.hasFocus.not) {
				view.prStopEditing(false);
			}
		});
	}

	isEditing { ^keyString.notNil }

	defaultValue { ^" " }
	valueToString { | obj | ^obj.asString }
	stringToValue { | str | ^str }

	addInput { | char, maxLength |
		this.prStartEditing(true);
		keyString = keyString.add(char);
		if (maxLength.notNil) {
			this.text = keyString.copyRange(0, maxLength);
		}{
			this.text = keyString;
		}
	}
	deleteLastInput {
		this.prStartEditing(false);
		keyString = keyString.copyRange(0, keyString.size - 2);
		this.text = keyString;
	}

	defaultKeyDownAction { | evt |
		evt.ifNl({
			if (this.isEditing) {
				this.prStopEditing(true);
			}{
				this.prStartEditing(false);
			}
			^this
		});
		evt.ifKey(keyBackSpace) {
			this.deleteLastInput;
			^this
		};
		evt.ifKeys([keyDelete, keyEscape], {
			this.prStopEditing(false);
			^this
		});
		super.defaultKeyDownAction(evt);
	}
	
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\editColor]
	}
	editColor_ { | color |
		editColor = color;
		if (this.isEditing) {
			this.fgColor = editColor;
		}
	}
	value_ { | obj |
		var prev = value;
		keyString = nil;
		value = obj;
		if (setBoth) {
			this.text = this.valueToString(value);
		};
		if (value != prev) {
			this.doActionUnlessMuted(\value);
		};
	}

	onStartEditing { }
	onStopEditing { }

	// PRIVATE
	prStartEditing { | erase |
		if (this.isEditing.not) {
			keyString = if (erase) { String.new } { this.text };
			fgColorSave = this.fgColor;
			this.fgColor = editColor;
			this.onStartEditing;
		}
	}
	prStopEditing { | commit |
		if (this.isEditing) {
			if (commit) {
				this.value = this.stringToValue(keyString);
			}{
				this.text = this.valueToString(value);
			};
			keyString = nil;
			this.fgColor = fgColorSave;
			this.onStopEditing;
		}
	}
}

SCUMStringEntry : SCUMTextEntry {
	var <>history, historyIndex;

	initDefaults {
		super.initDefaults;
		history = SCUMHistory(10);
		ActionListener(this, \value, #{ | view |
			view.history.push(view.value);
		});
	}

	defaultKeyDownAction { | evt |
		if (evt.char.isPrint) {
			this.addInput(evt.char);
			^this
		};
		if (this.isEditing) {
			evt.ifKey(keyUp, { ^this.prSetFromHistory(1) });
			evt.ifKey(keyDown, { ^this.prSetFromHistory(-1) });
		};
		super.defaultKeyDownAction(evt);
	}

	onStartEditing { historyIndex = 0 }

	prSetFromHistory { | n |
		var last;
		historyIndex = historyIndex + n;
		last = history.at(historyIndex);
		if (last.notNil) {
			keyString = last;
			this.text = last;
		}
	}
}

SCUMNumberEntry : SCUMTextEntry {
	var <>precision, <>coarseStep=1, <>fineStep=0.001, <>min, <>max;
	var lastMousePos;

	*serverClass { ^SCUMStringEntry }

	initDefaults {
		super.initDefaults;
		min = -inf;
		max = inf;
		this.bgColor = Color.grey(0.6);
		this.font = Font("Courier", 12);
		this.textAlignment = 4;
		this.precision = 5;
		this
			.setKey(nil, SCUM.keyUp, { |v| v.increment })
			.setKey(nil, SCUM.keyDown, { |v| v.decrement })
			.setKey(\S, SCUM.keyUp, { |v| v.increment(fine: true) })
			.setKey(\S, SCUM.keyDown, { |v| v.decrement(fine: true) })
	}

	defaultValue { ^0 }
	valueToString { | obj | ^obj.asFloat.asStringPrec(precision) }
	stringToValue { | str | ^str.asFloat }
	value_ { | aNumber | super.value_(aNumber.clip(min, max)) }

	increment { | n=1, fine=false | this.value = this.value + (n * fine.if(fineStep, coarseStep)) }
	decrement { | n=1, fine=false | this.value = this.value - (n * fine.if(fineStep, coarseStep)) }

	mouseDown { | evt |
		lastMousePos = evt.pos;
		evt.accept;
	}
	mouseMove { | evt |
		this.increment(lastMousePos.y - evt.pos.y, evt.hasMod(modShift));
		lastMousePos = evt.pos;
	}
	scrollWheel { | evt |
		this.decrement(evt.delta.y, evt.hasMod(modShift));
	}

	defaultKeyDownAction { | evt |
		if (evt.char.isDecDigit || "+-.eE".includes(evt.char)) {
			this.addInput(evt.char, precision);
			^this
		};
		super.defaultKeyDownAction(evt);
	}

	*propertyKeys {
		^super.propertyKeys ++ #[\precision];
	}
	/*
	precision_ { | n |
		var size;
		precision = n;
		// TODO: need a minTextWidth property
		size = this.font.measure(String.fill(n+2, $2));
		size.width = 2.0 * this.padding.x + size.width;
		size.height = 2.0 * this.padding.y + size.height;
		this.minSize = size;
	}
	*/
	padding_ { | v |
		super.padding_(v);
		this.precision_(this.precision);
	}
}

SCUMList : SCUMScrollView {
	*initClass {
		this.propertyDefaults.putAll((
			value: 0,
			font: { SCUM.desktop.font },
			textAlignment: 5,
			fgColorSel: nil, // FIXME
			bgColorSel: nil, // FIXME
			xPadding: 4,
			yPadding: 1
		));
	}
	
	// properties
	value { ^this.getProperty(\value) }
	value_ { |v| this.setActiveProperty(\value, v.clip(0, this.items.size)) }
	font { ^this.getProperty(\font) }
	font_ { |v| this.setProperty(\font, v) }
	items { ^this.getProperty(\items) }
	items_ { |v| this.setProperty(\items, v.asArray) }
	item { ^this.items.at(this.value) }
	bgColorSel_ { |v| this.setProperty(\bgColorSel, v) }
	fgColorSel_ { |v| this.setProperty(\fgColorSel, v) }
}

// EOF
