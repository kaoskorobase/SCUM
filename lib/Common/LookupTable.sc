// OBSOLETE, use SCUMButtonAction
LookupTable
{
	var <>indexMsg=\value, <>table;

	*new { | ... functions |
		^super.new.table_(functions)
	}

	at { | indexer |
		^table.at(indexer.perform(indexMsg))
	}

	value { | indexer |
		^this.at(indexer).value(indexer)
	}
	valueArray { | indexer ... args |
		^this.at(indexer).valueArray(indexer, args);
	}
}
