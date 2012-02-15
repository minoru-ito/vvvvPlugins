/*
	NOTE:
	AntTweakBar lib save current state(like ZEnable) before Drawing,
	then apply saved state after Drawing in TwDraw() method.
	In vvvv (or SlimDX?) seems not working.
	So you need save & apply manually.
	It can do with Capture() & Apply() methods of StateBlock class.
	http://slimdx.org/docs/#T_SlimDX_Direct3D9_StateBlock

	[what happen connect and disconnect to renderer]
	00:00:02     : I am from Evaluate
	00:00:02     : I am from CreateDeviceData
	00:00:02     : I am from Render
	00:00:02     : I am from Evaluate
	00:00:02     : I am from UpdateDeviceData
	00:00:02     : I am from Render
	00:00:02     : I am from Evaluate
	00:00:02     : I am from Render
						...
	00:00:03     : I am from Evaluate
	00:00:03     : I am from Render
	00:00:03     : I am from DestroyDeviceData
*/


#include "stdafx.h"

#include "TweakBarNode.h"

using namespace VVVV::Nodes;
using namespace SlimDX::Direct3D9;


// import host and hand it to base constructor
// the two booleans set whether to create a render state and/or sampler state pin
TweakBarNode::TweakBarNode(IPluginHost2^ host) : DXLayerOutPluginBase(host, false, false)
{
	// set IPluginHost2 to field
	// you can use like FHost->GetID() with it
	this->FHost = host;

	// create TweakBarVariables instance
	this->FVariables = gcnew TweakBarVariables();

	// set order of FLayerOut
	//this->FLayerOut->Order = 0;
}

// Destructor
// recommend release managed resource in this method
// Equivalent IDisposable.Dospose() method of .NET Framework
// call finalizer after released managed resource
TweakBarNode::~TweakBarNode()
{
	//FLogger->Log(LogType::Debug, "I am from Destructor");

	// Release managed resources here
	
	// 1)Clear NameList
	this->UpdateAllNameList(-1);
	
	// 2)Clear Dictionaries
	this->FVariables->ClearAllDictionary();

	// 3)Delete TweakBar
	TwDeleteBar(bar);

	// 4)Terminate AntTweakBar lib if need
	if(--GCreatedBarCount == 0)		//check with count down
	{
		// Terminate AntTweakBar
		TwTerminate();

		// Reset Counter
		GConnectedBarCount = 0;
		GRenderCounter = 0;

		// Dispose StateBlock
		if(GBlock != nullptr)
		{
			delete GBlock;
			GBlock = nullptr;	//set nullptr is neccesary
		}
	}

	// Call Finalizer
	this->!TweakBarNode();
}

// Finalizer
// Equivalent Object.Finalize() method of .NET Framework
// release unmanaged resource
TweakBarNode::!TweakBarNode()
{
	//FLogger->Log(LogType::Debug, "I am from Finalizer");

	//Release unmanaged resources here
}



