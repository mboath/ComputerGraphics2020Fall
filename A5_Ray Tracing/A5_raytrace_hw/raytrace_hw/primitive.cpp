#include"primitive.h"
#include<sstream>
#include<cstdio>
#include<string>
#include<cmath>
#include<iostream>
#include<cstdlib>
#define ran() ( double( rand() % 32768 ) / 32768 )

const int BEZIER_MAX_DEGREE = 5;
const int Combination[BEZIER_MAX_DEGREE + 1][BEZIER_MAX_DEGREE + 1] =
{	0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0,
	1, 2, 1, 0, 0, 0,
	1, 3, 3, 1, 0, 0,
	1, 4, 6, 4, 1, 0,
	1, 5, 10,10,5, 1
};

const int MAX_COLLIDE_TIMES = 10;
const int MAX_COLLIDE_RANDS = 10;


std::pair<double, double> ExpBlur::GetXY()
{
	double x,y;
	x = ran();
	x = pow(2, x)-1;
	y = rand();
	return std::pair<double, double>(x*cos(y),x*sin(y));
}

Material::Material() {
	color = absor = Color();
	refl = refr = 0;
	diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
	blur = new ExpBlur();
}

void Material::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
	if ( var == "absor=" ) absor.Input( fin );
	if ( var == "refl=" ) fin >> refl;
	if ( var == "refr=" ) fin >> refr;
	if ( var == "diff=" ) fin >> diff;
	if ( var == "spec=" ) fin >> spec;
	if ( var == "drefl=" ) fin >> drefl;
	if ( var == "rindex=" ) fin >> rindex;
	if ( var == "texture=" ) {
		std::string file; fin >> file;
		texture = new Bmp;
		texture->Input( file );
	}
	if ( var == "blur=" ) {
		std::string blurname; fin >> blurname;
		if(blurname == "exp")
			blur = new ExpBlur();
	}
}

Primitive::Primitive() {
	sample = rand();
	material = new Material;
	next = NULL;
}

Primitive::Primitive( const Primitive& primitive ) {
	*this = primitive;
	material = new Material;
	*material = *primitive.material;
}

Primitive::~Primitive() {
	delete material;
}

void Primitive::Input( std::string var , std::stringstream& fin ) {
	material->Input( var , fin );
}

Sphere::Sphere() : Primitive() {
	De = Vector3( 0 , 0 , 1 );
	Dc = Vector3( 0 , 1 , 0 );
}

void Sphere::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "De=" ) De.Input( fin );
	if ( var == "Dc=" ) Dc.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Sphere::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 P = ray_O - O;
	double b = -P.Dot( ray_V );
	double det = b * b - P.Module2() + R * R;
	CollidePrimitive ret;

	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = b - det  , x2 = b + det;

		if ( x2 < EPS ) return ret;
		if ( x1 > EPS ) {
			ret.dist = x1;
			ret.front = true;
		} else {
			ret.dist = x2;
			ret.front = false;
		} 
	} else 
		return ret;

	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.C - O ).GetUnitVector();
	if ( ret.front == false ) ret.N = -ret.N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Sphere::GetTexture(Vector3 crash_C) {
	Vector3 I = ( crash_C - O ).GetUnitVector();
	double a = acos( -I.Dot( De ) );
	double b = acos( std::min( std::max( I.Dot( Dc ) / sin( a ) , -1.0 ) , 1.0 ) );
	double u = a / PI , v = b / 2 / PI;
	if ( I.Dot( Dc * De ) < 0 ) v = 1 - v;
	return material->texture->GetSmoothColor( u , v );
}


void Plane::Input( std::string var , std::stringstream& fin ) {
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Plane::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	CollidePrimitive ret;
	if ( fabs( d ) < EPS ) return ret;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return ret;

	ret.dist = l;
	ret.front = ( d < 0 );
	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.front ) ? N : -N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Plane::GetTexture(Vector3 crash_C) {
	double u = crash_C.Dot( Dx ) / Dx.Module2();
	double v = crash_C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

void Square::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Square::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 N = Dx * Dy;
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	CollidePrimitive ret;
	if ( fabs( d ) < EPS ) return ret;
	double l = ( O - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return ret;

	double dist = l;
	Vector3 C = ray_O + ray_V * dist;
	Vector3 vertices[5] = {O - Dx + Dy, O - Dx - Dy, O + Dx - Dy, O + Dx + Dy, O - Dx + Dy};
	double addAngles = 0;
	for(int i = 0; i < 4; i ++)
	{
		Vector3 ray1 = vertices[i] - C;
		ray1 = ray1.GetUnitVector();
		Vector3 ray2 = vertices[i+1] - C;
		ray2 = ray2.GetUnitVector();
		if((ray1 * ray2).Module() < EPS)
			return ret;
		else
		{
			double angle = acos(ray1.Dot(ray2));
			addAngles += (ray1 * ray2).GetUnitVector().Dot(N) * angle;
		}
	}

	if (addAngles < PI * 2 - EPS)
		return ret;
	else
	{
		ret.dist = dist;
		ret.front = ( d < 0 );
		ret.C = C;
		ret.N = ( ret.front ) ? N : -N;
		ret.isCollide = true;
		ret.collide_primitive = this;
		return ret;
	}
}

Color Square::GetTexture(Vector3 crash_C) {
	double u = (crash_C - O).Dot( Dx ) / Dx.Module2() / 2 + 0.5;
	double v = (crash_C - O).Dot( Dy ) / Dy.Module2() / 2 + 0.5;
	return material->texture->GetSmoothColor( u , v );
}

void Cylinder::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "R=" ) fin>>R; 
	Primitive::Input( var , fin );
}

