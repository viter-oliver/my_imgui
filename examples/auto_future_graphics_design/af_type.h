#pragma once
//#prama execution_character_set("utf-8")
namespace auto_future
{
	template <class T>
	struct af_vt2{
		T x, y;
		//af_vt2() = default;
		af_vt2& operator +=(af_vt2& tar)
		{
			this->x += tar.x;
			this->y += tar.y;
			return *this;
		}
		af_vt2 operator+ (af_vt2& tar)
		{
			af_vt2 av{ this->x + tar.x, this->y + tar.y };
			return av;
		}
		af_vt2 operator -(af_vt2& tar)
		{
			af_vt2 av{ this->x - tar.x, this->y - tar.y };
			return av;
		}
	};
	using af_vi2 = af_vt2<int>;
	using af_vui2 = af_vt2<unsigned int>;
	using af_vec2 = af_vt2<float>;
	template <class T>
	struct af_vt3{
		T x, y,z;
		//af_vt2() = default;
		af_vt3& operator +=(af_vt3& tar)
		{
			this->x += tar.x;
			this->y += tar.y;
			this->z += tar.z;

			return *this;
		}
		af_vt3 operator+ (af_vt3& tar)
		{
			af_vt3 av{ this->x + tar.x, this->y + tar.y, this->z + tar.z};
			return av;
		}
		af_vt3 operator -(af_vt3& tar)
		{
			af_vt3 av{ this->x - tar.x, this->y - tar.y, this->z - tar.z };
			return av;
		}
	};
	using af_vi3 = af_vt3<int>;
	using af_vui3 = af_vt3<unsigned int>;
	using af_vec3 = af_vt3<float>;
	template <class T>
	struct af_vt4{
		T x, y,z,w;
		//af_vt2() = default;
		af_vt4& operator +=(af_vt4& tar)
		{
			this->x += tar.x;
			this->y += tar.y;
			this->w += tar.w;
			this->z += tar.z;

			return *this;
		}
		af_vt4 operator+ (af_vt4& tar)
		{
			af_vt4 av{ this->x + tar.x, this->y + tar.y, this->z + tar.z, this->w + tar.w };
			return av;
		}
		af_vt4 operator -(af_vt4& tar)
		{
			af_vt4 av{this->x-tar.x,this->y-tar.y,this->z-tar.z,this->w-tar.w};
			return av;
		}
	};
	using af_vi4 = af_vt4<int>;
	using af_vui4 = af_vt4<unsigned int>;
	using af_vec4 = af_vt4<float>;
	struct ft_vertex
	{

		af_vec2  pos;
		af_vec2  uv;
		unsigned int col;
	};
}