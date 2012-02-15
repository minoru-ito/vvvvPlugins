// 
// TweakBarNode.h
// 
// AntTweakBar in vvvv
// 
// updated: 2012/02/05
// written: Minoru Ito

#pragma once

using namespace System::Drawing;	//needs for RGBAColor. why?

using namespace SlimDX;
using namespace SlimDX::Direct3D9;
using namespace VVVV::PluginInterfaces::V2::EX9;
using namespace VVVV::Utils::SlimDX;

//enable TweakBar variables as pin
#include "TweakBarVariables.h"
using namespace VVVV::DataTypes;


namespace VVVV
{
	namespace Nodes
	{
		//enum of text color
		enum class TextEnum
		{
			Dark,
			Light
		};

		//enum of data type
		enum class TypeEnum
		{
			Toggle,
			Integer,
			Double,
			String,
			Color,
			Direction,
			Quaternion,
			//Separator
		};

		
		//PluginInfo
		[PluginInfo(Name = "TweakBar", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//ref class
		public ref class TweakBarNode : DXLayerOutPluginBase<DeviceData^>, IPluginEvaluate
		{

		public:

			//Constructor
			[ImportingConstructor()]
			TweakBarNode(IPluginHost2^ host);
			
			//Destructor
			~TweakBarNode();

			
			//----------------------必須のpublicメソッド
			//IPluginEvaluate
			virtual void Evaluate(int SpreadMax);
			//DXLayerOutPluginBase
			virtual DeviceData^ CreateDeviceData(Device^ device) override;
			virtual void UpdateDeviceData(DeviceData^ deviceData) override;
			virtual void DestroyDeviceData(DeviceData^ deviceData, bool OnlyUnManaged) override;
			virtual void Render(Device^ device, DeviceData^ deviceData) override;

		//internal:

		protected:

			//Finalizer
			!TweakBarNode();
			
		private:

			void InitTweakBar(Device^ device);
			void CreateKeyEvent(int keyCodeWin32, int modifiers);
			void AddVariable(String^ type, String^ name, String^ param);
			void RemoveVariable(String^ name);

			/*
			//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
			//----------------------TweakBarのcallBack function
			void TW_CALL ButtonCallback(void *clientData);
			*/

			
			//---------------------------------Input pins
			// 
			// IDiffSpreadはISpreadを継承している。前回のフレームから変化したかどうかを得られる


			//---------イベント処理関連

			//マウス左ボタン
			[Input("Mouse Button", IsSingle = true)]
			IDiffSpread<bool>^ FMouseButton;
			
			//マウス座標
			[Input("Mouse XY", DefaultValue = 0.0)]
			IDiffSpread<double>^ FMouseXY;
			
			//キーコード（Win32 APIの仮想キーコード）
			[Input("KeyCode")]
			IDiffSpread<int>^ FKeyCode;
			
			//Rendererサイズ
			[Input("Window Size", DefaultValue = 400)]
			IDiffSpread<int>^ FWindowSize;
			

			//---------グローバル設定関連

			//タイトル
			[Input("Title", IsSingle = true, DefaultString = "TweakBar")]
			IDiffSpread<String^>^ FTitle;
			
			//背景色
			//TweakBarの初期設定色は(R:17, G:109, B:143, A:64)
			[Input("Color", IsSingle = true, DefaultColor = gcnew array<double>{(double)17/255, (double)109/255, (double)143/255, (double)64/255})]
			IDiffSpread<RGBAColor>^ FColor;
			
			//テキスト表示
			[Input("Text", IsSingle = true, DefaultEnumEntry = "Light")]
			IDiffSpread<TextEnum>^ FText;


			//---------パラメータ設定関連

			//追加する変数のデータ型
			[Input("Add Type", DefaultEnumEntry = "Double")]
			ISpread<TypeEnum>^ FType;

			//追加する変数の名前
			[Input("Add Name")]
			IDiffSpread<String^>^ FAddName;

			//追加する変数のオプション
			[Input("Add Parameters")]
			ISpread<String^>^ FAddParameters;

			//変数の追加
			[Input("Add", IsSingle = true, IsBang = true)]
			ISpread<bool>^ FAdd;
			
			//削除する変数の名前
			[Input("Remove Name")]
			IDiffSpread<String^>^ FRemoveName;

			//変数の削除
			[Input("Remove", IsSingle = true, IsBang = true)]
			ISpread<bool>^ FRemove;
			
			//ボタン追加テスト
			//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
			//[Input("Add Button", IsSingle = true, IsBang = true)]
			//ISpread<bool>^ FAddButton;
			
			//線追加テスト：Add Variableと統合する？
			[Input("Add Separator", IsSingle = true, IsBang = true)]
			ISpread<bool>^ FAddSeparator;


			//---------other

			[Input("Enabled", DefaultValue = 1.0, IsSingle = true)]
			ISpread<bool>^ FEnabled;


			//---------------------------------Output pins

			[Output("TweakBar Variables", IsSingle = true)]
			ISpread<TweakBarVariables^>^ FVariablesOutput;
			
			//added variable names
			[Output("Toggle Name")]
			ISpread<String^>^ FToggleName;

			[Output("Integer Name")]
			ISpread<String^>^ FIntegerName;

			[Output("Double Name")]
			ISpread<String^>^ FDoubleName;

			[Output("String Name")]
			ISpread<String^>^ FStringName;

			[Output("Color Name")]
			ISpread<String^>^ FColorName;

			[Output("Direction Name")]
			ISpread<String^>^ FDirectionName;

			[Output("Quaternion Name")]
			ISpread<String^>^ FQuaternionName;

			
			//----------------------ILogger
			[Import()]
			ILogger^ FLogger;


			//----------------------Other
			
			//TweakBar
			TwBar* bar;

			//TweakBar variables
			TweakBarVariables^ FVariables;

			//PluginHost
			IPluginHost2^ FHost;

			//slice count
			//int FSpreadCount;

			//check connect
			bool isConnected;
		};
	}
}