// called when data for any output pin is requested
void TweakBarNode::Evaluate(int SpreadMax)
{
	//FLogger->Log(LogType::Debug, "I am from Evaluate" + FHost->GetID().ToString());

	//do if any TweakBar connect to Renderer
	if(GConnectedBarCount > 0)
	{
		//--------------------------------------Events
		
		//AntTweakBar events only need just one time
		//even if create multiple tweakbars

		//WINDOW SIZE EVENT
		if(FWindowSize->IsChanged)
		{
			if((int)FWindowSize[0] > 0 && (int)FWindowSize[1] > 0)
			{
				//call TweakBar event
				TwWindowSize((int)FWindowSize[0], (int)FWindowSize[1]);

				//set flag
				isSizeChanged = true;
			}
		}

		//MOUSE XY EVENT
		if(FMouseXY->IsChanged)
		{
			//set mouse xy for TweakBar from vvvv
			int mouseX = (int)(((double)FMouseXY[0] + 1) * ((int)FWindowSize[0] * 0.5));
			int mouseY = (int)((1 - (double)FMouseXY[1]) * ((int)FWindowSize[1] * 0.5));

			//call TweakBar event
			TwMouseMotion(mouseX, mouseY);
		}

		//MOUSE BUTTON EVENT
		if(FMouseButton->IsChanged)
		{
			//true:pressed
			if((bool)FMouseButton[0])
			{
				//call TweakBar event
				TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
			}
			//false:released
			else
			{
				//call TweakBar event
				TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
			}
		}

		//KEY EVENT
		if(FKeyCode->IsChanged)
		{
			int code, modifier;
			
			code = (int)FKeyCode[0];

			if(code != 0)
			{
				switch(code)
				{
					case 16:	//SHIFT
						modifier = TW_KMOD_SHIFT;
						code = (int)FKeyCode[1];
						break;
					case 17:	//CONTROL
						modifier = TW_KMOD_CTRL;
						code = (int)FKeyCode[1];
						break;
					case 18:	//ALT
						modifier = TW_KMOD_ALT;
						code = (int)FKeyCode[1];
						break;
					default:
						modifier = TW_KMOD_NONE;
						break;
				}
				//create keyevent
				if(code != 0) this->CreateKeyEvent(code, modifier);
			}
		}
	}


	//do if this TweakBar already created
	if(isCreated)
	{
		//--------------------------------------Bar Settings

		//set tweakbar title
		if(FTitle->IsChanged)
		{
			this->SetTitle((String^)FTitle[0]);
		}

		//set tweakbar color
		if(FColor->IsChanged)
		{
			this->SetColor((RGBAColor)FColor[0]);
		}

		//set tweakbar text type
		if(FText->IsChanged)
		{
			//convert System::String^ to char*
			String^ str = Enum::GetName(TextEnum::typeid, FText[0]);
			char* pStr = (char *)Marshal::StringToHGlobalAnsi(str).ToPointer();

			//call TwSetParam
			TwSetParam(bar, NULL, "text", TW_PARAM_CSTRING, 1, pStr);

			//release memory
			Marshal::FreeHGlobal(IntPtr(pStr));
		}


		//--------------------------------------Variable Settings

		//add variable(s)
		if(FAdd[0])
		{
			for (int i = 0; i < FAddName->SliceCount; i++)
			{
				//add variable with type, name, and parameter
				this->AddVariable(Enum::GetName(TypeEnum::typeid, FType[i]), (String^)FAddName[i], (String^)FAddParameters[i]);
			}

			//update output pins
			this->UpdateAllNameList(1);
		}

		//remove variable(s)
		if(FRemove[0])
		{
			for (int i = 0; i < FRemoveName->SliceCount; i++)
			{
				//remove variable by name
				this->RemoveVariable((String^)FRemoveName[i]);
			}

			//update output pins
			this->UpdateAllNameList(1);
		}

		//switch show/hide
		//NOTE: it's separated with TweakBar Draw method.
		if(FEnabled->IsChanged)
		{
			((bool)FEnabled[0])? this->SetVisible(1) : this->SetVisible(0);
		}

		/*
		//UNDER DEVELOPMENT
		//add button
		if(FAddButton[0])
		{
			//call TwAddButton
			TwAddButton(bar, "Button", ButtonCallback, NULL, NULL);
		}

		//test: how to get name of TweakBar in vvvv
		FStringOutput[0] = gcnew String(TwGetBarName(bar));
		
		//test: how to get color of TweakBar in vvvv
		int col[3], alpha;
		TwGetParam(bar, NULL, "color", TW_PARAM_INT32, 3, col);
		TwGetParam(bar, NULL, "alpha", TW_PARAM_INT32, 1, &alpha);
		FLogger->Log(LogType::Debug, "r:" + col[0].ToString());
		FLogger->Log(LogType::Debug, "a:" + alpha.ToString());
		*/

	}//end of if(isCreated)
}

