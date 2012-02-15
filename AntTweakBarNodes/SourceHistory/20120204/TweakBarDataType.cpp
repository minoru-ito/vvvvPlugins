#include "Stdafx.h"
#include "TweakBarDataType.h"

namespace VVVV
{
	namespace DataTypes
	{
		TweakBarDataType::TweakBarDataType(void)
		{
			this->myBar = nullptr;
		}

		TweakBarDataType::~TweakBarDataType(void)
		{
			if(this->myBar != nullptr)
			{
				delete this->myBar;
				this->myBar = nullptr;
			}
		}
		
		TwBar* TweakBarDataType::GetTwBar()
		{
			return this->myBar;
		}

		void TweakBarDataType::SetTwBar(TwBar* bar)
		{
			this->myBar = bar;
		}
		
	}
}