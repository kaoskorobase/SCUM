SCUMLabel : SCUMView
{
	var <text, <font;

	initDefaults {
		super.initDefaults;
		this.canFocus = false;
		this.xFill = 1;
		this.font = SCUMDesktop.font;
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
}

SCUMTextEntry : SCUMLabel
{
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
		this.prStartEditing;
		keyString = keyString.add(char);
		if (maxLength.notNil) {
			this.text = keyString.copyRange(0, maxLength);
		}{
			this.text = keyString;
		}
	}

	defaultKeyDownAction { | evt |
		evt.ifNl({
			if (this.isEditing) {
				this.prStopEditing(true);
			}{
				this.prStartEditing;
			}
			^this
		});
		evt.ifKeys([keyDelete, keyBackSpace, keyEscape], {
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
		keyString = nil;
		value = obj;
		if (setBoth) {
			this.text = this.valueToString(value);
		}
	}
	valueAction_ { | obj |
		var prev;
		prev = value;
		this.value = obj;
		if (value != prev) {
			this.doAction;
			this.changed(\value);
		}
	}

	onStartEditing { }
	onStopEditing { }

	// PRIVATE
	prStartEditing {
		if (this.isEditing.not) {
			keyString = String.new;
			fgColorSave = this.fgColor;
			this.fgColor = editColor;
			this.onStartEditing;
		}
	}
	prStopEditing { | commit |
		if (this.isEditing) {
			if (commit) {
				this.valueAction = this.stringToValue(keyString);
			}{
				this.text = this.valueToString(value);
			};
			keyString = nil;
			this.fgColor = fgColorSave;
			this.onStopEditing;
		}
	}
}

SCUMStringEntry : SCUMTextEntry
{
	var <>history, historyIndex;

	initDefaults {
		super.initDefaults;
		history = History(10);
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

SCUMNumberEntry : SCUMTextEntry
{
	var <precision, <>coarseStep=1, <>fineStep=0.001;
	var lastMousePos;

	*viewClass { ^SCUMStringEntry }

	initDefaults {
		super.initDefaults;
		this.bgColor = Color.grey(0.6);
		this.font = Font("Courier", 12);
		this.textAlignment = 4;
		this.precision = 5;
	}

	defaultValue { ^0 }
	valueToString { | obj | ^obj.asFloat.asStringPrec(precision) }
	stringToValue { | str | ^str.asFloat }

	increment { | n=1, fine=false | this.valueAction = this.value + (n * fine.if(fineStep, coarseStep)) }
	decrement { | n=1, fine=false | this.valueAction = this.value - (n * fine.if(fineStep, coarseStep)) }

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
	precision_ { | n |
		var size;
		precision = n;
		// TODO: need a minTextWidth property
		size = this.font.measure(String.fill(n+2, $2));
		size.width = 2.0 * this.padding.x + size.width;
		size.height = 2.0 * this.padding.y + size.height;
		this.minSize = size;
	}
	padding_ { | v |
		super.padding_(v);
		this.precision_(this.precision);
	}
}

SCUMList : SCUMScrollView {
	var <font, <items;

	// properties
	value { ^this.getProperty(\value) }
	value_ { |v| this.setProperty(\value, v) }
	font_ { |v| font = v; this.setProperty(\font, v) }
	items_ { |v|
		items = v;
		this.setProperty(\items, v.asArray);
	}
	item { ^items[this.value] }
	bgColorSel_ { |v| this.setProperty(\bgColorSel, v) }
	fgColorSel_ { |v| this.setProperty(\fgColorSel, v) }
}

// EOF