//this method gets called, when Reinitialize() was called in evaluate,
//or a graphics device asks for its data
DeviceData^ TweakBarNode::CreateDeviceData(Device^ device)
{
	//FLogger->Log(LogType::Debug, "I am from CreateDeviceData" + FHost->GetID().ToString());

	//do nothing if already created tweakbar
	if(!this->isCreated)
	{
		//NOTE:
		//create TweakBar(s) enable at single Renderer which connect at first only now.
		//it comes from limitation of device handling of AntTweakBar lib.
		
		//clear NameList
		this->UpdateAllNameList(-1);

		//init tweakbar
		this->InitTweakBar(device);

		//set TweakBarVariables (cannot do this in constructor)
		FVariablesOutput[0] = this->FVariables;
	}

	//create StateBlock
	if(GBlock == nullptr)
	{
		// create stateblock at render method is heavy (bad performance)
		// about stateblock:http://msdn.microsoft.com/ja-jp/library/cc324052.aspx
		GBlock = gcnew StateBlock(device, StateBlockType::All);

		//GBlock->Capture();	//capture here will NOT work with open patch with connected to Renderer
	}

	//return a new device data
	return gcnew DeviceData();
}

//this method gets called, when Update() was called in evaluate,
//or a graphics device asks for its data, here you fill the resources with the actual data
void TweakBarNode::UpdateDeviceData(DeviceData^ deviceData)
{
	//FLogger->Log(LogType::Debug, "I am from UpdateDeviceData" + FHost->GetID().ToString());
	
	//check TweakBar count with static variable
	if(GConnectedBarCount == 0)
	{
		//Set WindowSize
		if((int)FWindowSize[0] > 0 && (int)FWindowSize[1] > 0)
		{
			TwWindowSize((int)FWindowSize[0], (int)FWindowSize[1]);
		}

	}

	//set Title and Color of TweakBar
	this->SetTitle((String^)FTitle[0]);
	this->SetColor((RGBAColor)FColor[0]);

	//visible true
	if((bool)FEnabled[0]) this->SetVisible(1);

	//set flag should do at UpdateDeviceData for detect 1st Render called.
	isConnected = true;

	//counter up
	GConnectedBarCount++;
}

//this is called by vvvv to delete the resources of a specific device data
void TweakBarNode::DestroyDeviceData(DeviceData^ deviceData, bool OnlyUnManaged)
{
	//FLogger->Log(LogType::Debug, "I am from DestroyDeviceData" + FHost->GetID().ToString());
	
	//hide TweakBar (not delete)
	this->SetVisible(0);

	//reset flag
	isConnected = false;

	//counter down
	if(--GConnectedBarCount == 0)
	{
		// Dispose StateBlock (delete and set nullptr neccesary for switch fullscreen mode)
		delete GBlock;
		GBlock = nullptr;
	}
}

//render into the vvvv renderer
void TweakBarNode::Render(Device^ device, DeviceData^ deviceData)
{
	//FLogger->Log(LogType::Debug, "I am from Render" + FHost->GetID().ToString());

	//	before = device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable);
	//	FLogger->Log(LogType::Debug, "before: " + before);
	//
	//	//getting ZEnable status by string
	//	String^ before = Enum::GetName(SlimDX::Direct3D9::ZBufferType::typeid, device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable));
	

	//capture stateblock BEFORE UpdateDeviceData call or WindowSize changed
	if(GConnectedBarCount == 0 || isSizeChanged)
	{
		GBlock->Capture();

		//reset flag
		isSizeChanged = false;
	}
	
	//TwDraw() should call only once at one loop.
	//GRenderCounter needs when multiple tweakbar exists
	if(GRenderCounter == 0)
	{
		// Draw tweak bars
		TwDraw();

		//apply captured state block to current device state
		GBlock->Apply();

		//device->SetRenderState(RenderState::ZEnable, ZBufferType::UseZBuffer);
	}

	//Reset counter if >= tweakbar count
	if(++GRenderCounter >= GConnectedBarCount) GRenderCounter = 0;

	
	//  device->SetRenderState(SlimDX::Direct3D9::RenderState::ZEnable, SlimDX::Direct3D9::ZBufferType::UseZBuffer);
	//
	//	after = device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable);
	//	FLogger->Log(LogType::Debug, "after:" + after.ToString());
	//	if(before != after) FLogger->Log(LogType::Debug, "change ZEnable");
}



//set TweakBar Title
void TweakBarNode::SetTitle(String^ title)
{
	//convert System::String^ to char*
	char* label = (char *)Marshal::StringToHGlobalAnsi(title).ToPointer();

	//call TwSetParam
	TwSetParam(bar, NULL, "label", TW_PARAM_CSTRING, 1, label);

	//release memory
	Marshal::FreeHGlobal(IntPtr(label));
}

