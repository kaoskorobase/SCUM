SCUMLayout
{
	var <>alignment, <>expand, <>fill, <>minSize;

	*new { | alignment=5, xExpand=0, yExpand=0, xFill=0, yFill=0, minSize=0 |
		^this.newCopyArgs(
			alignment,
			Point(xExpand, yExpand),
			Point(xFill, yFill),
			minSize.asSize
		)
	}

	xExpand { ^expand.x }
	xExpand_ { |v| expand.x = v }
	yExpand { ^expand.y }
	yExpand_ { |v| expand.y = v }
	xFill { ^fill.x }
	xFill_ { |v| fill.x = v }
	yFill { ^fill.y }
	yFill_ { |v| fill.y = v }

	applyTo { | view |
		view.alignment = alignment;
		view.xExpand = expand.x;
		view.yExpand = expand.y;
		view.xFill = fill.x;
		view.yFill = fill.y;
		view.minSize = minSize;
	}
}

// EOF