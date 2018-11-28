#pragma once

#include "FBXImporterCommon.h"
#include <atomic>

namespace FBXImporterUnmanaged
{
#pragma warning( disable: 4251 )
	typedef std::atomic_int32_t aint32;

	class RCObject
	{
		aint32 counter;

	protected:
		virtual inline void deleteMethod( RCObject* object )
		{
			assert( counter == 0 );
			delete object;
		}

	public:
		RCObject() : counter( 0 ) {}
		virtual ~RCObject() { assert( counter == 0 ); }

		inline void addReference()
		{
			++counter;
		}

		inline void removeReference()
		{
			assert( counter > 0 );
			--counter;
			if( counter == 0 )
			{
				deleteMethod( this );
			}
		}

		inline int getReferenceCount() { return counter; }
	};

	template<typename T>
	class RCPtr
	{
		T* data;

	public:
		RCPtr()
			: data( 0 )
		{
		}

		RCPtr( T* p )
			: data( p )
		{
			if( data )
			{
				data->addReference();
			}
		}

		RCPtr( const RCPtr& other )
			: data( other.data )
		{
			if( data )
			{
				data->addReference();
			}
		}

		virtual ~RCPtr()
		{
			if( data )
			{
				data->removeReference();
			}
		}

		RCPtr& operator=( T* p )
		{
			if( p )
			{
				p->addReference();
			}

			if( data )
			{
				data->removeReference();
			}

			data = p;
			return *this;
		}

		RCPtr& operator=( const RCPtr& rhs )
		{
			return *this = rhs.data;
		}

		T* get() const
		{
			return data;
		}

		T* operator->( ) const { return data; }
		T& operator*( ) const { return *data; }
		operator T*( ) const { return data; }
		operator bool() const { return data != 0; }
		bool IsNull() const { return data == 0; }
		static RCPtr<T> Null;
	};

	template<typename T>
	RCPtr<T> RCPtr<T>::Null = RCPtr<T>();

	template<typename T>
	bool operator==( const RCPtr<T>& lhs, RCPtr<T>& rhs )
	{
		return lhs.get() == rhs.get();
	}

	template<typename T>
	bool operator!=( const RCPtr<T>& lhs, RCPtr<T>& rhs )
	{
		return lhs.get() != rhs.get();
	}

	template<typename T>
	bool operator==( const T* lhs, const RCPtr<T>& rhs )
	{
		return lhs == rhs.get();
	}

	template<typename T>
	bool operator!=( const T* lhs, const RCPtr<T>& rhs )
	{
		return lhs != rhs.get();
	}

	template<typename T>
	bool operator==( const RCPtr<T>& lhs, const T* rhs )
	{
		return lhs.get() == rhs;
	}

	template<typename T>
	bool operator!=( const RCPtr<T>& lhs, const T* rhs )
	{
		return lhs.get() != rhs;
	}

	template<typename T, typename U>
	RCPtr<T> DynamicCast( const RCPtr<U> p )
	{
		T* r = dynamic_cast<T*>( p.get() );
		return r != 0 ? RCPtr<T>( r ) : RCPtr<T>();
	}

	template<typename T, typename U>
	RCPtr<T> StaticCast( const RCPtr<U> p )
	{
		T* r = static_cast<T*>( p.get() );
		return r != 0 ? RCPtr<T>( r ) : RCPtr<T>();
	}

	template<typename T, typename U>
	RCPtr<T> ReinterpretCast( const RCPtr<U> p )
	{
		T* r = reinterpret_cast<T*>( p.get() );
		return RCPtr<T>( r );
	}
}