//set TweakBar Color
void TweakBarNode::SetColor(RGBAColor color)
{
	int intColor[3], intAlpha;

	//mapping 0 - 1 to 0 - 255
	intColor[0] = (int)(color.R * 255);
	intColor[1] = (int)(color.G * 255);
	intColor[2] = (int)(color.B * 255);
	intAlpha = (int)(color.A * 255);

	//call TwSetParam
	TwSetParam(bar, NULL, "color", TW_PARAM_INT32, 3, intColor);
	TwSetParam(bar, NULL, "alpha", TW_PARAM_INT32, 1, &intAlpha);
}

//set TweakBar visible
void TweakBarNode::SetVisible(int visible)
{
	//int visible = 0;
	TwSetParam(bar, NULL, "visible", TW_PARAM_INT32, 1, &visible);
}

//initialize TweakBar
void TweakBarNode::InitTweakBar(Device^ device)
{
	//do it when TweakBar count was 0
	//if(TwGetBarCount() == 0)
	if(GCreatedBarCount == 0)
	{
		//---------------------------------------------------------init AntTweakBar itself
		if(!TwInit(TW_DIRECT3D9, (void *)device->ComPointer))
		{
			String^ str = gcnew String(TwGetLastError());
			
			FMessage[0] = "AntTweakBar Init failed: " + str;

			delete str;

			return;
		}
		else
		{
			FMessage[0] = "AntTweakBar Init successed";
		}


		//---------------------------------------------------------set dammy windowsize
		//if use FWindowSize pin value here, it will failed when open patch.
		//this method called before loading saved input pin value.
		//then FWindowSize gets 0.
		//so set dammy value here, then reset at UpdateDeviceData()
		TwWindowSize(400, 300);
	}
	

	//---------------------------------------------------------create TweakBar
	//cannot use same name. it should manage inside.
	
	//use Node ID as name and convert to char*
	char* name = (char *)Marshal::StringToHGlobalAnsi( "TweakBar" + FHost->GetID().ToString() ).ToPointer();
	
	//create TweakBar
	bar = TwNewBar(name);

	//release memory
	Marshal::FreeHGlobal(IntPtr(name));

	//set flag
	this->isCreated = true;

	//counter up
	GCreatedBarCount++;

	//hide until UpdateDeviceData()
	this->SetVisible(0);


	//---------------------------------------------------------help window
	//TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar in vvvv.' "); // Message added to the help bar.
	//TwDefine("TW_HELP visible=false"); //help bar is hidden
}

