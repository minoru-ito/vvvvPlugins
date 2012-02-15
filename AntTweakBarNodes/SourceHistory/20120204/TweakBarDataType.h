#pragma once


namespace VVVV
{
	namespace DataTypes
	{
		public ref class TweakBarDataType : IDisposable
		{
		public:
			TweakBarDataType(void);
			~TweakBarDataType(void);
			
			TwBar* GetTwBar();
			void SetTwBar(TwBar* bar);

		private:
			TwBar* myBar;
		};
	}
}