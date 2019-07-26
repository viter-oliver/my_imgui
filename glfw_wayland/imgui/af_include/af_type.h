#pragma once
//#prama execution_character_set("utf-8")
#define TXT_BUFF_SZ 1024 * 16

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
		af_vt2& operator -=(af_vt2& tar)
		{
			this->x -= tar.x;
			this->y -= tar.y;
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
		bool operator == (af_vt2& tar)
		{
			return x == tar.x&&y == tar.y;
		}
		af_vt2& operator *(T scl)
		{
			this->x *= scl;
			this->y *= scl;
			return *this;
		}
		float norm()
		{
			return sqrt(x*x + y*y);
		}
	};
	using af_vi2 = af_vt2<int>;
	using af_vui2 = af_vt2<unsigned int>;
	using af_vs2 = af_vt2<short>;
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
		af_vt3& operator -=(af_vt3& tar)
		{
			this->x -= tar.x;
			this->y -= tar.y;
			this->z -= tar.z;
			return *this;
		}
		af_vt3& operator *(T scl)
		{
			this->x *= scl;
			this->y *= scl;
			this->z *= scl;
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
		af_vt4& operator -=(af_vt4& tar)
		{
			this->x -= tar.x;
			this->y -= tar.y;
			this->w -= tar.w;
			this->z -= tar.z;
			return *this;
		}
		af_vt4& operator *(T scl)
		{
			this->x *= scl;
			this->y *= scl;
			this->z *= scl;
			this->w *= scl;
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

	struct camera
	{
		af_vec3 _position;
		af_vec3 _direction;
		af_vec3 _up;
	};
	struct directional_light
	{
		af_vec3 _color;
		af_vec3 _direction;
	};
	struct point_light
	{
		af_vec3 _color;
		af_vec3 _position;
	};

	struct transformation
	{
		af_vec3 _scale;
		af_vec3 _rotation;
		af_vec3 _translation;
		bool _same_scale;
	};

	struct projection
	{
		float _fovy;
		//float _aspect;
		float _near;
		float _far;
	};
	struct point_pair
	{
		af_vec2 _point0;
		af_vec2 _point1;
	};
/*	struct ft_vertex
	{

		af_vec2  pos;
		af_vec2  uv;
		unsigned int col;
	};
	
	struct af_area
	{
		virtual bool contains(float posx, float posy)
		{
			return false;
		}
	};
	struct af_rect_area:public af_area 
	{
		af_vec2 _min, _max;
		af_rect_area(af_vec2& min, af_vec2& max)
			:_min(min), _max(max){}
		bool contains(float posx, float posy)
		{
			return posx>=_min.x &&posy >=_min.y&&posx<=_max.x&&posy<=_max.y;
		}
	};
	struct af_circle_area:public af_area
	{
		af_vec2 _center;
		float _radius;
		af_circle_area(af_vec2& center, float& radius)
			:_center(center), _radius(radius){}
		bool contains(float posx, float posy)
		{
			float xoff = posx - _center.x;
			float yoff = posy - _center.y;
			float dstsqt = xoff*xoff + yoff*yoff;
			float rdsqt = _radius*_radius;
			return dstsqt <= rdsqt;
		}
	};
	*/
#define FILE_NAME_LEN 50
}