// 
// GetVariableNode.h
// 
// get variables of AntTweakBar
// 
// updated: 2012/02/05
// written: Minoru Ito

#pragma once

//----------------------必要な名前空間を宣言
//良く使うものはStdafx.hに移動



//TweakBarの変数をPinで扱えるようにする
#include "TweakBarVariables.h"
using namespace VVVV::DataTypes;



namespace VVVV
{
	namespace Nodes
	{
		//PluginInfoの設定（名前、カテゴリ、ヘルプ、タグなど）
		[PluginInfo(Name = "GetVariable", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//クラス宣言
		public ref class GetVariableNode : IPluginEvaluate
		{

		public:

			//コンストラクタ
			[ImportingConstructor()]
			GetVariableNode(IPluginHost2^ host);

			
			//----------------------必須のpublicメソッド
			virtual void Evaluate(int SpreadMax);

		protected:

			//

		private:

			//---------------------------------入力ピン
			
			//TweakBarの変数を扱うためのクラス
			TweakBarVariables^ FVariables;
			
			[Input("TweakBar Variables", IsSingle = true)]
			IDiffSpread<TweakBarVariables^>^ FVariablesInput;

			[Input("Toggle Name")]
			IDiffSpread<String^>^ FToggleName;

			[Input("Integer Name")]
			IDiffSpread<String^>^ FIntegerName;

			[Input("Double Name")]
			IDiffSpread<String^>^ FDoubleName;

			[Input("String Name")]
			IDiffSpread<String^>^ FStringName;

			[Input("Color Name")]
			IDiffSpread<String^>^ FColorName;

			[Input("Direction Name")]
			IDiffSpread<String^>^ FDirectionName;

			[Input("Quaternion Name")]
			IDiffSpread<String^>^ FQuaternionName;


			//---------------------------------出力ピン

			[Output("Test String")]
			ISpread<String^>^ FTestString;

			[Output("Toggle")]
			ISpread<bool>^ FToggleOutput;

			[Output("Integer")]
			ISpread<int>^ FIntegerOutput;

			[Output("Double")]
			ISpread<double>^ FDoubleOutput;

			[Output("String")]
			ISpread<String^>^ FStringOutput;

			[Output("Color")]
			ISpread<RGBAColor>^ FColorOutput;

			[Output("Direction")]
			ISpread<Vector3D>^ FDirectionOutput;

			[Output("Quaternion")]
			ISpread<Vector4D>^ FQuaternionOutput;


			//----------------------ILogger
			[Import()]
			ILogger^ FLogger;


			//----------------------その他のフィールド
			
			//slice count
			//int FSpreadCount;
		};
	}
}