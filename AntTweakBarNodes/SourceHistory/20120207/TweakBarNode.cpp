#include "stdafx.h"

#include "TweakBarNode.h"

using namespace VVVV::Nodes;



// import host and hand it to base constructor
// the two booleans set whether to create a render state and/or sampler state pin
TweakBarNode::TweakBarNode(IPluginHost2^ host) : DXLayerOutPluginBase(host, false, false)
{
	//set IPluginHost2 to filed
	//you can use like FHost->GetID() with it
	FHost = host;

	//create TweakBarVariables instance
	FVariables = gcnew TweakBarVariables();

	//set order of FLayerOut
	//this->FLayerOut->Order = 0;
}

//デストラクタ：リソース、特にアンマネージ度リソースを積極的に解放するためのメソッド
//.NET FrameworkのIDisposable.Dospose()に相当する
//マネージドリソースを解放してファイナライザを呼び出すのが良い
TweakBarNode::~TweakBarNode()
{
	//FLogger->Log(LogType::Debug, "I am from Destructor");

	//ここでマネージドリソースをデストラクト

	//ファイナライザの実行
	this->!TweakBarNode();
}

//ファイナライザ：リソースの解放し忘れによる被害を最小限に抑えるためのメソッド
//.NET FrameworkのObject.Finalize()に相当する
//アンマネージドリソースを解放する
TweakBarNode::!TweakBarNode()
{
	//FLogger->Log(LogType::Debug, "I am from Finalizer");

	//ここでアンマネージドリソースをデストラクト
	
}



//called when data for any output pin is requested
void TweakBarNode::Evaluate(int SpreadMax)
{
	//FLogger->Log(LogType::Debug, "I am from Evaluate");

	//表示・非表示の切り替え（TweakBar自体の描画とは分けている）
	if(FEnabled->IsChanged)
	{
		int visible;

		((bool)FEnabled[0])? visible = 1 : visible = 0;

		TwSetParam(bar, NULL, "visible", TW_PARAM_INT32, 1, &visible);
	}

	if(FLayerOut->IsConnected && FEnabled[0])
	{
		//ウィンドウサイズの変化を調べる
		if(FWindowSize->IsChanged)
		{
			//call TweakBar event
			TwWindowSize((int)FWindowSize[0], (int)FWindowSize[1]);
		}

		//マウス座標の変化を調べる
		if(FMouseXY->IsChanged)
		{
			//TweakBarに合わせてマウス座標を設定
			int mouseX = (int)(((double)FMouseXY[0] + 1) * ((int)FWindowSize[0] * 0.5));
			int mouseY = (int)((1 - (double)FMouseXY[1]) * ((int)FWindowSize[1] * 0.5));

			//call TweakBar event
			TwMouseMotion(mouseX, mouseY);
		}

		//マウスボタンの変化を調べる
		if(FMouseButton->IsChanged)
		{
			int result;

			//変化後の値がtrue：押された
			if((bool)FMouseButton[0])
			{
				//call TweakBar event
				result = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
			}
			//false：離れた
			else
			{
				//call TweakBar event
				result = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
			}

			FLogger->Log(LogType::Debug, FHost->GetID() + ":" + result.ToString());
		}

		//キーボードの変化を調べる
		// 複数のキーが同時に押された場合（コントロールキーなども含む）は、
		// Spreadで渡されるのでそれに応じた処理が必要
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
				//キーイベントを作成
				if(code != 0) this->CreateKeyEvent(code, modifier);
			}
		}

		//タイトルの変化を調べる
		if(FTitle->IsChanged)
		{
			//System::String^からchar*へ変換
			char* pStr = (char *)Marshal::StringToHGlobalAnsi((String^)FTitle[0]).ToPointer();

			//call TwSetParam
			TwSetParam(bar, NULL, "label", TW_PARAM_CSTRING, 1, pStr);

			//メモリの解放
			Marshal::FreeHGlobal(IntPtr(pStr));
		}

		//色の変化を調べる
		if(FColor->IsChanged)
		{
			RGBAColor color = (RGBAColor)FColor[0];
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


		/*
		//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
		//ボタンを追加する
		if(FAddButton[0])
		{
			//call TwAddButton
			TwAddButton(bar, "Button", ButtonCallback, NULL, NULL);
		}
		*/

		/*
		//線を追加する：変数の追加に統合
		if(FAddSeparator[0])
		{
			//call TwAddSeparator
			TwAddSeparator(bar, NULL, NULL);
		}
		*/

		//変数の追加
		if(FAdd[0])
		{
			for (int i = 0; i < FAddName->SliceCount; i++)
			{
				//変数を追加する
				this->AddVariable(Enum::GetName(TypeEnum::typeid, FType[i]), (String^)FAddName[i], (String^)FAddParameters[i]);
			}
		}

		//変数の削除
		if(FRemove[0])
		{
			for (int i = 0; i < FRemoveName->SliceCount; i++)
			{
				//変数を削除する
				this->RemoveVariable((String^)FRemoveName[i]);
			}
		}

		//テキスト表示の変更
		if(FText->IsChanged)
		{
			//System::String^からchar*へ変換
			String^ str = Enum::GetName(TextEnum::typeid, FText[0]);
			char* pStr = (char *)Marshal::StringToHGlobalAnsi(str).ToPointer();

			//call TwSetParam
			TwSetParam(bar, NULL, "text", TW_PARAM_CSTRING, 1, pStr);

			//メモリの解放
			Marshal::FreeHGlobal(IntPtr(pStr));
		}
		
		/*
		//TweakBarの名前をvvvvで確認するためのやり方
		FStringOutput[0] = gcnew String(TwGetBarName(bar));
		
		//TweakBarの色をvvvvで確認するためのやり方
		int col[3], alpha;
		TwGetParam(bar, NULL, "color", TW_PARAM_INT32, 3, col);
		TwGetParam(bar, NULL, "alpha", TW_PARAM_INT32, 1, &alpha);
		FLogger->Log(LogType::Debug, "r:" + col[0].ToString());
		FLogger->Log(LogType::Debug, "a:" + alpha.ToString());
		*/

	}//end of if(_isConnected)
}

