#ifndef GAMEGLOBAL_HEAD_FILE
#define GAMEGLOBAL_HEAD_FILE

#include "AirGame.h"
#include "AirGameHeader.h"
namespace Air{
	
	namespace Client{
	
		namespace	Game{
			namespace	Data{
				class	ISystem;
			}
			class	System;
			class	Section;
			/**	\brief	��Ϸϵͳȫ�ֱ���
			*
			*	��Ϸϵͳȫ�ֱ���
			*
			***/
			class	Global{
			public:
				Global();
	
				Data::ISystem*	m_pDataSystem;
				System*		m_pSystem;
				Section*		m_pSection;
				Section*		m_pLoadingSection;
				//UI::Flash::IISystem*	m_pUIFlashSys;
			};
	
			GAME_EXPORT	Global&	GetGlobal();
		}
	
	};
};
#endif // GAMEGLOBAL_HEAD_FILE