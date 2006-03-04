AbstractListener
{
	var model;

	*new { | model |
		^super.newCopyArgs(model).initModel
	}
	initModel {
		model.addDependant(this);
	}

	update { | theChanger, what ... moreArgs |
		^this.subclassResponsibility
	}

	remove {
		model.removeDependant(this);
	}
	free {
		this.remove;
	}
}

ActionListener : AbstractListener
{
	var <aspect, <action;

	*new { | model, aspect, action |
		^super.newCopyArgs(model, aspect, action).initModel
	}

	update { | theChanger, what ... moreArgs |
		if (what === aspect) {
			action.valueArray(theChanger, what, moreArgs);
		}
	}
}

MultiListener : AbstractListener
{
	var <>actions;

	put { | aspect, action |
		if (actions.isNil) {
			actions = IdentityDictionary.new(4);
		};
		actions.put(aspect, action);
	}

	update { | theChanger, what ... moreArgs |
		var action;
		action = actions.at(what);
		if (action.notNil) {
			action.valueArray(theChanger, what, moreArgs);
		}
	}
}

// EOF