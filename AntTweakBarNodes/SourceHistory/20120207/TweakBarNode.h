// 
// TweakBarNode.h
// 
// AntTweakBar in vvvv
// 
// updated: 2012/02/07
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
			Separator
		};

		
		//PluginInfo
		[PluginInfo(Name = "TweakBar", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//Node class
		public ref class TweakBarNode : DXLayerOutPluginBase<DeviceData^>, IPluginEvaluate
		{

		public:

			//Constructor
			[ImportingConstructor()]
			TweakBarNode(IPluginHost2^ host);
			
			//Destructor
			~TweakBarNode();

			//Finalizer
			!TweakBarNode();
			
			//----------------------Interface methods
			//IPluginEvaluate
			virtual void Evaluate(int SpreadMax);
			//DXLayerOutPluginBase
			virtual DeviceData^ CreateDeviceData(Device^ device) override;
			virtual void UpdateDeviceData(DeviceData^ deviceData) override;
			virtual void DestroyDeviceData(DeviceData^ deviceData, bool OnlyUnManaged) override;
			virtual void Render(Device^ device, DeviceData^ deviceData) override;

			static int GBarCount;			//check TweakBar count
			static int GRenderCounter;		//use for call TwDraw() only once

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

			
			//---------------------------------------------------Input pins

			//---------events

			//Mouse Button
			[Input("Mouse Button", IsSingle = true)]
			IDiffSpread<bool>^ FMouseButton;
			
			//Mouse XY
			[Input("Mouse XY", DefaultValue = 0.0)]
			IDiffSpread<double>^ FMouseXY;
			
			//Win32 virtual keycode
			[Input("KeyCode")]
			IDiffSpread<int>^ FKeyCode;
			
			//Renderer width/height
			[Input("Window Size", DefaultValue = 400)]
			IDiffSpread<int>^ FWindowSize;
			

			//---------Bar settings

			//Title
			[Input("Title", IsSingle = true, DefaultString = "TweakBar")]
			IDiffSpread<String^>^ FTitle;
			
			//Background color
			//default color of TweakBar: (R:17, G:109, B:143, A:64)
			[Input("Color", IsSingle = true, DefaultColor = gcnew array<double>{(double)17/255, (double)109/255, (double)143/255, (double)64/255})]
			IDiffSpread<RGBAColor>^ FColor;
			
			//Text color
			[Input("Text", IsSingle = true, DefaultEnumEntry = "Light")]
			IDiffSpread<TextEnum>^ FText;


			//---------variable settings

			//Add Type
			[Input("Add Type", DefaultEnumEntry = "Double")]
			ISpread<TypeEnum>^ FType;

			//Add Name
			[Input("Add Name")]
			IDiffSpread<String^>^ FAddName;

			//Add Parameters
			[Input("Add Parameters")]
			ISpread<String^>^ FAddParameters;

			//Add Bang
			[Input("Add", IsSingle = true, IsBang = true)]
			ISpread<bool>^ FAdd;
			
			//Remove Name
			[Input("Remove Name")]
			IDiffSpread<String^>^ FRemoveName;

			//Remove Bang
			[Input("Remove", IsSingle = true, IsBang = true)]
			ISpread<bool>^ FRemove;
			
			//ボタン追加テスト
			//マネージとアンマネージのcallBackなどやや複雑なのでとりあえず保留
			//[Input("Add Button", IsSingle = true, IsBang = true)]
			//ISpread<bool>^ FAddButton;
			
			//Add Separator : join to Add Variable
			//[Input("Add Separator", IsSingle = true, IsBang = true)]
			//ISpread<bool>^ FAddSeparator;


			//---------other

			[Input("Enabled", DefaultValue = 1.0, IsSingle = true)]
			IDiffSpread<bool>^ FEnabled;


			//---------------------------------------------------Output pins

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

			[Output("Message", IsSingle = true, Visibility = PinVisibility::OnlyInspector)]
			ISpread<String^>^ FMessage;

			
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

			//SlimDX StateBlock for Render method
			StateBlock^ block;

			//slice count
			//int FSpreadCount;
		};
	}
}