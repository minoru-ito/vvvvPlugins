#include "stdafx.h"

#include "GetVariableNode.h"

using namespace VVVV::Nodes;

GetVariableNode::GetVariableNode(IPluginHost2^ host)
{
	//TweakBarVariables‚Ì‰Šú‰»
	//FTweakBarVariables = gcnew TweakBarVariables();
}

//called when data for any output pin is requested
void GetVariableNode::Evaluate(int SpreadMax)
{
	//FLogger->Log(LogType::Debug, "I am from Evaluate");
	
	if(FVariablesInput->IsChanged)
    {
		if(FVariablesInput[0] != nullptr)
		{
			//FLogger->Log(LogType::Debug, "TweakBar variables connected");

			//set TweakBar Variables
			FVariables = (TweakBarVariables^)FVariablesInput[0];
		}
		else
		{
			//FLogger->Log(LogType::Debug, "TweakBar variables disconnected");

			//release TweakBar Variables
			FVariables = nullptr;

			//reset Output pins
			FToggleOutput->SliceCount = 0;
			FIntegerOutput->SliceCount = 0;
			FDoubleOutput->SliceCount = 0;
			FStringOutput->SliceCount = 0;
			FColorOutput->SliceCount = 0;
			FDirectionOutput->SliceCount = 0;
			FQuaternionOutput->SliceCount = 0;
		}
	}

	if(FVariables != nullptr)
	{
		String^ str;

		//if(FIntegerName->IsChanged)
		
		//Toggle
		if(FToggleName[0] == "")
		{
			FToggleOutput->SliceCount = 0;
		}
		else
		{
			FToggleOutput->SliceCount = FToggleName->SliceCount;
			
			for(int i = 0; i < FToggleName->SliceCount; i++)
			{
				str = (String^)FToggleName[i];

				if(FVariables->GetType(str) == "Toggle")
				{
					FToggleOutput[i] = FVariables->GetToggle(str);
				}
				else
				{
					FToggleOutput[i] = false;
				}
			}
		}

		//Integer
		if(FIntegerName[0] == "")
		{
			FIntegerOutput->SliceCount = 0;
		}
		else
		{
			FIntegerOutput->SliceCount = FIntegerName->SliceCount;
			
			for(int i = 0; i < FIntegerName->SliceCount; i++)
			{
				str = (String^)FIntegerName[i];

				if(FVariables->GetType(str) == "Integer")
				{
					FIntegerOutput[i] = FVariables->GetInteger(str);
				}
				else
				{
					FIntegerOutput[i] = 0;
				}
			}
		}

		//Double
		if(FDoubleName[0] == "")
		{
			FDoubleOutput->SliceCount = 0;
		}
		else
		{
			FDoubleOutput->SliceCount = FDoubleName->SliceCount;
			
			for(int i = 0; i < FDoubleName->SliceCount; i++)
			{
				str = (String^)FDoubleName[i];

				if(FVariables->GetType(str) == "Double")
				{
					FDoubleOutput[i] = FVariables->GetDouble(str);
				}
				else
				{
					FDoubleOutput[i] = 0;
				}
			}
		}

		//Color
		if(FColorName[0] == "")
		{
			FColorOutput->SliceCount = 0;
		}
		else
		{
			FColorOutput->SliceCount = FColorName->SliceCount;
			
			for(int i = 0; i < FColorName->SliceCount; i++)
			{
				str = (String^)FColorName[i];

				if(FVariables->GetType(str) == "Color")
				{
					FColorOutput[i] = FVariables->GetColor(str);
				}
				else
				{
					FColorOutput[i] = gcnew RGBAColor(0.0, 0.0, 0.0, 1.0);
				}
			}
		}

		//Direction
		if(FDirectionName[0] == "")
		{
			FDirectionOutput->SliceCount = 0;
		}
		else
		{
			FDirectionOutput->SliceCount = FDirectionName->SliceCount;
			
			for(int i = 0; i < FDirectionName->SliceCount; i++)
			{
				str = (String^)FDirectionName[i];

				if(FVariables->GetType(str) == "Direction")
				{
					FDirectionOutput[i] = FVariables->GetDirection(str);
				}
				else
				{
					FDirectionOutput[i] = gcnew Vector3D(0.0);
				}
			}
		}

		//Quaternion
		if(FQuaternionName[0] == "")
		{
			FQuaternionOutput->SliceCount = 0;
		}
		else
		{
			FQuaternionOutput->SliceCount = FQuaternionName->SliceCount;
			
			for(int i = 0; i < FQuaternionName->SliceCount; i++)
			{
				str = (String^)FQuaternionName[i];

				if(FVariables->GetType(str) == "Quaternion")
				{
					FQuaternionOutput[i] = FVariables->GetQuaternion(str);
				}
				else
				{
					FQuaternionOutput[i] = gcnew Vector4D(0.0);
				}
			}
		}

	}
}
