Resource {
	var <>freeAction;

	*new { | freeAction | ^this.newCopyArgs(freeAction) }
	free { freeAction.value }
}
