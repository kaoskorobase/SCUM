+ Stethoscope
{
	makeSCUMWindow { | view |
		var style = 0, w;
		var vb, hb;

		if (view.isNil) {
			view = window = SCUMWindow({ |v|
				v.title = "stethoscope";
				v.initialSize = this.makeBounds.size;
			});
		};

		view.addResource(this);

		vb = SCUMVBox(view);
		vb.expand = 1;
		vb.fill = 1;

		n = SCUMScope(vb);
		n.bgColor = Color.black;
		n.expand = 1;
		n.fill = 1;

		view.keyDownAction = { | view, evt | this.keyDown(evt.char) };

		zx = n.xZoom.log2;
		zy = n.yZoom.log2;
		
		audiospec = ControlSpec(0, server.options.numAudioBusChannels, step:1);
		controlspec = ControlSpec(0, server.options.numControlBusChannels, step:1);

		hb = SCUMHBox(vb);
		hb.xExpand = 1;
		hb.xFill = 1;
		hb.spacing = 2;

		sl = SCUMHSlider(hb);
		sl.action = { arg x;
			var i;
			i = this.spec.map(x.value);
			this.index = i;
		};
		sl.expand = 1;
		sl.fill = 1;

		c = SCUMNumberEntry(hb).value_(0);
		c.action = { this.index = c.value };

		d = SCUMNumberEntry(hb).value_(numChannels);
		d.action = { this.numChannels = d.value.asInteger };

		this.updateColors;

		view.show;
	}
	
	updateSCUMColors {
		// not yet.
// 		var no, ni, c;
// 		no = server.options.numOutputBusChannels;
// 		ni = server.options.numInputBusChannels;
// 		n.waveColors = if(\audio === rate) { 
// 			c = Array.fill(numChannels, { rgb(255, 218, 000) }); 
// 			//ni.do { arg i; c.tryPut(index+i+no, rgb(255, 000, 000)) };
// 			//c
// 		} { 
// 			Array.fill(numChannels, { rgb(125, 255, 205) }); 
// 		}
		n.fgColor = if (\audio === rate)
		{ Color.new255(255, 218, 000) }
		{ Color.new255(125, 255, 205) };
	}
}
