SCUMButtonAction
{
	var table;
	*new { | ... functions |
		^this.newCopyArgs(functions)
	}
	value { | view |
		^table[view.value].value(view)
	}
}
