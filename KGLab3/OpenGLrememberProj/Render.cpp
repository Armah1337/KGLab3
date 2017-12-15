#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include <vector>
#include <array>
#include <cmath>


bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}

typedef std::vector < std::array<double, 3>> VtxVec;
void drawCube(VtxVec);


void drawCube(VtxVec vtx) { // ��������� ����, ������� ����� ������������ �� ������
	glBegin(GL_QUADS);
	glVertex3d(vtx[0][0], vtx[0][1], vtx[0][2]);
	glVertex3d(vtx[1][0], vtx[1][1], vtx[1][2]);
	glVertex3d(vtx[2][0], vtx[2][1], vtx[2][2]);
	glVertex3d(vtx[3][0], vtx[3][1], vtx[3][2]);

	glVertex3d(vtx[4][0], vtx[4][1], vtx[4][2]);
	glVertex3d(vtx[5][0], vtx[5][1], vtx[5][2]);
	glVertex3d(vtx[6][0], vtx[6][1], vtx[6][2]);
	glVertex3d(vtx[7][0], vtx[7][1], vtx[7][2]);

	glVertex3d(vtx[0][0], vtx[0][1], vtx[0][2]);
	glVertex3d(vtx[4][0], vtx[4][1], vtx[4][2]);
	glVertex3d(vtx[5][0], vtx[5][1], vtx[5][2]);
	glVertex3d(vtx[1][0], vtx[1][1], vtx[1][2]);

	glVertex3d(vtx[1][0], vtx[1][1], vtx[1][2]);
	glVertex3d(vtx[5][0], vtx[5][1], vtx[5][2]);
	glVertex3d(vtx[6][0], vtx[6][1], vtx[6][2]);
	glVertex3d(vtx[2][0], vtx[2][1], vtx[2][2]);

	glVertex3d(vtx[2][0], vtx[2][1], vtx[2][2]);
	glVertex3d(vtx[6][0], vtx[6][1], vtx[6][2]);
	glVertex3d(vtx[7][0], vtx[7][1], vtx[7][2]);
	glVertex3d(vtx[3][0], vtx[3][1], vtx[3][2]);

	glVertex3d(vtx[3][0], vtx[3][1], vtx[3][2]);
	glVertex3d(vtx[7][0], vtx[7][1], vtx[7][2]);
	glVertex3d(vtx[4][0], vtx[4][1], vtx[4][2]);
	glVertex3d(vtx[0][0], vtx[0][1], vtx[0][2]);
	glEnd();
}

double tOffset = 0, segments = 10, speed = 0.01; // tOffset = �������� ������ �� ������ ��� ������ ����������, segments = ���������� ��������� ������, speed = �������� ����������� �� ������
bool fwd = true; // ���� ����������� �����������

