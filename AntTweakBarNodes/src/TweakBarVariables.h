// 
// TweakBarVariables.h
// 
// holding variables for TweakBar
// 
// updated: 2012/02/10
// written: mino

#pragma once

// max count of each variables (temporary)
#define MAX_COUNT 20

// length of String
#define STR_LENGTH 64

namespace VVVV
{
	namespace DataTypes
	{
		//http://akatukisiden.wordpress.com/2011/12/23/cpp-ccli-wrapping-object/
		//Unmanaged class for variables.
		//you must check manually because instance will not checked by GC.
		public class UnmanagedClass
		{
		public:

			bool toggleArray[MAX_COUNT];
			int integerArray[MAX_COUNT];
			double doubleArray[MAX_COUNT];
			char stringArray[MAX_COUNT][STR_LENGTH];	//C-Static sized null-terminated String.
			float colorArray[MAX_COUNT][4];				//4 float for RGBA
			double directionArray[MAX_COUNT][3];		//3 double for XYZ
			double quaternionArray[MAX_COUNT][4];		//4 double for XYZW
		};


		//managed class
		public ref class TweakBarVariables
		{
		public:

			//Constructor
			TweakBarVariables();
			
			//Destructor
			~TweakBarVariables();

			//add variable
			void AddVariable(String^ type, String^ name, int index);

			//remove variable
			void RemoveVariable(String^ name);

			//find variable
			bool FindVariable(String^ name);

			//get index of variable
			int GetIndex(String^ name);

			//get empty index
			int GetEmptyIndex(String^ type);

			//get pointer of variable
			//(get with name will better than index)
			void* GetPointer(String^ type, int index);

			//get type of variable
			String^ GetType(String^ name);

			//get variable
			bool GetToggle(String^ name);
			int GetInteger(String^ name);
			double GetDouble(String^ name);
			String^ GetString(String^ name);
			RGBAColor^ GetColor(String^ name);
			Vector3D^ GetDirection(String^ name);
			Vector4D^ GetQuaternion(String^ name);

			//get Dictionary
			Dictionary<String^, int>^ GetDictionary(String^ type);

			//clear Dictionary
			void ClearAllDictionary();

		protected:

			//Finalizer
			!TweakBarVariables();

		private:
			
			//Dictionary for variable name and index of unmanaged array
			Dictionary<String^, int>^ FDictToggle;
			Dictionary<String^, int>^ FDictInteger;
			Dictionary<String^, int>^ FDictDouble;
			Dictionary<String^, int>^ FDictString;
			Dictionary<String^, int>^ FDictColor;
			Dictionary<String^, int>^ FDictDirection;
			Dictionary<String^, int>^ FDictQuaternion;
			Dictionary<String^, int>^ FDictSeparator;

			//Dictionary of Variable Dictionaries
			Dictionary<String^, Dictionary<String^, int>^>^ FDictDict;

			//unmanaged class for keep variables
			UnmanagedClass* unmanagedClass;

		};
	}
}