//this method gets called, when Reinitialize() was called in evaluate,
//or a graphics device asks for its data
DeviceData^ TweakBarNode::CreateDeviceData(Device^ device)
{
	//FLogger->Log(LogType::Debug, "I am from CreateDeviceData");

	//TweakBarの初期化
	this->InitTweakBar(device);

	//TweakBarVariablesをセット
	FVariablesOutput[0] = FVariables;

	//StateBlockを作成（Renderメソッド内で行うと描画が重くなるのでここで）
	//参考：http://msdn.microsoft.com/ja-jp/library/cc324052.aspx
	block = gcnew StateBlock(device, StateBlockType::All);

	//return a new device data
	return gcnew DeviceData();
}

//this method gets called, when Update() was called in evaluate,
//or a graphics device asks for its data, here you fill the resources with the actual data
void TweakBarNode::UpdateDeviceData(DeviceData^ deviceData)
{
	//FLogger->Log(LogType::Debug, "I am from UpdateDeviceData");
}

//this is called by vvvv to delete the resources of a specific device data
void TweakBarNode::DestroyDeviceData(DeviceData^ deviceData, bool OnlyUnManaged)
{
	//FLogger->Log(LogType::Debug, "I am from DestroyDeviceData");

	// Delete TweakBar
	if(!TwDeleteBar(bar))
	{
		FMessage[0] = gcnew String(TwGetLastError());
	}
	else
	{
		FMessage[0] = "TweakBar Deleted";

		//カウントダウン
		GBarCount--;
	}
	
	//bar count = 1 means only help bar left
	//if(TwGetBarCount() == 1)
	if(GBarCount == 0)
	{
		// Terminate AntTweakBar
		if(!TwTerminate())
		{
			FMessage[0] = gcnew String(TwGetLastError());
		}
		else
		{
			FMessage[0] = "TweakBar Terminated";
		}

		//Reset Counter
		GRenderCounter = 0;
	}

	//Clear Dictionaries
	FVariables->ClearAllDictionary();

	//Dispose StateBlock
	delete block;
}