void bezierTrace(VtxVec vtx, double P1[], double P2[], double P3[], double P4[]) { // ����������� ���� �� ������ �����
	std::array<double, 3> centerVtx = { (vtx[0][0] + vtx[6][0]) / 2 , (vtx[0][1] + vtx[6][1]) / 2, (vtx[0][2] + vtx[6][2]) / 2 };
	glPushMatrix();
	double x = (1 - tOffset)*(1 - tOffset)*(1 - tOffset)*P1[0] + 3 * tOffset*(1 - tOffset)*(1 - tOffset)*P2[0] + 3 * tOffset*tOffset*(1 - tOffset)*P3[0] + tOffset*tOffset*tOffset*P4[0];
	double y = (1 - tOffset)*(1 - tOffset)*(1 - tOffset)*P1[1] + 3 * tOffset*(1 - tOffset)*(1 - tOffset)*P2[1] + 3 * tOffset*tOffset*(1 - tOffset)*P3[1] + tOffset*tOffset*tOffset*P4[1];
	double z = (1 - tOffset)*(1 - tOffset)*(1 - tOffset)*P1[2] + 3 * tOffset*(1 - tOffset)*(1 - tOffset)*P2[2] + 3 * tOffset*tOffset*(1 - tOffset)*P3[2] + tOffset*tOffset*tOffset*P4[2];
	double tOffsetNext;
	if (fwd) tOffsetNext = tOffset + speed / segments;
	else tOffsetNext = tOffset - speed / segments;
	double xNext = (1 - tOffsetNext)*(1 - tOffsetNext)*(1 - tOffsetNext)*P1[0] + 3 * tOffsetNext*(1 - tOffsetNext)*(1 - tOffsetNext)*P2[0] + 3 * tOffsetNext*tOffsetNext*(1 - tOffsetNext)*P3[0] + tOffsetNext*tOffsetNext*tOffsetNext*P4[0] - x;
	double yNext = (1 - tOffsetNext)*(1 - tOffsetNext)*(1 - tOffsetNext)*P1[1] + 3 * tOffsetNext*(1 - tOffsetNext)*(1 - tOffsetNext)*P2[1] + 3 * tOffsetNext*tOffsetNext*(1 - tOffsetNext)*P3[1] + tOffsetNext*tOffsetNext*tOffsetNext*P4[1] - y;
	double zNext = (1 - tOffsetNext)*(1 - tOffsetNext)*(1 - tOffsetNext)*P1[2] + 3 * tOffsetNext*(1 - tOffsetNext)*(1 - tOffsetNext)*P2[2] + 3 * tOffsetNext*tOffsetNext*(1 - tOffsetNext)*P3[2] + tOffsetNext*tOffsetNext*tOffsetNext*P4[2] - z;
	double deltaYZ = sqrt(pow(yNext, 2) + pow(zNext, 2));
	double deltaXY = sqrt(pow(xNext, 2) + pow(yNext, 2));
	double deltaXZ = sqrt(pow(xNext, 2) + pow(zNext, 2));
	double pitchAng = acos(yNext / deltaYZ) * 180 / PI;
	double yawAng = acos(xNext / deltaXY) * 180 / PI;
	double rollAng = acos(xNext / deltaXZ) * 180 / PI;
	// pitch axis = (1,0,0)
	// yaw axis = (0,0,1)
	// roll axis = (0,1,0)
	//glTranslated(-centerVtx[0], -centerVtx[1], -centerVtx[2]);
	glTranslated(x, y, z);
	glRotated(pitchAng, 1, 0, 0);
	glRotated(yawAng, 0, 0, 1);
	glRotated(rollAng, 0, 1, 0);
	drawCube(vtx);
	if (tOffset >= 1) fwd = false;
	else if (tOffset <= 0) fwd = true;

	if (fwd) tOffset += speed / segments;
	else tOffset -= speed / segments;
	glPopMatrix();
}

