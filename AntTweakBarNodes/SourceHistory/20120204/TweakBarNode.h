// 
// TweakBarNode.h
// 
// vvvvでAntTweakBarを使う。
// 
// updated: 2012/02/04
// written: Minoru Ito

#pragma once

//----------------------必要な名前空間を宣言
using namespace System::Drawing;	//RGBAColorを使うとこれが必要になる。なぜ？
using namespace System::Collections::Generic;

using namespace SlimDX;
using namespace SlimDX::Direct3D9;
using namespace VVVV::PluginInterfaces::V2::EX9;
using namespace VVVV::Utils::SlimDX;

//TweakBarをOutputPinに使えるようにする
#include "TweakBarDataType.h"
using namespace VVVV::DataTypes;


//型ごとの最大数（暫定的に使用）
#define MAX_COUNT 10


namespace VVVV
{
	namespace Nodes
	{

		//http://akatukisiden.wordpress.com/2011/12/23/cpp-ccli-wrapping-object/
		//変数を保持するアンマネージドクラス
		//これをnewしたものはガベージコレクタに監視されないので手動で管理する。
		public class UnmanagedClass
		{
		public:
			bool toggleArray[MAX_COUNT];
			int integerArray[MAX_COUNT];
			double doubleArray[MAX_COUNT];
			//char* stringArray[MAX_COUNT];
			float colorArray[MAX_COUNT][4];			//4つのfloatを持つ2次元配列（RGBA）
			double directionArray[MAX_COUNT][3];	//3つのdoubleを持つ2次元配列（XYZ）
			double quaternionArray[MAX_COUNT][4];	//4つのdoubleを持つ2次元配列（XYZW）
		};

		//enum：テキストカラー
		enum class TextEnum
		{
			Dark,
			Light
		};

		//enum：データ型
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

		
		//PluginInfoの設定（名前、カテゴリ、ヘルプ、タグなど）
		[PluginInfo(Name = "TweakBar", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//クラス宣言
		public ref class TweakBarNode : DXLayerOutPluginBase<DeviceData^>, IPluginEvaluate
		{

		public:

			//コンストラクタ
			[ImportingConstructor()]
			TweakBarNode(IPluginHost2^ host);
			
			//デストラクタ
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

			//ファイナライザ
			!TweakBarNode();
			
		private:

			void InitTweakBar(Device^ device);
			void CreateKeyEvent(int keyCodeWin32, int modifiers);

			

			/*
			//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
			//----------------------TweakBarのcallBack function
			void TW_CALL ButtonCallback(void *clientData);
			*/

			
			//---------------------------------入力ピン
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
			[Input("Color", DefaultColor = gcnew array<double>{(double)17/255, (double)109/255, (double)143/255, (double)64/255})]
			IDiffSpread<RGBAColor>^ FColor;
			
			//テキスト表示
			[Input("Text", DefaultEnumEntry = "Light", IsSingle = true)]
			IDiffSpread<TextEnum>^ FText;


			//---------パラメータ設定関連

			//追加する変数のデータ型
			[Input("Add Type", DefaultEnumEntry = "Double")]
			IDiffSpread<TypeEnum>^ FType;

			//追加する変数の名前
			[Input("Add Name")]
			IDiffSpread<String^>^ FAddName;

			//追加する変数のオプション
			[Input("Add Parameters")]
			IDiffSpread<String^>^ FOptionParameters;

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


			//---------そのほか

			[Input("Enabled", DefaultValue = 1.0, IsSingle = true)]
			ISpread<bool>^ FEnabled;

			//[Input("Transform In")]
			//IDiffSpread<Matrix>^ FTransformIn;

			//[Input("Vertex Positions")]
			//IDiffSpread<Vector3>^ FVertexIn;

			//[Input("Texture Filename", StringType = StringType::Filename)]
			//IDiffSpread<String^>^ FTexFileName;


			//---------------------------------出力ピン

			[Output("TweakBar", IsSingle = true)]
			ISpread<TweakBarDataType^>^ FTweakBarOutput;
			
			//登録済みの変数名
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


			//----------------------その他のフィールド
			
			//TweakBar
			TwBar* bar;

			//TweakBarをPinで扱うためのクラス
			TweakBarDataType^ FTweakBarDataType;

			//変数をアンマネージドに管理するクラス
			UnmanagedClass* unmanagedClass;

			//PluginHost
			IPluginHost2^ FHost;

			//slice count
			//int FSpreadCount;

			//check connect
			bool isConnected;

			/*
			//変数を保持するarray
			array<bool>^ FArrayToggle;
			array<int>^ FArrayInteger;
			array<double>^ FArrayDouble;
			array<char*>^ FArrayString;
			array<float, 2>^ FArrayColor;
			array<double, 2>^ FArrayDirection;
			array<double, 2>^ FArrayQuaternion;

			//変数をカウントする
			int FCounterBool;
			int FCounterInt;
			int FCounterDouble;
			int FCounterString;
			int FCounterColor;
			int FCounterDirection;
			int FCounterQuaternion;
			*/

			//変数名と配列のindexを対応付けるためのDictionary
			Dictionary<String^, int>^ FDictToggle;
			Dictionary<String^, int>^ FDictInteger;
			Dictionary<String^, int>^ FDictDouble;
			Dictionary<String^, int>^ FDictString;
			Dictionary<String^, int>^ FDictColor;
			Dictionary<String^, int>^ FDictDirection;
			Dictionary<String^, int>^ FDictQuaternion;
		};
	}
}