ServerPanel {
	var server, frame, contentFrame, countViews, recorder, ctlr;
	var running, booting, stopped;

	*new { | parent, server |
		^super.new.init(parent, server);
	}

	init { | parent, argServer |
		var serverName;
		var buttonBar, countFrame;
		var active, booter;
		var properties;

		server = argServer;
		serverName = server.name.asString;

		frame = SCUMVBox(parent, { |v|
			v.expand = 1;
			v.fill = 1;
			v.padding = 1;
			v.spacing = 2;
			ActionListener(v, \destroyed, { this.prStopListening });
		});

		active = SCUMButton(frame, { |v|
			v.canFocus = false;
			v.padding = 4;
			v.expand = 1;
			v.fill = 1;
			v.textAlignment = 4;
			v.bgColor = Color.grey(0.4);
			v.font = Font("Helvetica-Bold", 14);
			v.states = [[serverName], [serverName]];
			v.action = LookupTable({ this.prShowContents }, { this.prHideContents });
		});

		contentFrame = SCUMVBox(frame, { |v|
			v.fill = 1;
		});

		buttonBar = SCUMHBox(contentFrame, { |v|
 			v.homogenous = true;
			v.expand = 1;
			v.fill = 1;
		});

		properties = Environment.make {
			~expand = 1;
			~fill = 1;
		};

// 		#[["Up", \raise], ["Down", \lower]].do { |item|
// 			var lbl, msg;
// 			#lbl, msg = item;
// 			SCUMButton(buttonBar, {
// 				~properties = properties;
// 				~states = [[lbl]];
// 				~action = { frame.perform(msg) };
// 			});
// 		};

		if (server.isLocal) {
			booter = SCUMButton(buttonBar, { |v|
				v.properties = properties;
				v.states = [["Boot"], ["Quit"]];
				v.value = server.serverRunning.binaryValue;
				v.action = LookupTable({ server.quit }, { booting.value; server.boot });
			});
			SCUMButton(buttonBar, { |v|
				v.properties = properties;
				v.states = [["Free All"]];
				v.action = { server.freeAll };
			});
		};

		SCUMButton(buttonBar, { |v|
			v.properties = properties;
			v.states = [["Make Default"]];
			v.action = {
				Server.default = server;
				thisProcess.interpreter.s = server;
			};
		});
		recorder = SCUMButton(buttonBar, { |v|
			v.properties = properties;
			v.states = [["Rec ?"], ["Rec !"], ["Rec ."]];
			v.action = LookupTable(
				{ server.stopRecording },
				{ server.prepareForRecord },
				{ server.record }
			);
		}).enabled_(false);
		
		if (server.isLocal) {
			running = {
				active.fgColor = Color.red;
				booter.setProperty(\value, 1);
				recorder.enabled = true;
			};
			stopped = {
				active.fgColor = Color.grey(0.5);
				booter.setProperty(\value, 0);
				countViews.do({ | view | view.text = "?" });
				recorder.setProperty(\value, 0);
				recorder.enabled = false;
			};
			booting = {
				active.fgColor = Color(0.2, 0.3, 0.2);
			};	
		}{
			running = {
				active.fgColor = Color.red;
				recorder.enabled = true;
			};
			stopped = {
				active.fgColor = Color.grey(0.5);
				countViews.do({ | view | view.text = "?" });
				recorder.setProperty(\value, 0);
				recorder.enabled = false;
			};
			booting = {
				active.fgColor = Color(0.2, 0.3, 0.2);
			};	
		};

		countFrame = SCUMHGrid(contentFrame, { |v|
			v.name = "countFrame";
			v.dimensions = Size(4, 3);
			v.xExpand = 1;
			v.xFill = 1;
			v.homogenous = true;
		});

		countViews = #[
			"Avg CPU", "Peak CPU",
			"UGens", "Synths",
			"Groups", "SynthDefs"
		].collect({ | label |
			SCUMLabel(countFrame, { |v|
				v.text = label + ":";
				v.textAlignment = 6;
				v.xExpand = 0;
			});
			SCUMLabel(countFrame, { |v|
				v.textAlignment = 4;
				v.xPadding = 5;
			});
		});

		this.prStartListening;
	}

	asView { ^frame }

	// PRIVATE
	prStartListening {
		if (server.serverRunning, running, stopped);
		ctlr = SimpleController(server)
		.put(\serverRunning, {
			if (server.serverRunning, running, stopped)
		})
		.put(\counts, {
			countViews.at(0).text = server.avgCPU.round(0.1);
			countViews.at(1).text = server.peakCPU.round(0.1);
			countViews.at(2).text = server.numUGens;
			countViews.at(3).text = server.numSynths;
			countViews.at(4).text = server.numGroups;
			countViews.at(5).text = server.numSynthDefs;
		})
		.put(\cmdPeriod, {
			recorder.setProperty(\value, 0);
		});
		server.startAliveThread;
	}
	prStopListening {
		if (server.isLocal.not) { server.stopAliveThread };
		if (ctlr.notNil) {
			ctlr.remove;
			ctlr = nil;
		}
	}
	prShowContents {
		this.prStartListening;
		contentFrame.show.updateLayout(false, true);
	}
	prHideContents {
		contentFrame.hide.updateLayout(false, true);
		this.prStopListening;
		stopped.value;
	}
}

ServerWindow {
	var window, frame;
	classvar default;

	*new { ^super.new.init }
	*default {
		if (default.isNil) {
			default = this.new;
		};
		^default
	}

	init {
		var box;

		window = SCUMWindow.new;
		window.title = "Server Panel";
		window.padding = 1;
		window.resizable = false;

		frame = SCUMVBox(window);
		frame.spacing = 1;
		
		box = SCUMHBox(frame);
		box.padding = 1;
		box.xExpand = 1;
		box.xFill = 1;
		
		SCUMButton(box, { |v|
			v.xExpand = 1;
			v.xFill = 1;
			v.states = [["Free All"]];
			v.action = {
				Server.freeAll;
			};
		});
		SCUMButton(box, { |v|
			v.xExpand = 1;
			v.xFill = 1;
			v.states = [["Kill All", Color.red]];
			v.action = {
				Server.killAll;
			};
		});
	}

	frame {
		if (frame.notValid) {
			this.init;
		};
		^frame
	}
	window {
		if (window.notValid) {
			this.init;
		};
		^window
	}
	show { this.window.show }
	hide { this.window.hide }

	asView { ^this.frame }
}