void hermiteTrace(VtxVec vtx, double P1_E[], double P2_E[], double R1_0_E[], double R1_1_E[], double R2_0_E[], double R2_1_E[]) { // ����������� ���� �� ������ ������
	glPushMatrix();
	double x = P1_E[0] * (2 * tOffset*tOffset*tOffset - 3 * tOffset*tOffset + 1) + P2_E[0] * (-2 * tOffset*tOffset*tOffset + 3 * tOffset*tOffset) + (R1_1_E[0] - R1_0_E[0])*(tOffset*tOffset*tOffset - 2 * tOffset*tOffset + tOffset) + (R2_1_E[0] - R2_0_E[0])*(tOffset*tOffset*tOffset - tOffset*tOffset);
	double y = P1_E[1] * (2 * tOffset*tOffset*tOffset - 3 * tOffset*tOffset + 1) + P2_E[1] * (-2 * tOffset*tOffset*tOffset + 3 * tOffset*tOffset) + (R1_1_E[1] - R1_0_E[1])*(tOffset*tOffset*tOffset - 2 * tOffset*tOffset + tOffset) + (R2_1_E[1] - R2_0_E[1])*(tOffset*tOffset*tOffset - tOffset*tOffset);
	double z = P1_E[2] * (2 * tOffset*tOffset*tOffset - 3 * tOffset*tOffset + 1) + P2_E[2] * (-2 * tOffset*tOffset*tOffset + 3 * tOffset*tOffset) + (R1_1_E[2] - R1_0_E[2])*(tOffset*tOffset*tOffset - 2 * tOffset*tOffset + tOffset) + (R2_1_E[2] - R2_0_E[2])*(tOffset*tOffset*tOffset - tOffset*tOffset);
	double tOffsetNext;
	if (fwd) tOffsetNext = tOffset + speed / segments;
	else tOffsetNext = tOffset - speed / segments;
	double xNext = P1_E[0] * (2 * tOffsetNext*tOffsetNext*tOffsetNext - 3 * tOffsetNext*tOffsetNext + 1) + P2_E[0] * (-2 * tOffsetNext*tOffsetNext*tOffsetNext + 3 * tOffsetNext*tOffsetNext) + (R1_1_E[0] - R1_0_E[0])*(tOffsetNext*tOffsetNext*tOffsetNext - 2 * tOffsetNext*tOffsetNext + tOffsetNext) + (R2_1_E[0] - R2_0_E[0])*(tOffsetNext*tOffsetNext*tOffsetNext - tOffsetNext*tOffsetNext) - x;
	double yNext = P1_E[1] * (2 * tOffsetNext*tOffsetNext*tOffsetNext - 3 * tOffsetNext*tOffsetNext + 1) + P2_E[1] * (-2 * tOffsetNext*tOffsetNext*tOffsetNext + 3 * tOffsetNext*tOffsetNext) + (R1_1_E[1] - R1_0_E[1])*(tOffsetNext*tOffsetNext*tOffsetNext - 2 * tOffsetNext*tOffsetNext + tOffsetNext) + (R2_1_E[1] - R2_0_E[1])*(tOffsetNext*tOffsetNext*tOffsetNext - tOffsetNext*tOffsetNext) - y;
	double zNext = P1_E[2] * (2 * tOffsetNext*tOffsetNext*tOffsetNext - 3 * tOffsetNext*tOffsetNext + 1) + P2_E[2] * (-2 * tOffsetNext*tOffsetNext*tOffsetNext + 3 * tOffsetNext*tOffsetNext) + (R1_1_E[2] - R1_0_E[2])*(tOffsetNext*tOffsetNext*tOffsetNext - 2 * tOffsetNext*tOffsetNext + tOffsetNext) + (R2_1_E[2] - R2_0_E[2])*(tOffsetNext*tOffsetNext*tOffsetNext - tOffsetNext*tOffsetNext) - z;
	double deltaYZ = sqrt(pow(yNext, 2) + pow(zNext, 2));
	double deltaXY = sqrt(pow(xNext, 2) + pow(yNext, 2));
	double deltaXZ = sqrt(pow(xNext, 2) + pow(zNext, 2));
	double pitchAng = acos(yNext / deltaYZ) * 180 / PI;
	double yawAng = acos(xNext / deltaXY) * 180 / PI;
	double rollAng = acos(xNext / deltaXZ) * 180 / PI;
	// pitch axis = (1,0,0)
	// yaw axis = (0,0,1)
	// roll axis = (0,1,0)
	glTranslated(x, y, z);
	glRotated(pitchAng, 1, 0, 0);
	glRotated(yawAng, 0, 0, 1);
	glRotated(rollAng, 0, 1, 0);
	drawCube(vtx);
	if (tOffset >= 1) fwd = false;
	else if (tOffset <= 0) fwd = true;

	if (fwd) tOffset += speed / segments;
	else tOffset -= speed / segments;
	glPopMatrix();
}

