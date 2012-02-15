#include "stdafx.h"

#include "SendEventsNode.h"

using namespace VVVV::Nodes;


/*
// import host and hand it to base constructor
// the two booleans set whether to create a render state and/or sampler state pin
SendEventsNode::SendEventsNode(IPluginHost2^ host)
{
	//set IPluginHost2 to filed
	//you can use like FHost->GetID() with it
	FHost = host;
}

//デストラクタ：リソース、特にアンマネージ度リソースを積極的に解放するためのメソッド
//.NET FrameworkのIDisposable.Dospose()に相当する
//マネージドリソースを解放してファイナライザを呼び出すのが良い
SendEventsNode::~SendEventsNode()
{
	//FLogger->Log(LogType::Debug, "I am from Destructor");

	//ここでマネージドリソースをデストラクト

	//ファイナライザの実行
	this->!SendEventsNode();
}

//ファイナライザ：リソースの解放し忘れによる被害を最小限に抑えるためのメソッド
//.NET FrameworkのObject.Finalize()に相当する
//アンマネージドリソースを解放する
SendEventsNode::!SendEventsNode()
{
	//FLogger->Log(LogType::Debug, "I am from Finalizer");

	//ここでアンマネージドリソースをデストラクト
	
}
*/

//called when data for any output pin is requested
void SendEventsNode::Evaluate(int SpreadMax)
{
	//FLogger->Log(LogType::Debug, "I am from Evaluate");

	if((bool)FEnabled[0])
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
			//変化後の値がtrue：押された
			if((bool)FMouseButton[0])
			{
				//call TweakBar event
				TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
			}
			//false：離れた
			else
			{
				//call TweakBar event
				TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
			}
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
	}
}

//TweakBarのキーボードイベントを作成
void SendEventsNode::CreateKeyEvent(int keyCodeWin32, int modifiers)
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