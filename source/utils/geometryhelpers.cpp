/*These files provide helper functions for the Asteroids game.
*/

/*ToDo: Create Random points, polygons.
 *		Shatter polygons (asteroids).
 *      Delauny Triangluation.
 *      Triangluation of polygons
 *
 * */
#include "common.h"
mat2 rotation(float angle){ //Returns a rotation matrix by a given angle. The angle is in radians.
  mat2 toReturn(vec2(cos(angle),-sin(angle)),vec2(sin(angle), cos(angle)));
  return toReturn;
}


//typedef float (*curve)(float);
float* findParabola(vec2 pt0, vec2 pt1, vec2 pt2){
	//returns the coefficients of a parabola which passes through these 3 points.
	float a;
	float b;
	float c;
	
	a = ((pt0.y-pt1.y)*(pt1.x-pt2.x)-(pt1.y-pt2.y)*(pt0.x-pt1.x)) / ((pow(pt0.x,2)-pow(pt1.x,2))*(pt1.x-pt2.x)-(pow(pt1.x,2)-pow(pt2.x,2))*(pt0.x-pt1.x));
	b = ((pt0.y-pt1.y)*(pow(pt1.x,2)-pow(pt2.x,2)) - (pt1.y-pt2.y)*(pow(pt0.x,2)-pow(pt1.x,2))) / ((pt0.x-pt1.x)*(pow(pt1.x,2)-pow(pt2.x,2)) - (pt1.x-pt2.x)*(pow(pt0.x,2)-pow(pt1.x,2)));
	c = pt0.y - (a*pow(pt0.x,2)+b*pt0.x);
	float* toReturn = new float[3];
	toReturn[0]=a;
	toReturn[1]=b;
	toReturn[2]=c;
    return(toReturn);
	}

vec2* drawCurve(float start, float end, std::function<float(float)> cur, int num_pts){
	//Draws the curve between the 'start' and 'end'
	vec2 *resultingPoints = new vec2[num_pts];
	float divisor = float(num_pts -1);
	for (int i=0; i<num_pts; i++){
		resultingPoints[i] = vec2(end*i/divisor+(1.0-(i/divisor))*start, cur(end*i/divisor+(1.0-(i/divisor))*start));
		}
	return resultingPoints;
	}

bool isInsidePolygon(vec2 pt, vec2 *polygon, int l){//l is the length of the polygon
	//returns True if the point is inside the polygon.
	//implements the ray casting algorithm https://en.wikipedia.org/wiki/Point_in_polygon
	vec2 p_pt; //previous point
	vec2 c_pt; //current point
	int counter = 0; //counts the number of intersections of the polygon with the ray going to the right.
	float x; //The value of the intersect.
	for (int i=0; i<l; i++){//loop through the edges of the polygon
		if (i==0){
			p_pt = polygon[l-1];
			c_pt = polygon[i];
		}
		else{
			p_pt = polygon[i-1];
			c_pt = polygon[i];
			}
		if((p_pt.y <= pt.y and c_pt.y >= pt.y) or (p_pt.y >= pt.y and c_pt.y <= pt.y)){ //the pt.y needs to be between c_pt.y and p_pt.y
		  //std::cout<<"inside "<<std::endl;
		  x = c_pt.x+( (c_pt.x-p_pt.x)*(pt.y - c_pt.y)/ (c_pt.y-p_pt.y) );
		  if (x > pt.x) {
			counter +=1;
			}
		}
	}
	if (counter %2 == 1){
		return true;
		}
	else{
		return false;
		}
}

bool polygonsIntersect(vec2 *polygon1, vec2 *polygon2, int l1, int l2){
	for (int i=0; i<l2; i++){
		if (isInsidePolygon(polygon2[i], polygon1, l1))
			return true;
		}
	return false;
	}

//Initialize the gl state and variables
vec2 *smoothCorners(vec2 *polygon, int l){//Will smooth out the corners of the polygon, creating a new polygon.
  vec2 p0; //These are the points which form the corner.
  vec2 p1;
  vec2 p2;
  vec2 direction;
  vec2 center;
  float t;
  float theta; //half the angle between the two vectors.
  float magnitude;//The distance to the center of the circle, wedged in the angle.
  float center_angle;
  float start_angle;
  float end_angle;
  float temp;
  vec2 angle;//The angle, in degrees
  int n = 10; //Number of points to use on each corner.
  vec2 *smooth_shape = new vec2[n*l];
  for (int i=0; i<l; i++){ //loop through the original polygon
	
	if (i == 0){
		p0 = polygon[l - 1];
		p1 = polygon[0];
		p2 = polygon[1];
	}
	else if(i == l - 1){
		p0 = polygon[l - 2];
		p1 = polygon[l - 1];
		p2 = polygon[0];
	}
	else{
		p0 = polygon[i-1];
		p1 = polygon[i];
		p2 = polygon[i+1];
		}
      theta = 0.5*acos(dot(p0-p1, p2-p1)/length(p0-p1)/length(p2-p1));
	  
	  float r = 0.6/7.0*(pow(sin(theta),2.0));//adaptive radius. You gotta play around with this function.
	  //float r = 0.3/7.0;
	  direction = normalize(normalize(p2-p1)+normalize(p0-p1));
	  
	  magnitude = r/sin(theta);
	  center = p1+magnitude*direction;
	  center_angle = asin( direction.y); //will be the normalized vector from the center to p1.
	  if (direction.x < 0){
         center_angle = pi - center_angle;
		  }
	  
	  start_angle = pi+(center_angle +(pi/2.0 - theta));
	  end_angle   = pi+(center_angle -(pi/2.0 - theta));
	  if (isInsidePolygon((99.0*p1+direction)/100.0, polygon, l)){//reverse the circle orientation if outside the polygon.
		  temp = start_angle;
		  start_angle = end_angle;
		  end_angle = temp;
		  }
	  for (int j=0; j<n; j++){
		  t =start_angle*j/float(n-1) + end_angle*(1.0-j/float(n-1));
		  angle = rotation(t)*vec2(1.0,0.0);
		  smooth_shape[n*i+j]= r*angle + center;
		  }
	  
	  }
	  return smooth_shape;
	}
	