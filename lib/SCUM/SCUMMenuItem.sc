SCUMMenuItem
{
	text { ^"" }
	update { | menu, index | ^index + 1 }
	invoke { | menu, index | }

	primTag { ^this.subclassResponsibility  }
	primSpec { ^[this.primTag, this.text.asString] }
}

SCUMMenuSeparator : SCUMMenuItem
{
	primTag { ^$- }
}

SCUMMenuLabel : SCUMMenuItem
{
	var <text;

	*new { | text | ^this.newCopyArgs(text) }
}

SCUMMenuOpen : SCUMMenuLabel
{
	primTag { ^$> }
}

SCUMMenuClose : SCUMMenuItem
{
	primTag { ^$< }
}

SCUMMenuAction : SCUMMenuLabel
{
	var <>action;

	*new { | text, action |
		^super.new(text).action_(action)
	}

	invoke { | menu, index |
		action.value(menu, this)
	}

	primTag { ^$! }
}

SCUMMenuToggle : SCUMMenuLabel
{
	var <>value=false, <>action, <>updateAction;

	*new { | text, action, updateAction |
		^super.new(text).action_(action).updateAction_(updateAction)
	}

	update { | menu, index |
		menu.item = index;
		value = menu.value;
		updateAction.value(menu, this);
		menu.value = value;
		^index + 1
	}
	invoke { | menu, index |
		value = menu.value.not;
		menu.value = value;
		action.value(menu, this)
	}

	primTag { ^$? }
}

SCUMMenuRadioProxy : SCUMMenuLabel
{
	var group;

	*new { | group, text |
		^super.new(text).initGroup(group)
	}

	initGroup { | argGroup |
		group = argGroup;
	}

	update { | menu, index |
		^group.update(menu, index)
	}
	invoke { | menu, index |
		group.invoke(menu, index);
	}

	primTag { ^$* }
}

SCUMMenuRadioGroup : SCUMMenuItem
{
	var firstIndex, size;
	var <>value, <>action, <>updateAction;

	*new { | firstIndex, items, action, updateAction |
		var obj;
		obj = this.newCopyArgs(firstIndex, items.size, 0, action, updateAction);
		^items.collect({ | text | SCUMMenuRadioProxy(obj, text) })
	}

	update { | menu, index |
		updateAction.value(menu, this);
		menu.item = index + value;
		menu.value = true;
		^index + size
	}
	invoke { | menu, proxyIndex |
		value = proxyIndex - firstIndex;
		menu.item = proxyIndex;
		menu.value = true;
		action.value(menu, this);
	}
}

// EOF