SCUM
{
	// constants
	classvar <modShift, <modControl, <modCommand, <modKeypad;

	classvar <keyTab, <keyDelete, <keyBackSpace, <keySpace;
	classvar <keyLF, <keyCR, <keyEscape;
	classvar <keyUp, <keyDown, <keyLeft, <keyRight, <keyPageUp, <keyPageDown;
	classvar <keyHome, <keyBegin, <keyEnd;
	classvar <keyF1, <keyF2, <keyF3, <keyF4, <keyF5, <keyF6;
	classvar <keyF7, <keyF8, <keyF9, <keyF10, <keyF11, <keyF12;

	classvar <borderNone, <borderFlat, <borderRaised, <borderSunken;

	classvar <argv;

	*initClass {
		modShift		= (1 << 0);
		modControl		= (1 << 1);
		modCommand		= (1 << 2);
		modKeypad		= (1 << 3);

		keyBackSpace	= 16r0008;
		keyTab			= 16r0009;
		keyLF			= 16r000A;
		keyCR			= 16r000D;
		keyEscape		= 16r001B;
		keySpace		= 16r0020;
// 		keyDelete		= 16r007F;
		keyDelete		= 16rF728;

		keyUp			= 16rF700;
		keyDown			= 16rF701;
		keyLeft			= 16rF702;
		keyRight		= 16rF703;
		keyPageUp		= 16rF72C;
		keyPageDown		= 16rF72D;
		keyHome			= 16rF729;
		keyBegin		= 16rF72A;
		keyEnd			= 16rF72B;

		keyF1			= 16rF704;
		keyF1			= 16rF705;
		keyF1			= 16rF706;
		keyF1			= 16rF707;
		keyF1			= 16rF708;
		keyF1			= 16rF709;
		keyF1			= 16rF70A;
		keyF1			= 16rF70B;
		keyF1			= 16rF70C;
		keyF1			= 16rF70D;
		keyF1			= 16rF70E;
		keyF1			= 16rF70F;
	}
}