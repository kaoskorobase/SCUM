SCUMMenuBuilder
{
	var menu, radioItems, radioAction, radioUpdateAction;

	*new { ^this.newCopyArgs(List.new) }

	action { | text, action |
		this.prCloseRadio(false);
		menu.add(SCUMMenuAction(text, action))
	}
	toggle { | text, action, updateAction |
		this.prCloseRadio(false);
		menu.add(SCUMMenuToggle(text, action, updateAction))
	}
	sep {
		this.prCloseRadio(false);
		this.prSep;
	}

	openMenu { | text |
		this.prCloseRadio(false);
		menu.add(SCUMMenuOpen(text))
	}
	closeMenu {
		this.prCloseRadio(false);
		menu.add(SCUMMenuClose.new)
	}

	openRadio { | action, updateAction |
		this.prCloseRadio(true);
		radioItems = List.new;
		radioAction = action;
		radioUpdateAction = updateAction;
		if (menu.size > 0) { this.prSep };
	}
	radio { | text |
		if (radioItems.isNil) {
			warning("No open radio group");
		};
		radioItems.add(text)
	}

	menu {
		this.prCloseRadio(false);
		^SCUMMenu(menu.asArray.deepCopy);
	}

	// PRIVATE
	prSep {
		menu.add(SCUMMenuSeparator.new)
	}
	prCloseRadio { | sep |
		if (radioItems.notNil) {
			menu.addAll(SCUMMenuRadioGroup(menu.size, radioItems, radioAction, radioUpdateAction));
			if (sep) { this.prSep };
			radioItems = nil;
		}
	}
}

SCUMMenu : SCUMObject
{
	var <items;

	*new { | items |
		^super.new.init(items)
	}

	init { | argItems |
		items = argItems;
		this.prInit(items.collectMsg(\primSpec));
		SCUMDesktop.prRetain(this);
	}

	popup { | pos, item=nil |
		items.do { | item, i |
			item.update(this, i);
		};
		item = this.prPopup(pos, item ?? -1);
		if (item >= 0) {
			items[item].invoke(this, item);
		};
		^item
	}

	// properties
	item { ^this.getProperty(\item) }
	item_ { |v| this.setProperty(\item, v) }
	value { ^this.getProperty(\value) }
	value_ { |v| this.setProperty(\value, v) }

	// PRIVATE
	prInit { | items |
		_SCUM_Menu_New
		^this.primitiveFailed
	}
	prDestroyed {
		SCUMDesktop.prRelease(this);
		super.prDestroyed;
	}
	prPopup { | pos, item |
		_SCUM_Menu_Popup
	}
}

// EOF