//create keyboard event for TweakBar
void TweakBarNode::CreateKeyEvent(int keyCodeWin32, int modifiers)
{
	int key = -1;	//set dammy value

	//is it useful?
	//Win32 API's MapVirtualKey method
	//http://msdn.microsoft.com/ja-jp/library/cc410909.aspx
	//user32.lib, Windows.h
	//UINT ui = MapVirtualKey(keyCodeWin32, 2);

	//set key code with Win32API virtual keycode
	//it seems judge with shift key for some ASCII code ('(', '%', '!', ...)
	switch(keyCodeWin32)
	{
		case 8:		//BackSpace
			key = TW_KEY_BACKSPACE;
			break;
		case 9:		//Tab
			key = TW_KEY_TAB;
			break;
		case 13:	//Return
			key = TW_KEY_RETURN;
			break;
		case 19:	//Pause
			key = TW_KEY_PAUSE;
			break;
		case 27:	//Escape
			key = TW_KEY_ESCAPE;
			break;
		case 32:	//Space
			key = TW_KEY_SPACE;
			break;
		case 33:	//PageUp
			key = TW_KEY_PAGE_UP;
			break;
		case 34:	//PageDown
			key = TW_KEY_PAGE_DOWN;
			break;
		case 35:	//End
			key = TW_KEY_END;
			break;
		case 36:	//Home
			key = TW_KEY_HOME;
			break;
		case 37:	//LEFT
			key = TW_KEY_LEFT;
			break;
		case 38:	//UP
			key = TW_KEY_UP;
			break;
		case 39:	//RIGHT
			key = TW_KEY_RIGHT;
			break;
		case 40:	//DOWN
			key = TW_KEY_DOWN;
			break;
		case 45:	//Insert
			key = TW_KEY_INSERT;
			break;
		case 46:	//Delete
			key = TW_KEY_DELETE;
			break;
		case 48:	//0 - 9
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			//same with ASCII
			key = keyCodeWin32;
			break;
		case 65:	//A - Z
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
			//detect caps
			if(modifiers == TW_KMOD_SHIFT)
			{
				//A - Z (65 - 90 of ASCII)
				key = keyCodeWin32;
			}
			else
			{
				//a - z (97 - 122 of ASCII)
				key = keyCodeWin32 + 32;
			}
			break;
		case 96:	//NUMPAD 0 - 9
		case 97:
		case 98:
		case 99:
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
			//convert to 0 - 9 (48 - 57 of ASCII)
			key = keyCodeWin32 - 48;
			break;
		case 106:	//NUMPAD *,+,,,-,.,/
		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
			//convert to 42 - 47 of ASCII
			key = keyCodeWin32 - 64;
			break;
		//F1 - F15 (ignore F16 - F24 of Win32)
		case 112:
			key = TW_KEY_F1;
			break;
		case 113:
			key = TW_KEY_F2;
			break;
		case 114:
			key = TW_KEY_F3;
			break;
		case 115:
			key = TW_KEY_F4;
			break;
		case 116:
			key = TW_KEY_F5;
			break;
		case 117:
			key = TW_KEY_F6;
			break;
		case 118:
			key = TW_KEY_F7;
			break;
		case 119:
			key = TW_KEY_F8;
			break;
		case 120:
			key = TW_KEY_F9;
			break;
		case 121:
			key = TW_KEY_F10;
			break;
		case 122:
			key = TW_KEY_F11;
			break;
		case 123:
			key = TW_KEY_F12;
			break;
		case 124:
			key = TW_KEY_F13;
			break;
		case 125:
			key = TW_KEY_F14;
			break;
		case 126:
			key = TW_KEY_F15;
			break;
		case 186:	//:
			key = 58;
			break;
		case 187:	//;
			key = 59;
			break;
		case 188:	//,
			key = 44;
			break;
		case 189:	//-
			key = 45;
			break;
		case 190:	//.
			key = 46;
			break;
		case 191:	///
			key = 47;
			break;
		case 192:	//@
			key = 64;
			break;
		case 219:	//[
			key = 91;
			break;
		case 220:	//yen
			key = 92;
			break;
		case 221:	//]
			key = 93;
			break;
		case 222:	//^
			key = 94;
			break;
		case 223:	//_
			key = 95;
			break;
		case 226:	//_
			key = 95;
			break;
		default:
			//do nothing default
			break;
	}
	
	if(key > 0)
	{
		//call TweakBar event
		TwKeyPressed(key, modifiers);
	}
}

