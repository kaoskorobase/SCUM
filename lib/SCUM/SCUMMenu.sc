SCUMMenuBuilder
{
	var menu, radioItems, radioAction, radioUpdateAction;

	action { | text, action |
		this.prCloseRadio(false);
		this.prAdd(SCUMMenuAction, text, action)
	}
	toggle { | text, action, updateAction |
		this.prCloseRadio(false);
		this.prAdd(SCUMMenuToggle, text, action, updateAction)
	}
	sep {
		this.prCloseRadio(false);
		this.prSep;
	}

	openMenu { | text |
		this.prCloseRadio(false);
		this.prAdd(SCUMMenuOpen, text)
	}
	closeMenu {
		this.prCloseRadio(false);
		this.prAdd(SCUMMenuClose)
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
			Error("No open radio group").throw;
		};
		radioItems.add(text)
	}

	menu {
		this.prCloseRadio(false);
		^SCUMMenu(menu.collectMsg(\value).flatten);
	}
	clear {
		menu = nil;
	}

	// PRIVATE
	prAdd { | class ... args |
		menu = menu.add(Message(class, \new, args))
	}
	prSep {
		this.prAdd(SCUMMenuSeparator)
	}
	prCloseRadio { | sep |
		if (radioItems.notNil) {
			this.prAdd(
				SCUMMenuRadioGroup,
				menu.size,
				radioAction,
				radioUpdateAction,
				radioItems
			);
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
		items.do { | elt, i |
			elt.update(this, i);
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