//render into the vvvv renderer
void TweakBarNode::Render(Device^ device, DeviceData^ deviceData)
{
	//FLogger->Log(LogType::Debug, "I am from Render");

	//NOTE:
	//  AntTweakBar lib save current state(like ZEnable) before Drawing,
	//  and load saved state after Drawing in TwDraw() method.
	//  In vvvv (or SlimDX?) it seems NOT working.
	//  So you need save & load manually with SlimDX lib.
	//  It can do with Capture() & Apply() methods od StateBlock class.
	//  http://slimdx.org/docs/#T_SlimDX_Direct3D9_StateBlock
	

	//	before = device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable);
	//	FLogger->Log(LogType::Debug, "before: " + before);
	//
	//	//getting ZEnable status by string
	//	String^ before = Enum::GetName(SlimDX::Direct3D9::ZBufferType::typeid, device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable));

	
	//TwDraw() should call only once at one loop.
	//GRenderCounter needs when multiple tweakbar exists
	if(GRenderCounter == 0)
	{
		//デバイスの現在のステートを取得する
		block->Capture();

		// Draw tweak bars
		TwDraw();

		//ステートブロックを現在のデバイスステートに適用する
		block->Apply();
	}

	//Reset counter if equals tweakbar count
	if(++GRenderCounter == GBarCount) GRenderCounter = 0;


	//  device->SetRenderState(SlimDX::Direct3D9::RenderState::ZEnable, SlimDX::Direct3D9::ZBufferType::UseZBuffer);
	//
	//	after = device->GetRenderState(SlimDX::Direct3D9::RenderState::ZEnable);
	//	FLogger->Log(LogType::Debug, "after:" + after.ToString());
	//	if(before != after) FLogger->Log(LogType::Debug, "change ZEnable");
}

//TweakBarの初期化
void TweakBarNode::InitTweakBar(Device^ device)
{
	//---------------------------------------------------------AntTweakBarの初期化
    //TweakBarの数が0個の場合のみ行う
	if(TwGetBarCount() == 0)
	{
		// (note that the Direct3D device pointer must be passed to TwInit)
		if( !TwInit(TW_DIRECT3D9, (void *)device->ComPointer))
		{
			String^ str = gcnew String(TwGetLastError());
			
			FMessage[0] = "TweakBar Init failed: " + str;

			delete str;
		}
		else
		{
			FMessage[0] = "TweakBar Init successed";
		}

		//---------------------------------------------------------画面サイズをセット
		TwWindowSize((int)FWindowSize[0], (int)FWindowSize[1]);
		/*
		if(!TwWindowSize(FHoldWidth, FHoldHeight))
		{
			String^ str = gcnew String(TwGetLastError());
			FLogger->Log(LogType::Debug, str);
			delete str;
		}
		else
		{
			FLogger->Log(LogType::Debug, "TwWindowSize OK");
		}
		*/
	}
	

	//---------------------------------------------------------TweakBarの作成
	//複数のTweakBarを作成した時に名前がかぶらないようにする必要がある
	
	//System::String^でTweakBarの名前を設定し、char*へ変換
	char* name = (char *)Marshal::StringToHGlobalAnsi( "TweakBar" + FHost->GetID().ToString() ).ToPointer();
	
	//TweakBarの作成
	bar = TwNewBar(name);

	//メモリの解放
	Marshal::FreeHGlobal(IntPtr(name));

	//カウントアップ
	GBarCount++;

	//---------------------------------------------------------タイトルをセット
	//System::String^からchar*へ変換
	char* label = (char *)Marshal::StringToHGlobalAnsi((String^)FTitle[0]).ToPointer();

	//call TwSetParam
	TwSetParam(bar, NULL, "label", TW_PARAM_CSTRING, 1, label);

	//メモリの解放
	Marshal::FreeHGlobal(IntPtr(label));
	//TwDefine(" TweakBar label='Settings' ");


	//---------------------------------------------------------色をセット
	RGBAColor color = (RGBAColor)FColor[0];
	int intColor[3], intAlpha;

	//mapping 0 - 1 to 0 - 255
	intColor[0] = (int)(color.R * 255);
	intColor[1] = (int)(color.G * 255);
	intColor[2] = (int)(color.B * 255);
	intAlpha = (int)(color.A * 255);

	//call TwSetParam
	TwSetParam(bar, NULL, "color", TW_PARAM_INT32, 3, intColor);
	TwSetParam(bar, NULL, "alpha", TW_PARAM_INT32, 1, &intAlpha);



	//---------------------------------------------------------ヘルプ画面
	//TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar in vvvv.' "); // Message added to the help bar.
	//TwDefine("TW_HELP visible=false"); //help bar is hidden


	//---------------------------------------------------------変数の追加
	/*
	//bool test
	TwAddVarRW(bar, "toggle", TW_TYPE_BOOLCPP, &(this->unmanagedClass->toggleArray[0]), "label='Toggle'");
	//pin_ptr<bool> pBool = &FArrayToggle[0];
	//TwAddVarRW(bar, "toggle", TW_TYPE_BOOLCPP, pBool, " label='Toggle' ");
	//pBool = nullptr;

	//int test
	TwAddVarRW(bar, "int", TW_TYPE_INT32, &(this->unmanagedClass->integerArray[0]), "label='Int' min=0 max=1000");
	//pin_ptr<int> pInt = &FArrayInteger[0];
    //TwAddVarRW(bar, "Int", TW_TYPE_INT32, pInt, " label='Integer' min=1 max=1000 keyIncr=s keyDecr=S help='int variable test' ");
	//pInt = nullptr;
	
	//double test
	TwAddVarRW(bar, "double", TW_TYPE_DOUBLE, &(this->unmanagedClass->doubleArray[0]), "label='Double' step=0.1");
	//pin_ptr<double> pDouble = &FArrayDouble[0];
	//TwAddVarRW(bar, "double", TW_TYPE_DOUBLE, pDouble, " label='Double' step=0.1 ");
	//pDouble = nullptr;
	
	//うまく動かない？？
	//pin_ptr<char*> pString = &FArrayString[0];
	//TwAddVarRW(bar, "String", TW_TYPE_CDSTRING, pString, "label='String'");
	//pString = nullptr;

	//color test
	TwAddVarRW(bar, "color", TW_TYPE_COLOR4F, &(this->unmanagedClass->colorArray[0][0]), "label='Color'");
	//pin_ptr<float> pColor = &FArrayColor[0, 0];
    //TwAddVarRW(bar, "color", TW_TYPE_COLOR4F, pColor, " label='Color' ");
	//pColor = nullptr;

	//direction test
	TwAddVarRW(bar, "dir", TW_TYPE_DIR3D, &(this->unmanagedClass->directionArray[0][0]), "label='Direction'");
	//pin_ptr<double> pDir = &FArrayDirection[0, 0];
	//TwAddVarRW(bar, "dir", TW_TYPE_DIR3D, pDir, " label='Direction' ");
	//pDir = nullptr;

	//quaternion test
	TwAddVarRW(bar, "quat", TW_TYPE_QUAT4D, &(this->unmanagedClass->quaternionArray[0][0]), "label='Quaternion'");
	//pin_ptr<double> pQuat = &FArrayQuaternion[0, 0];
	//TwAddVarRW(bar, "quat", TW_TYPE_QUAT4D, pQuat, "label='Quaternion'");
	//pQuat = nullptr;
	*/
}