void Render(OpenGL *ogl)
{       	
	
	
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);\
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

    //���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	double B1Offset = -10, B2Offset = 10, E1Offset = -5, E2Offset = 0; // ��������� ������ ����� � ������
	double P1[] = {0 + B1Offset,0,0}, P2[] = {3 + B1Offset,4,5}, P3[] = {2 + B1Offset,5,7}, P4[] = {1 + B1Offset,7,4};
	double P5[] = {1 + B2Offset,2,3}, P6[] = {5 + B2Offset,4,1}, P7[] = {9 + B2Offset,7,5}, P8[] = {3 + B2Offset,10,11};

	glBegin(GL_LINE_STRIP);
	glColor3d(0,0,1); // blue
	for (double t = 0; t <= 1; t += 1.0/segments){
	double x = (1-t)*(1-t)*(1-t)*P1[0] + 3*t*(1-t)*(1-t)*P2[0] + 3*t*t*(1-t)*P3[0] + t*t*t*P4[0];
	double y = (1-t)*(1-t)*(1-t)*P1[1] + 3*t*(1-t)*(1-t)*P2[1] + 3*t*t*(1-t)*P3[1] + t*t*t*P4[1];
	double z = (1-t)*(1-t)*(1-t)*P1[2] + 3*t*(1-t)*(1-t)*P2[2] + 3*t*t*(1-t)*P3[2] + t*t*t*P4[2];
	glVertex3d(x,y,z);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	glColor3d(0,0,0);
	glVertex3d(P1[0],P1[1],P1[2]);
	glVertex3d(P2[0],P2[1],P2[2]);
	glVertex3d(P3[0],P3[1],P3[2]);
	glVertex3d(P4[0],P4[1],P4[2]);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3d(P5[0],P5[1],P5[2]);
	glVertex3d(P6[0],P6[1],P6[2]);
	glVertex3d(P7[0],P7[1],P7[2]);
	glVertex3d(P8[0],P8[1],P8[2]);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glColor3d(1,1,0); // yellow
	for (double t = 0; t <= 1; t += 1.0/segments){
	double x = (1-t)*(1-t)*(1-t)*P5[0] + 3*t*(1-t)*(1-t)*P6[0] + 3*t*t*(1-t)*P7[0] + t*t*t*P8[0];
	double y = (1-t)*(1-t)*(1-t)*P5[1] + 3*t*(1-t)*(1-t)*P6[1] + 3*t*t*(1-t)*P7[1] + t*t*t*P8[1];
	double z = (1-t)*(1-t)*(1-t)*P5[2] + 3*t*(1-t)*(1-t)*P6[2] + 3*t*t*(1-t)*P7[2] + t*t*t*P8[2];
	glVertex3d(x,y,z);
	}
	glEnd();

	double P1_E[] = {1 + E1Offset,2,3}, P2_E[] = {5 + E1Offset,4,1}, R1_0_E[] = {1 + E1Offset,2,3}, R1_1_E[] = {3 + E1Offset,10,11}, R2_0_E[] = {5 + E1Offset,4,1}, R2_1_E[] = {0 + E1Offset,0,0};
	glBegin(GL_LINE_STRIP);
	glColor3d(1,0,0); // red
	for (double t = 0; t <= 1; t += 1.0/segments){
	double x = P1_E[0]*(2*t*t*t - 3*t*t + 1) + P2_E[0]*(-2*t*t*t + 3*t*t) + (R1_1_E[0] - R1_0_E[0])*(t*t*t - 2*t*t + t) + (R2_1_E[0] - R2_0_E[0])*(t*t*t - t*t);
	double y = P1_E[1]*(2*t*t*t - 3*t*t + 1) + P2_E[1]*(-2*t*t*t + 3*t*t) + (R1_1_E[1] - R1_0_E[1])*(t*t*t - 2*t*t + t) + (R2_1_E[1] - R2_0_E[1])*(t*t*t - t*t);
	double z = P1_E[2]*(2*t*t*t - 3*t*t + 1) + P2_E[2]*(-2*t*t*t + 3*t*t) + (R1_1_E[2] - R1_0_E[2])*(t*t*t - 2*t*t + t) + (R2_1_E[2] - R2_0_E[2])*(t*t*t - t*t);
	glVertex3d(x,y,z);
	}
	glEnd();
	double P3_E[] = {5 + E2Offset,7,4}, P4_E[] = {8 + E2Offset,1,4}, R3_0_E[] = {5 + E2Offset,7,4}, R3_1_E[] = {7 + E2Offset,3,2}, R4_0_E[] = {8 + E2Offset,1,4}, R4_1_E[] = {1 + E2Offset,3,8};
	glBegin(GL_LINE_STRIP);
	glColor3d(0,1,0); // green
	for (double t = 0; t <= 1; t += 1.0/segments){
	double x = P3_E[0]*(2*t*t*t - 3*t*t + 1) + P4_E[0]*(-2*t*t*t + 3*t*t) + (R3_1_E[0] - R3_0_E[0])*(t*t*t - 2*t*t + t) + (R4_1_E[0] - R4_0_E[0])*(t*t*t - t*t);
	double y = P3_E[1]*(2*t*t*t - 3*t*t + 1) + P4_E[1]*(-2*t*t*t + 3*t*t) + (R3_1_E[1] - R3_0_E[1])*(t*t*t - 2*t*t + t) + (R4_1_E[1] - R4_0_E[1])*(t*t*t - t*t);
	double z = P3_E[2]*(2*t*t*t - 3*t*t + 1) + P4_E[2]*(-2*t*t*t + 3*t*t) + (R3_1_E[2] - R3_0_E[2])*(t*t*t - 2*t*t + t) + (R4_1_E[2] - R4_0_E[2])*(t*t*t - t*t);
	glVertex3d(x,y,z);
	}
	glEnd();
	glBegin(GL_LINES);
	glColor3d(0,0,0);
	glVertex3d(R1_0_E[0],R1_0_E[1],R1_0_E[2]);
	glVertex3d(R1_1_E[0],R1_1_E[1],R1_1_E[2]);
	glVertex3d(R2_0_E[0],R2_0_E[1],R2_0_E[2]);
	glVertex3d(R2_1_E[0],R2_1_E[1],R2_1_E[2]);
	glVertex3d(R3_0_E[0],R3_0_E[1],R3_0_E[2]);
	glVertex3d(R3_1_E[0],R3_1_E[1],R3_1_E[2]);
	glVertex3d(R4_0_E[0],R4_0_E[1],R4_0_E[2]);
	glVertex3d(R4_1_E[0],R4_1_E[1],R4_1_E[2]);
	glEnd();
	glColor3d(0, 0, 0);
	/*double vtx[8][3] = {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
	{0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};*/
	VtxVec vtx = { {0 - 0.5,0 - 0.5,0 - 0.5 },{1 - 0.5,0 - 0.5,0 - 0.5 },{1 - 0.5,1 - 0.5,0 - 0.5 },{0 - 0.5,1 - 0.5,0 - 0.5 },{0 - 0.5,0 - 0.5,1 - 0.5 },{1 - 0.5,0 - 0.5,1 - 0.5 },{1 - 0.5,1 - 0.5,1 - 0.5 },{0 - 0.5,1 - 0.5,1 - 0.5 } };
	glColor3d(0, 0, 1);																																	// ���������� ���� � ����������� ��� ������������ ������� �� 4 ������
	bezierTrace(vtx, P1, P2, P3, P4);
	glColor3d(1, 1, 0);
	bezierTrace(vtx, P5, P6, P7, P8);
	glColor3d(1, 0, 0);
	hermiteTrace(vtx, P1_E, P2_E, R1_0_E, R1_1_E, R2_0_E, R2_1_E);
	glColor3d(0, 1, 0);
	hermiteTrace(vtx, P3_E, P4_E, R3_0_E, R3_1_E, R4_0_E, R4_1_E);
	/*glPushMatrix();
	glRotated(rot, 1, 0, 0);
	glTranslated(-centerVtx[0], -centerVtx[1], -centerVtx[2]);
	glTranslated(5, 0, 0);
	rot++;
	drawCube(vtx);
	glPopMatrix();*/

	/*//������ ��������� ���������� ��������
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������
    
	
	//����� ��������� ������ �����, ���� ������� - ���������������, ��� �������� =)
	char c[250];  //������������ ����� ���������
	sprintf_s(c, "(T)�������� - %d\n(L)���� - %d\n\n���������� ������:\n"
		"G - ����������� � �������������� ���������,\nG+���+����������� �� ������������ �����\n"
		"R - ���������� ������ � ���� � ��������� ���������\n"
		"F - ����������� ���� � ����� ������", textureMode, lightMode);
	ogl->message = std::string(c);




}   //����� ���� �������

