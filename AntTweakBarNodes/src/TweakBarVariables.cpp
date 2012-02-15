#include "Stdafx.h"
#include "TweakBarVariables.h"

using namespace VVVV::DataTypes;


//Constructor
TweakBarVariables::TweakBarVariables()
{
	//create instance of UnmanagedClass for keep variables
	this->unmanagedClass = new UnmanagedClass();

	//init Dictionaries
	this->FDictToggle = gcnew Dictionary<String^, int>();
	this->FDictInteger = gcnew Dictionary<String^, int>();
	this->FDictDouble = gcnew Dictionary<String^, int>();
	this->FDictString = gcnew Dictionary<String^, int>();
	this->FDictColor = gcnew Dictionary<String^, int>();
	this->FDictDirection = gcnew Dictionary<String^, int>();
	this->FDictQuaternion = gcnew Dictionary<String^, int>();
	this->FDictSeparator = gcnew Dictionary<String^, int>();

	//init Dictionary of Variable Dictionaries
	this->FDictDict = gcnew Dictionary<String^, Dictionary<String^, int>^>();
	this->FDictDict->Add("Toggle", this->FDictToggle);
	this->FDictDict->Add("Integer", this->FDictInteger);
	this->FDictDict->Add("Double", this->FDictDouble);
	this->FDictDict->Add("String", this->FDictString);
	this->FDictDict->Add("Color", this->FDictColor);
	this->FDictDict->Add("Direction", this->FDictDirection);
	this->FDictDict->Add("Quaternion", this->FDictQuaternion);
	this->FDictDict->Add("Separator", this->FDictSeparator);
}

//Destructor
TweakBarVariables::~TweakBarVariables()
{
	//release managed object

	this->!TweakBarVariables();
}

//Finalizer
TweakBarVariables::!TweakBarVariables()
{
	//release unmanaged object
	delete this->unmanagedClass;
}



//add variable
void TweakBarVariables::AddVariable(String^ type, String^ name, int index)
{
	if(this->FDictDict->ContainsKey(type)) this->FDictDict[type]->Add(name, index);
}

//remove variable
void TweakBarVariables::RemoveVariable(String^ name)
{
	for each(Dictionary<String^, int>^ dict in this->FDictDict->Values)
	{
		if(dict->ContainsKey(name))
		{
			dict->Remove(name);
			break;
		}
	}
}

//find variable
bool TweakBarVariables::FindVariable(String^ name)
{
	bool b = false;
	
	for each(Dictionary<String^, int>^ dict in this->FDictDict->Values)
	{
		if(dict->ContainsKey(name))
		{
			b = true;
			break;
		}
	}

	return b;
}

//get index of variable
int TweakBarVariables::GetIndex(String^ name)
{
	int index = -1;

	for each(Dictionary<String^, int>^ dict in this->FDictDict->Values)
	{
		if(dict->ContainsKey(name))
		{
			index = dict[name];
			break;
		}
	}

	return index;
}

//get empty index
int TweakBarVariables::GetEmptyIndex(String^ type)
{
	if(this->FDictDict->ContainsKey(type))
	{
		Dictionary<String^, int>^ dict = this->FDictDict[type];
		
		for(int index = 0; index < MAX_COUNT; index++)
		{
			//return index when find empty index from small value
			if(dict->ContainsValue(index) == false) return index;
		}

		//if failed, return -1
		return -1;
	}
	else
	{
		return -1;
	}
}

