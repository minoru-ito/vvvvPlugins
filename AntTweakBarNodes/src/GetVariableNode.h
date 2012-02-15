// 
// GetVariableNode.h
// 
// get variables of TweakBar
// 
// updated: 2012/02/10
// written: mino

#pragma once

//common using namespace move to Stdafx.h

#include "TweakBarVariables.h"
using namespace VVVV::DataTypes;


namespace VVVV
{
	namespace Nodes
	{
		//PluginInfo
		[PluginInfo(Name = "GetVariable", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//Node class
		public ref class GetVariableNode : IPluginEvaluate
		{

		public:

			//Constructor
			//[ImportingConstructor()]
			//GetVariableNode(IPluginHost2^ host);

			
			//IPluginEvaluate method
			virtual void Evaluate(int SpreadMax);

		protected:

			//

		private:

			//---------------------------------Input pins
			
			//getting TweakBar variables
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


			//---------------------------------Output pins

			//[Output("Test String")]
			//ISpread<String^>^ FTestString;

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
			//[Import()]
			//ILogger^ FLogger;


			//----------------------Other
			
			//slice count
			//int FSpreadCount;
		};
	}
}