+ SynthDescLib {
	browse {
		var w;
		var synthDescLib;
		var synthDescLibListView;
		var synthDescListView;
		var ugensListView;
		var controlsListView;
		var inputsListView;
		var outputsListView;
		var synthDescList;
		var hvBold12;
		var updateLib, updateDesc;
		var btn, testFn;

		var frame;
		var hb, grid;
		var props;

		hvBold12 = Font("Helvetica-Bold", 12);		
		
		w = SCUMWindow.new;
		w.title = "SynthDef browser";
		
		testFn = {
			var synth, item;
			item = this[synthDescListView.item.asSymbol];
			if (item.notNil) {
				synth = Synth(item.name);
				synth.postln;
// 				synth.play;
				SystemClock.sched(3, { 
					if (item.hasGate) 
						{ synth.release }
						{ synth.free };
				});
			};
		};

		frame = SCUMVBox(w);

		hb = SCUMHBox(frame);
		hb.xExpand = 1;
		hb.xFill = 1;
		hb.homogenous = true;
		
		btn = SCUMButton(hb);
		btn.states = [["test", Color.black]];
		btn.action = testFn;
		
		btn = SCUMButton(hb);
		btn.states = [["window", Color.black]];
		btn.action = {
			var item;
			item = this[synthDescListView.item.asSymbol];
			if (item.notNil) {
				item.makeWindow;
			}
		};
		
		grid = SCUMHGrid(frame);
		grid.wrap = 3;
		grid.expand = 1;
		grid.fill = 1;

		#["SynthDescLibs", "SynthDefs", "UGens"].do { |label|
			SCUMLabel(grid, { |v|
				v.text = label;
			});
		};

		props = IdentityDictionary[\expand -> 1, \fill -> 1];

		props[\fgColorSel] = Color.blue;
		props[\bgColorSel] = SCUMDesktop.bgColor;

		synthDescLibListView = SCUMList(grid).properties_(props); //.focus;
		synthDescListView = SCUMList(grid).properties_(props).value_(0);
// 		synthDescListView.beginDragAction_({arg v;
// 			v.items[v.value].asSymbol;
// 		});
		ugensListView = SCUMList(grid).properties_(props).font_(Font("Courier", 10));
		#["SynthDefControls", "SynthDef Inputs", "SynthDef Outputs"].do { | label |
			SCUMLabel(grid, { |v|
				v.text = label;
			});
		};

		props[\fgColorSel] = Color.black;
		props[\bgColorSel] = SCUMDesktop.bgColor;
		props[\canFocus] = false;

		controlsListView = SCUMList(grid).properties_(props);
		inputsListView = SCUMList(grid).properties_(props);
		outputsListView = SCUMList(grid).properties_(props);
// 		controlsListView.resize = 4;
// 		inputsListView.resize = 4;
// 		outputsListView.resize = 4;
		
		// this is a trick to not show hilighting.
// 		controlsListView.hiliteColor = Color.clear;
// 		inputsListView.hiliteColor = Color.clear;
// 		outputsListView.hiliteColor = Color.clear;
// 		controlsListView.selectedStringColor = Color.black;
// 		inputsListView.selectedStringColor = Color.black;
// 		outputsListView.selectedStringColor = Color.black;
		
		controlsListView.font = Font("Courier", 10);
		inputsListView.font = Font("Courier", 10);
		outputsListView.font = Font("Courier", 10);
		
		synthDescLibListView.items = SynthDescLib.all.keys.asArray.sort;
		synthDescLibListView.value = 0;
		synthDescLibListView.action = {
			synthDescListView.value = 0;
			updateLib.value;
		};
		
		synthDescListView.items = [];
		synthDescListView.action = {
			updateDesc.value;
		};
// 		synthDescListView.enterKeyAction = testFn;
		
		updateLib = {
			var libName;			
			libName = synthDescLibListView.item;
			synthDescLib = SynthDescLib.all[libName];
			synthDescList = synthDescLib.synthDescs.values.sort {|a,b| a.name <= b.name };
			synthDescListView.items = synthDescList.collect {|desc| desc.name.asString };
		};

		updateDesc = {
			var synthDesc;

			synthDesc = synthDescList[synthDescListView.value];
			
			if (synthDesc.isNil) {
				ugensListView.items = [];
				inputsListView.items = [];
				outputsListView.items = [];
				controlsListView.items = [];
			}{
				ugensListView.items = synthDesc.def.children.collect { |x, i|
					i.asString.extend(7, $ ) ++ x.class.name.asString;
				};

				inputsListView.items = synthDesc.inputs.collect { |x|
					var string;
					string = x.rate.asString;
					string = string.extend(9, $ ) ++ " " ++ x.startingChannel;
					string = string.extend(19, $ ) ++ " " ++ x.numberOfChannels;
				};
				outputsListView.items = synthDesc.outputs.collect { |x|
					var string;
					string = x.rate.asString;
					string = string.extend(9, $ ) ++ " " ++ x.startingChannel;
					string = string.extend(19, $ ) ++ " " ++ x.numberOfChannels;
				};
				controlsListView.items = synthDesc.controls.collect { |x|
					var string;
					string = if (x.name.notNil) { x.name.asString; }{ "" };
					if (x.rate.notNil) 
					{ 
						string = string.extend(12, $ ) ++ " " ++ x.rate; 
					};
					if (x.defaultValue.notNil) 
					{ 
						string = string.extend(22, $ ) ++ " " 
							++ x.defaultValue.asStringPrec(6); 
					};
				};
			};
		};

		updateLib.value;
		updateDesc.value;
		
		w.show;
	}
}