//get pointer of variable
void* TweakBarVariables::GetPointer(String^ type, int index)
{
	void *var;

	//init variable and get pointer
	//use "if" because cannot use "switch" with String^ type
	if(type == "Toggle")
	{
		this->unmanagedClass->toggleArray[index] = false;

		var = &(this->unmanagedClass->toggleArray[index]);
	}
	else if(type == "Integer")
	{
		this->unmanagedClass->integerArray[index] = 0;

		var = &(this->unmanagedClass->integerArray[index]);
	}
	else if(type == "Double")
	{
		this->unmanagedClass->doubleArray[index] = 0.0;

		var = &(this->unmanagedClass->doubleArray[index]);
	}
	else if(type == "String")
	{
		this->unmanagedClass->stringArray[index][0] = NULL;
		
		var = &(this->unmanagedClass->stringArray[index][0]);
	}
	else if(type == "Color")
	{
		this->unmanagedClass->colorArray[index][0] = 0.0;
		this->unmanagedClass->colorArray[index][1] = 0.0;
		this->unmanagedClass->colorArray[index][2] = 0.0;
		this->unmanagedClass->colorArray[index][3] = 1.0;

		var = &(this->unmanagedClass->colorArray[index][0]);
	}
	else if(type == "Direction")
	{
		this->unmanagedClass->directionArray[index][0] = 0.0;
		this->unmanagedClass->directionArray[index][1] = 1.0;
		this->unmanagedClass->directionArray[index][2] = 0.0;

		var = &(this->unmanagedClass->directionArray[index][0]);
	}
	else if(type == "Quaternion")
	{
		this->unmanagedClass->quaternionArray[index][0] = 0.0;
		this->unmanagedClass->quaternionArray[index][1] = 0.0;
		this->unmanagedClass->quaternionArray[index][2] = 0.0;
		this->unmanagedClass->quaternionArray[index][3] = 1.0;

		var = &(this->unmanagedClass->quaternionArray[index][0]);
	}

	return var;
}


//get type of variable
String^ TweakBarVariables::GetType(String^ name)
{
	for each(KeyValuePair<String^, Dictionary<String^, int>^> pair in this->FDictDict)
	{
		if(pair.Value->ContainsKey(name)) return pair.Key;
	}

	//if failed return ""
	return "";
}

//get variable
bool TweakBarVariables::GetToggle(String^ name)
{
	int index;
	
	index = this->GetIndex(name);

	if(index == -1)
	{
		return false;
	}
	else
	{
		return this->unmanagedClass->toggleArray[index];
	}
}

int TweakBarVariables::GetInteger(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return 0;
	}
	else
	{
		return this->unmanagedClass->integerArray[index];
	}
}

double TweakBarVariables::GetDouble(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return 0.0;
	}
	else
	{
		return this->unmanagedClass->doubleArray[index];
	}
}

String^ TweakBarVariables::GetString(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return "";
	}
	else
	{
		return gcnew String(&(this->unmanagedClass->stringArray[index][0]));
	}
}

RGBAColor^ TweakBarVariables::GetColor(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return gcnew RGBAColor(0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		return gcnew RGBAColor((double)this->unmanagedClass->colorArray[index][0],
							   (double)this->unmanagedClass->colorArray[index][1],
							   (double)this->unmanagedClass->colorArray[index][2],
							   (double)this->unmanagedClass->colorArray[index][3]);
	}
}

Vector3D^ TweakBarVariables::GetDirection(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return gcnew Vector3D(0.0);
	}
	else
	{
		return gcnew Vector3D(this->unmanagedClass->directionArray[index][0],
							  this->unmanagedClass->directionArray[index][1],
							  this->unmanagedClass->directionArray[index][2]);
	}
}

Vector4D^ TweakBarVariables::GetQuaternion(String^ name)
{
	int index;

	index = this->GetIndex(name);

	if(index == -1)
	{
		return gcnew Vector4D(0.0);
	}
	else
	{
		return gcnew Vector4D(this->unmanagedClass->quaternionArray[index][0],
							  this->unmanagedClass->quaternionArray[index][1],
							  this->unmanagedClass->quaternionArray[index][2],
							  this->unmanagedClass->quaternionArray[index][3]);
	}
}

//get dictionary
Dictionary<String^, int>^ TweakBarVariables::GetDictionary(String^ type)
{
	if(this->FDictDict->ContainsKey(type))
	{
		return this->FDictDict[type];
	}
	else
	{
		//if failed return nullptr
		return nullptr;
	}
}

//clear All Dictionary
void TweakBarVariables::ClearAllDictionary()
{
	for each(Dictionary<String^, int>^ dict in this->FDictDict->Values)
	{
		//clear Dictionary
		dict->Clear();
	}
}