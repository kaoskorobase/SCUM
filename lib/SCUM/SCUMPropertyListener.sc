SCUMPropertyListener : ActionListener {
	*new { | model, property, action |
		^super.new(model, \property) { | theChanger, what, key ... moreArgs |
			if (key === property) {
				action.value(theChanger, what, key, *moreArgs)
			}
		}
	}
}