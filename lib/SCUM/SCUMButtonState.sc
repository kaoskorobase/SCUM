SCUMButtonState {
	var <>text, <>fgColor, <>bgColor;
	*new { |text, fgColor, bgColor|
		^super.newCopyArgs(text, fgColor ?? { Color.clear }, bgColor ?? { Color.clear })
	}
	scumEncodeOSC { |stream|
		stream.put(text);
		fgColor.scumEncodeOSC(stream);
		bgColor.scumEncodeOSC(stream);
	}
}