CollidePrimitive Cylinder::Collide( Vector3 ray_O , Vector3 ray_V ) {
	Vector3 N2 = O2 - O1;
	N2 = N2.GetUnitVector();
	Vector3 N1 = O1 - O2;
	N1 = N1.GetUnitVector();
	Vector3 a = ray_V * N2;
	Vector3 b = (ray_O - O1) * N2;
	double det = a.Dot(b) * a.Dot(b) - a.Module2() * (b.Module2() - R * R);
	CollidePrimitive ret;

	double dist;
	bool front;
	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = (- a.Dot(b) - det) / a.Module2(), x2 = (- a.Dot(b) + det) / a.Module2();

		if ( x2 < EPS ) return ret;
		if ( x1 > EPS ) {
			dist = x1;
			front = true;	
		} else {
			dist = x2;
			front = false;
		} 
	} else
		return ret;

	Vector3 C = ray_O + ray_V * dist;
	double h = (C - O1).Dot(N2);
	Vector3 N = (C - O1 - h * N2).GetUnitVector();
	if ( h < EPS )
	{
		double d1 = N1.Dot( ray_V );
		if ( fabs( d1 ) < EPS ) return ret;
		double l1 = ( O1 - ray_O ).Dot( N1 ) / d1;
		if ( l1 < EPS ) return ret;
		dist = l1;
		C = ray_O + ray_V * dist;
		N = N1;
		if ( C.Distance(O1) > R - EPS ) return ret;
	}
	if ( h > (O2 - O1).Module() - EPS )
	{
		double d2 = N2.Dot( ray_V );
		if ( fabs( d2 ) < EPS ) return ret;
		double l2 = ( O2 - ray_O ).Dot( N2 ) / d2;
		if ( l2 < EPS ) return ret;
		dist = l2;
		C = ray_O + ray_V * dist;
		N = N2;
		if ( C.Distance(O2) > R - EPS ) return ret;
	}

	ret.dist = dist;
	ret.front = front;
	ret.C = C;
	ret.N = N;
	if ( front == false ) ret.N = -ret.N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Cylinder::GetTexture(Vector3 crash_C) {
	double u = 0.5 ,v = 0.5;
	Vector3 N2 = O2 - O1;
	N2 = N2.GetUnitVector();
	u = (crash_C - O1).Dot(N2);
	Vector3 vertical = N2.GetAnVerticalVector();
	v = acos((crash_C - O1 - u * N2).GetUnitVector().Dot(vertical)) / PI / 2;
	if (vertical.Dot((crash_C - O1) * N2) < 0) v = 1 - v;
	u = u / (O2 - O1).Module();
	return material->texture->GetSmoothColor( u , v );
}

void Bezier::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "P=" ) {
		degree++;
		double newR, newZ;
		fin>>newZ>>newR;
		R.push_back(newR);
		Z.push_back(newZ);
	}
	if ( var == "Cylinder" ) {
		double maxR = 0;
		for(int i=0;i<R.size();i++)
			if(R[i] > maxR)
				maxR = R[i];
		boundingCylinder = new Cylinder(O1, O2, maxR);
		N = (O1 - O2).GetUnitVector();
		Nx = N.GetAnVerticalVector();
		Ny = N * Nx;
	}
	Primitive::Input( var , fin );
}

CollidePrimitive Bezier::Collide( Vector3 ray_O , Vector3 ray_V ) {
	CollidePrimitive ret;
	//NEED TO IMPLEMENT
	return ret;
}

Color Bezier::GetTexture(Vector3 crash_C) {
	double u = 0.5 ,v = 0.5;
	//NEED TO IMPLEMENT
	return material->texture->GetSmoothColor( u , v );
}