//TweakBarのキーボードイベントを作成
void TweakBarNode::CreateKeyEvent(int keyCodeWin32, int modifiers)
{
	int key = -1;	//ダミー値をセット

	//Win32 APIのMapVirtualKeyを使うと便利っぽい
	//http://msdn.microsoft.com/ja-jp/library/cc410909.aspx
	//user32.libとWindows.hが必要？
	//UINT ui = MapVirtualKey(keyCodeWin32, 2);

	//Win32APIの仮想キーコードに応じて処理
	//'('とか'%'とかはShiftキーが押されているかどうかで判断するしかない？
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
			//ASCIIと同じ
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
			//こうしないと大文字にならない。なぜ？
			if(modifiers == TW_KMOD_SHIFT)
			{
				//そのまま（ASCIIの65 - 90）
				key = keyCodeWin32;
			}
			else
			{
				//小文字に変換（ASCIIの97 - 122）
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
			//0 - 9に変換（ASCIIの48 - 57）
			key = keyCodeWin32 - 48;
			break;
		case 106:	//NUMPAD *,+,,,-,.,/
		case 107:
		case 108:
		case 109:
		case 110:
		case 111:
			//ASCIIの42 - 47に変換
			key = keyCodeWin32 - 64;
			break;
		//F1 - F15（Win32のF16 - F24は無視する）
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
			//上記以外は意味がないので終わる
			break;
	}
	
	if(key > 0)
	{
		//call TweakBar event
		TwKeyPressed(key, modifiers);
	}
}

