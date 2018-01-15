#include "math.h"

namespace Math
{

	std::mt19937 RNG;
	std::uniform_int_distribution<> RNG_DIST8(0, 255);
	std::uniform_int_distribution<> RNG_DIST32;



	Vector2::Vector2(float scalarx, float scalary){
		x = scalarx;
		y = scalary;

	}

	Vector2::Vector2(){
		x = 0.0;
		y = 0.0;
	}

	Vector2::~Vector2(){

	}


	void Vector2::add(Vector2 vec){
		x += vec.x;
		y += vec.y;
	}

	void Vector2::add(float scalarX, float scalarY){
		x += scalarX;
		y += scalarX;
	}

	void Vector2::sub(Vector2 vec){
		x -= vec.x;
		y -= vec.y;
	}

	void Vector2::sub(float scalarX, float scalarY){
		x -= scalarX;
		y -= scalarY;
	}

	void Vector2::mul(Vector2 vec){
		x *= vec.x;
		y *= vec.y;

	}
	void Vector2::mul(float scalar){
		x *= scalar;
		y *= scalar;
	}
	void Vector2::mul(float scalarX, float scalarY){
		x *= scalarX;
		y *= scalarY;
	}

	float Vector2::dot(Vector2 vec){
		return x * vec.x + y * vec.y;
	}

	float Vector2::cross(Vector2 vec){
		return x * vec.y + y * vec.x;
	}

	void Vector2::rotate(float angle){

	}

	
	float Vector2::len(){
		return 0.0;
		//return sqrt(SQUARE(x)+SQUARE(y));
	}

	float Vector2::angle(){
		return 0.0;
		//return atan2(x,y);
	}



}