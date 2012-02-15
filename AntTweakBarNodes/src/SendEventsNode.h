// 
// SendEventsNode.h
// 
// AntTweakBar in vvvv
// useful when create multiple tweakbars
// 
// updated: 2012/02/07
// written: Minoru Ito

#pragma once


namespace VVVV
{
	namespace Nodes
	{
		//PluginInfo
		[PluginInfo(Name = "SendEvents", Category = "AntTweakBar", Help = "", Tags = "", Author = "mino")]
		//Node class
		public ref class SendEventsNode : IPluginEvaluate
		{

		public:

			//Constructor
			//[ImportingConstructor()]
			//SendEventsNode(IPluginHost2^ host);
			
			//Destructor
			//~SendEventsNode();

			//Finalizer
			//!SendEventsNode();
			
			//----------------------Interface methods
			//IPluginEvaluate
			virtual void Evaluate(int SpreadMax);

		private:

			void CreateKeyEvent(int keyCodeWin32, int modifiers);

			
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
			

			//---------other

			[Input("Enabled", DefaultValue = 1.0, IsSingle = true)]
			ISpread<bool>^ FEnabled;


			//---------------------------------------------------Output pins

			//[Output("Message", IsSingle = true, Visibility = PinVisibility::OnlyInspector)]
			//ISpread<String^>^ FMessage;

			
			//----------------------ILogger
			[Import()]
			ILogger^ FLogger;


			//----------------------Other

			//PluginHost
			//IPluginHost2^ FHost;
		};
	}
}