//変数の追加
void TweakBarNode::AddVariable(String^ type, String^ name, String^ param)
{
	if(FVariables->FindVariable(name))
	{
		//すでに登録済みの名前の場合は終了
		FMessage[0] = name + " is already exists";

		return;
	}

	if(name == "" && type != "Separator")
	{
		//Separator以外で名前がない場合は終了
		FMessage[0] = "please input name";

		return;
	}



	//変数を追加する場所を見つける
	int index = FVariables->GetEmptyIndex(type);

	if(index == -1)
	{
		//登録数が制限値に達しているので追加しない
		FMessage[0] = type + " variables is full";

		return;
	}
	else
	{
		int result;
		char *label, *charName, *charParam;
		TwType twType;
		void *var;

		// System::String^からchar*へ変換
		// 1)label
		label = (char *)Marshal::StringToHGlobalAnsi(name).ToPointer();
			
		// 2)internal name(type + index)
		charName = (char *)Marshal::StringToHGlobalAnsi(type + index.ToString()).ToPointer();

		// 3)optional parameters
		charParam = (char *)Marshal::StringToHGlobalAnsi(param).ToPointer();

			
		//型がSeparatorの場合のみ別処理
		if(type == "Separator")
		{
			//名前のないSeparator向け
			if(name == "") charName = NULL;

			//TweakBarにSeparatorを追加する
			result = TwAddSeparator(bar, charName, charParam);
		}
		else
		{
			//typeに応じてTweakBarに追加する変数の型を設定する
			//switchの条件にString^は使えないのでifにする
			if(type == "Toggle") twType = TW_TYPE_BOOLCPP;
			else if(type == "Integer") twType = TW_TYPE_INT32;
			else if(type == "Double") twType = TW_TYPE_DOUBLE;
			else if(type == "String") twType = TW_TYPE_CSSTRING(STR_LENGTH);	//STR_LENGTH defined TweakBarVariables.h
			else if(type == "Color") twType = TW_TYPE_COLOR4F;
			else if(type == "Direction") twType = TW_TYPE_DIR3D;
			else if(type == "Quaternion") twType = TW_TYPE_QUAT4D;
				
			//変数のポインタを取得する（変数の初期化はGetPointer()で行う）
			var = FVariables->GetPointer(type, index);

			//TweakBarに変数を追加する
			result = TwAddVarRW(bar, charName, twType, var, charParam);
		}
			
		if(!result)
		{
			//失敗
			FMessage[0] = gcnew String(TwGetLastError());
				
			//オプションパラメータが無効の場合、変数は追加されてしまうのでそれを削除する
			//それ以外の場合は失敗するが気にしない
			TwRemoveVar(bar, charName);
		}
		else
		{
			//成功
			FMessage[0] = "Add " + name + " successed";

			//labelをセット
			if(type != "Separator") TwSetParam(bar, charName, "label", TW_PARAM_CSTRING, 1, label);

			//名前とindexを登録する（名前の無いSeparatorを除く）
			if(name != "") FVariables->AddVariable(type, name, index);
				

			//出力ピンのセット
			/*
			FIntegerName.SliceCount = FVariables->FDictInteger->Count;
			for(int i = 0; i < FIntegerName.SliceCount; i++)
			{
				//Dictionaryの一覧を出力する：追加した順にできるか？
			}
			*/
		}

		//メモリの解放
		Marshal::FreeHGlobal(IntPtr(label));
		Marshal::FreeHGlobal(IntPtr(charName));
		Marshal::FreeHGlobal(IntPtr(charParam));
	}
}


//変数の削除
void TweakBarNode::RemoveVariable(String^ name)
{
	if(FVariables->FindVariable(name) == false)
	{
		//見つからなかった場合は終了
		FMessage[0] = name + " not found";
	}
	else
	{
		int index;

		//変数名に対応するindexを取得
		index = FVariables->GetIndex(name);

		if(index == -1)
		{
			//失敗
			FMessage[0] = "get " + name + " failed";
		}
		else
		{
			char *charName;

			//内部用の名前を作成する（型＋ID）
			charName = (char *)Marshal::StringToHGlobalAnsi(FVariables->GetType(name) + index.ToString()).ToPointer();
				
			//TweakBarから変数を削除する
			if(!TwRemoveVar(bar, charName))
			{
				//失敗
				FMessage[0] = gcnew String(TwGetLastError());
			}
			else
			{
				//成功
				FMessage[0] = "Remove " + name + " successed";

				//名前とindexを削除する
				FVariables->RemoveVariable(name);
			}

			//メモリの解放
			Marshal::FreeHGlobal(IntPtr(charName));
		}
	}
}


/*
//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
//TweakBarのボタンを押したときに呼ばれるメソッド（テスト用）
void TW_CALL TweakBarDevelop::ButtonCallback(void *clientData)
{
	//do something here
}
*/