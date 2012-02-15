#include "Stdafx.h"
#include "TweakBarVariables.h"

using namespace VVVV::DataTypes;


//Constructor
TweakBarVariables::TweakBarVariables()
{
	//変数を保持するアンマネージドクラスのインスタンスを生成
	this->unmanagedClass = new UnmanagedClass();

	//Dictionaryの初期化
	this->FDictToggle = gcnew Dictionary<String^, int>();
	this->FDictInteger = gcnew Dictionary<String^, int>();
	this->FDictDouble = gcnew Dictionary<String^, int>();
	this->FDictString = gcnew Dictionary<String^, int>();
	this->FDictColor = gcnew Dictionary<String^, int>();
	this->FDictDirection = gcnew Dictionary<String^, int>();
	this->FDictQuaternion = gcnew Dictionary<String^, int>();
}

//Destructor
TweakBarVariables::~TweakBarVariables()
{
	//release managed

	this->!TweakBarVariables();
}

//Finalizer
TweakBarVariables::!TweakBarVariables()
{
	//release unmanaged
	delete this->unmanagedClass;
}



//add variable
void TweakBarVariables::AddVariable(String^ type, String^ name, int index)
{
	//switch文の条件にString^を使えないのでif文にする
	if(type == "Toggle") this->FDictToggle->Add(name, index);
	else if(type == "Integer") this->FDictInteger->Add(name, index);
	else if(type == "Double") this->FDictDouble->Add(name, index);
	else if(type == "String") this->FDictString->Add(name, index);
	else if(type == "Color") this->FDictColor->Add(name, index);
	else if(type == "Direction") this->FDictDirection->Add(name, index);
	else if(type == "Quaternion") this->FDictQuaternion->Add(name, index);
}

//remove variable
void TweakBarVariables::RemoveVariable(String^ name)
{
	if(this->FDictToggle->ContainsKey(name))
	{
		this->FDictToggle->Remove(name);
	}
	else if(this->FDictInteger->ContainsKey(name))
	{
		this->FDictInteger->Remove(name);
	}
	else if(this->FDictDouble->ContainsKey(name))
	{
		this->FDictDouble->Remove(name);
	}
	else if(this->FDictString->ContainsKey(name))
	{
		this->FDictString->Remove(name);
	}
	else if(this->FDictColor->ContainsKey(name))
	{
		this->FDictColor->Remove(name);
	}
	else if(this->FDictDirection->ContainsKey(name))
	{
		this->FDictDirection->Remove(name);
	}
	else if(this->FDictQuaternion->ContainsKey(name))
	{
		this->FDictQuaternion->Remove(name);
	}
}

//find variable
bool TweakBarVariables::FindVariable(String^ name)
{
	bool b = false;
	
	if(this->FDictToggle->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictInteger->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictDouble->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictString->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictColor->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictDirection->ContainsKey(name))
	{
		b = true;
	}
	else if(this->FDictQuaternion->ContainsKey(name))
	{
		b = true;
	}

	return b;
}

//get index of variable
int TweakBarVariables::GetIndex(String^ name)
{
	int index = -1;

	if(this->FDictToggle->ContainsKey(name))
	{
		index = this->FDictToggle[name];
	}
	else if(this->FDictInteger->ContainsKey(name))
	{
		index = this->FDictInteger[name];
	}
	else if(this->FDictDouble->ContainsKey(name))
	{
		index = this->FDictDouble[name];
	}
	else if(this->FDictString->ContainsKey(name))
	{
		index = this->FDictString[name];
	}
	else if(this->FDictColor->ContainsKey(name))
	{
		index = this->FDictColor[name];
	}
	else if(this->FDictDirection->ContainsKey(name))
	{
		index = this->FDictDirection[name];
	}
	else if(this->FDictQuaternion->ContainsKey(name))
	{
		index = this->FDictQuaternion[name];
	}

	return index;
}

//get empty index
int TweakBarVariables::GetEmptyIndex(String^ type)
{
	int index = -1;

	//switch文の条件にString^を使えないのでif文にする
	if(type == "Toggle")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			//若い番号から"空き"を見つけたら抜ける
			if(this->FDictToggle->ContainsValue(index) == false) break;
		}
	}
	else if(type == "Integer")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictInteger->ContainsValue(index) == false) break;
		}
	}
	else if(type == "Double")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictDouble->ContainsValue(index) == false) break;
		}
	}
	else if(type == "String")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictString->ContainsValue(index) == false) break;
		}
	}
	else if(type == "Color")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictColor->ContainsValue(index) == false) break;
		}
	}
	else if(type == "Direction")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictDirection->ContainsValue(index) == false) break;
		}
	}
	else if(type == "Quaternion")
	{
		for(index = 0; index < MAX_COUNT; index++)
		{
			if(this->FDictQuaternion->ContainsValue(index) == false) break;
		}
	}

	return index;
}

//get pointer of variable
void* TweakBarVariables::GetPointer(String^ type, int index)
{
	void *var;

	//switch文の条件にString^を使えないのでif文にする
	if(type == "Toggle") var = &(this->unmanagedClass->toggleArray[index]);
	else if(type == "Integer") var = &(this->unmanagedClass->integerArray[index]);
	else if(type == "Double") var = &(this->unmanagedClass->doubleArray[index]);
	else if(type == "String") var = nullptr;
	else if(type == "Color") var = &(this->unmanagedClass->colorArray[index][0]);
	else if(type == "Direction") var = &(this->unmanagedClass->directionArray[index][0]);
	else if(type == "Quaternion") var = &(this->unmanagedClass->quaternionArray[index][0]);
	
	return var;
}


//get type of variable
String^ TweakBarVariables::GetType(String^ name)
{
	String^ type = "";

	if(this->FDictToggle->ContainsKey(name))
	{
		type = "Toggle";
	}
	else if(this->FDictInteger->ContainsKey(name))
	{
		type = "Integer";
	}
	else if(this->FDictDouble->ContainsKey(name))
	{
		type = "Double";
	}
	else if(this->FDictString->ContainsKey(name))
	{
		type = "String";
	}
	else if(this->FDictColor->ContainsKey(name))
	{
		type = "Color";
	}
	else if(this->FDictDirection->ContainsKey(name))
	{
		type = "Direction";
	}
	else if(this->FDictQuaternion->ContainsKey(name))
	{
		type = "Quaternion";
	}

	return type;
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