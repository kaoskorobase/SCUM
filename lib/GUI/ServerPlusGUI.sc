+ Server {
	makeSCUMWindow { | w=nil |
		w = w ?? { ServerWindow.default };
		if (window.isNil) {
			window = ServerPanel(w, this);
			window.asView.addResource(Resource({ window = nil }));
		};
		^w.show;
	}
}
