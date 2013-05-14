#ifndef AirParticleEmitter_h__
#define AirParticleEmitter_h__

#include "AirEngineHeader.h"
#include "AirParticleElement.h"
namespace	Air{
	namespace	Engine{
		class	Particle;

		class	ENGINE_EXPORT	ParticleEmitter	:	public	IProduct{
		public:
			struct Info{
				float		fFreq;
				float		fElementLife;
			};
			static	AString	ProductTypeName;
			ParticleEmitter(CAString& strName,Info* pInfo);
			virtual	U1		Create();
			virtual	U1		Destroy();
			virtual	void	Update(const FrameTime& frameTime,Particle* pParticle);
			virtual	void	ElementBorn(const FrameTime& frameTime,PElementList& lst);
		protected:
			float			m_fFreq;
			float			m_fUpdateTime;
			float			m_fElementLife;
		};

		class	ENGINE_EXPORT	BoxEmitter	:	public ParticleEmitter
		{
		public:
			struct Info	:	ParticleEmitter::Info{
				Float3	vCenter;
				Float3	vHalfSize;
			};
			static	AString	ProductTypeName;
			BoxEmitter(CAString& strName,BoxEmitter::Info* pInfo);
			virtual	void	ElementBorn(const FrameTime& frameTime,PElementList& lst);
		protected:
			Float3	m_vCenter;
			Float3	m_vHalfSize;
		};

		class ENGINE_EXPORT	SphereEmitter	:	public ParticleEmitter{
		public:
			struct Info	:	ParticleEmitter::Info{
				Float3	vCenter;
				float	fRadius;
			};
			static	AString	ProductTypeName;
			SphereEmitter(CAString& strName,SphereEmitter::Info* pInfo);
			virtual	void	ElementBorn(const FrameTime& frameTime,PElementList& lst);
		protected:
			Float3	m_vCenter;
			float	m_fRadius;
		};

	}
}
#endif // AirParticleEmitter_h__