//add variable
void TweakBarNode::AddVariable(String^ type, String^ name, String^ param)
{
	if(FVariables->FindVariable(name))
	{
		//return if already exists
		FMessage[0] = name + " is already exists";

		return;
	}

	if(name == "" && type != "Separator")
	{
		//return if blank name and type is not Separator
		FMessage[0] = "please input name";

		return;
	}



	//get empty index
	int index = FVariables->GetEmptyIndex(type);

	if(index == -1)
	{
		//return if added count is full
		FMessage[0] = type + " variables is full";

		return;
	}
	else
	{
		int result;
		char *label, *charName, *charParam;
		TwType twType;
		void *var;

		// convert System::String^ to char*
		// 1)label
		label = (char *)Marshal::StringToHGlobalAnsi(name).ToPointer();
			
		// 2)internal name(type + index)
		charName = (char *)Marshal::StringToHGlobalAnsi(type + index.ToString()).ToPointer();

		// 3)optional parameters
		charParam = (char *)Marshal::StringToHGlobalAnsi(param).ToPointer();

			
		// do only Separator
		if(type == "Separator")
		{
			//blank separator is NULL
			if(name == "") charName = NULL;

			//add Separator to TweakBar
			result = TwAddSeparator(bar, charName, charParam);
		}
		else
		{
			//set tweak type of variable by type string
			//(cannot use "switch" with String^)
			if(type == "Toggle") twType = TW_TYPE_BOOLCPP;
			else if(type == "Integer") twType = TW_TYPE_INT32;
			else if(type == "Double") twType = TW_TYPE_DOUBLE;
			else if(type == "String") twType = TW_TYPE_CSSTRING(STR_LENGTH);	//STR_LENGTH defined TweakBarVariables.h
			else if(type == "Color") twType = TW_TYPE_COLOR4F;
			else if(type == "Direction") twType = TW_TYPE_DIR3D;
			else if(type == "Quaternion") twType = TW_TYPE_QUAT4D;
				
			//get pointer of variable (initialize variable at GetPointer())
			var = FVariables->GetPointer(type, index);

			//add variable to TweakBar
			result = TwAddVarRW(bar, charName, twType, var, charParam);
		}
			
		if(!result)
		{
			//failed
			FMessage[0] = gcnew String(TwGetLastError());
				
			//call remove variable nessesary when option parameter error occured
			//other case will failed, never mind.
			TwRemoveVar(bar, charName);
		}
		else
		{
			//successed
			FMessage[0] = "Add " + name + " successed";

			//set label
			if(type != "Separator") TwSetParam(bar, charName, "label", TW_PARAM_CSTRING, 1, label);

			//keep name and index pair except NULL separator
			if(name != "") FVariables->AddVariable(type, name, index);
		}

		//release memory
		Marshal::FreeHGlobal(IntPtr(label));
		Marshal::FreeHGlobal(IntPtr(charName));
		Marshal::FreeHGlobal(IntPtr(charParam));
	}
}

//remove variable
void TweakBarNode::RemoveVariable(String^ name)
{
	if(FVariables->FindVariable(name) == false)
	{
		//do nothing if not found
		FMessage[0] = name + " not found";
	}
	else
	{
		int index;

		//get index of variable
		index = FVariables->GetIndex(name);

		if(index == -1)
		{
			//failed
			FMessage[0] = "get " + name + " failed";
		}
		else
		{
			char *charName;

			//create "internal" name with type + index
			charName = (char *)Marshal::StringToHGlobalAnsi(FVariables->GetType(name) + index.ToString()).ToPointer();
				
			//remove variable from TweakBar
			if(!TwRemoveVar(bar, charName))
			{
				//failed
				FMessage[0] = gcnew String(TwGetLastError());
			}
			else
			{
				//successed
				FMessage[0] = "Remove " + name + " successed";

				//remove name and index pair
				FVariables->RemoveVariable(name);
			}

			//release memory
			Marshal::FreeHGlobal(IntPtr(charName));
		}
	}
}

//update all added variable names
void TweakBarNode::UpdateAllNameList(int code)
{
	if(code == -1)
	{
		FToggleName->SliceCount = 0;
		FIntegerName->SliceCount = 0;
		FDoubleName->SliceCount = 0;
		FStringName->SliceCount = 0;
		FColorName->SliceCount = 0;
		FDirectionName->SliceCount = 0;
		FQuaternionName->SliceCount = 0;
	}
	else
	{
		this->UpdateNameList("Toggle", FToggleName);
		this->UpdateNameList("Integer", FIntegerName);
		this->UpdateNameList("Double", FDoubleName);
		this->UpdateNameList("String", FStringName);
		this->UpdateNameList("Color", FColorName);
		this->UpdateNameList("Direction", FDirectionName);
		this->UpdateNameList("Quaternion", FQuaternionName);
	}
}

//update added variable name
void TweakBarNode::UpdateNameList(String^ type, ISpread<String^>^ output)
{
	Dictionary<String^, int>^ dict = FVariables->GetDictionary(type);
	int index = 0;

	output->SliceCount = dict->Count;
	
	for each(String^ name in dict->Keys)
	{
		output[index++] = name;
	}
}

/*
//UNDER DEVELOPMENT
//callback method for press button of TweakBar
void TW_CALL TweakBarDevelop::ButtonCallback(void *clientData)
{
	//do something here
}
*/