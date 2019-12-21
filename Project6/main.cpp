#include "Angel.h"
#include "mat.h"
#include "include/TriMesh.h"
#include <assert.h>
#include<vector>
#include<fstream>
#include<string>
#include<ctime>
#include<cstdlib>
#include "FreeImage.h"
#include<stack>
using namespace std;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")

//////////////////////////////////////////////////////////////////
vec4 lightPos;
GLuint lightPosID;
GLuint MatrixID;
GLuint vPositionID;
GLuint MatrixID2;
GLuint programID;
GLuint isCubeID;
struct CUBE {
	GLuint cubeVBO[6];
	GLuint cubeIBO[6];
	GLuint cubeUVBO[6];
	GLuint cubeTexBO[6];
	GLuint cubeNormal[6];
} SKY_CUBE;


struct ImgURL {
	string front_img;
	string top_img;
	string bottom_img;
	string right_img;
	string left_img;
	string back_img;
};
//���ն���
struct {
	GLuint Color;//��ɫ
	GLuint AmbientIntensity;//������ǿ��
	GLuint Direction;//����
	GLuint DiffuseIntensity;//������ǿ��
	GLuint lightPos;// ��Դ����������ϵ�µ�λ�á�
} m_dirLightLocation;


/////////////////////////////////////////////////////////////
// ��պ�
enum { FRONT, TOP, BOTTOM, RIGHT, LEFT, BACK };
struct SKY {
	GLuint SkyVBO[6];
	GLuint SkyIBO[6];
	GLuint SkyUVBO[6];
	GLuint SkyTexBO[6];
} sky;

GLfloat Boxheight = 2.0;
///////
TriMesh* mesh = new TriMesh();


//////////////////////////////////////////////////////////////////////////////////
//   ����
vector<vec3> ground_points;
vector<vec3> POINTS1;
vector<vec3i> INDEXS;
GLuint GROUND_IBO;
GLuint GROUND_VBO;
GLuint GROUND_UVBO;
vector<vec2> GROUND_UV;
int TEXTURE2 = 0;

GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint projMatrixID;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.1;
const double DEFAULT_DELTA = 0.3;

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

bool canCout = true;
vec3 scaleTheta(1.0, 1.0, 1.0);
vec3 rotateTheta(0.0, 0.0, 0.0);
vec3 translateTheta(0.0, 0.0, 0.0);

int currentTransform = TRANSFORM_ROTATE;
int currentDirection = 1;
int currentAxis = Y_AXIS;

int mainWindow;
int frame = 0;
int windowWidth = 1000;
int windowHeight = 800;

////////////////////////////////////////////////////////////////

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;


	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		mat4 c;
		c[0][0] = 2.0 / (right - left);
		c[1][1] = 2.0 / (top - bottom);
		c[2][2] = 2.0 / (zNear - zFar);
		c[3][3] = 1.0;
		c[0][3] = -(right + left) / (right - left);
		c[1][3] = -(top + bottom) / (top - bottom);
		c[2][3] = -(zFar + zNear) / (zFar - zNear);
		return c;
	}

	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		GLfloat const tanHalfFovy = tan(fovy / 2.0);
		mat4 Result(0);
		Result[0][0] = 1.0 / (aspect * tanHalfFovy);
		Result[1][1] = 1.0 / (tanHalfFovy);
		Result[2][3] = -1.0;
		Result[2][2] = -(zFar + zNear) / (zFar - zNear);
		Result[3][2] = -(2.0 * zFar * zNear) / (zFar - zNear);

		mat4 result(0);
		for (int i = 0;i != 4;i++) {
			for (int j = 0;j != 4;j++) {
				result[i][j] = Result[j][i];
			}
		}

		//return result;
		return Result;
	}

	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec4 n = normalize(eye - at);
		vec4 u = normalize(vec4(cross(up, n), 0.0));
		vec4 v = normalize(vec4(cross(n, u), 0.0));
		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 x = mat4(u, v, n, t) * Translate(-eye);
		mat4 result(0);
		for (int i = 0;i != 4;i++) {
			for (int j = 0;j != 4;j++) {
				result[i][j] = x[j][i];
			}
		}


		return result;
		return x;
	}

}


///////////////////////////////////////////////////////////////////////////
// ����
vector<vec3> NORMALS;
float verticeScale = 0.2f;
float yScale = 10.0f;
int imgWidth;
int imgHeight;
static  GLfloat *vertices;
static unsigned int *indices;
static  GLfloat *uvs;
static GLfloat *normals;
GLuint VBO;//���㻺�����

GLuint TextureID;
GLuint TextureID2;
GLuint vertexUVID;
GLuint TexBO;//��ͼ����
GLuint TexBO2;//��ͼ����
GLuint UVBO;//uv�������
GLuint NormalBO;//���߶���
GLuint NormalID;
//////////////////////////////////////////////////////////////////////////////////////////
// �������
float horizontalAngle = 0.500003;
float verticalAngle = -0.554995;
vec3 position;  //vec3(16.4819, 11.0353, 13.4931); //���λ��
vec3 direction(0.407467, -0.526939, 0.745859);
//vec3 direction(00,0,0);
vec3 right0(0, 0, 0);
vec3 up(0, 1, 0);
vec3 translate(0, 0, 0);
float wheelSpeed = 0.1f;
float initialFoV = 45.0f; //����ӳ���
float speed = 0.05f; //ƽ���ٶ� 
float mouseSpeed = 0.05f;
int mouseX= 429, mouseY= 96;//���λ�� ��������
bool mouseLeftDown = false;//����������


/////////////////////////////////////////////////////
// ����
GLuint GVBO;
GLuint GColor;
GLuint GIBO;
vector<vec3i> Gindex;

vector<vec4> points;
vector<vec4> colors;
int nVertices, nFaces, nLine;
// ����ĸ�����
vector<vec4> vertices1;
// ÿ������Ķ�Ӧ��ɫ
vector<vec4> vertexColors;
GLuint cube;
GLuint colorID;


GLuint normal_VBO;
GLuint IBO;

/////////////////////////////////////////////////////////////////////////
// ������

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

class MatrixStack {
	int    _index;
	int    _size;
	mat4*  _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;

	}

	mat4& pop(void) {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};


const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

MatrixStack  mvstack;
mat4 model_view;
point4 cube_points[NumVertices];
color4 cube_colors[NumVertices];
vec3 cube_normals[NumVertices];
point4 cube_vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

vec4 black(0.0, 0.0, 0.0, 1.0);
vec4 red(1.0, 0.0, 0.0, 1.0);  // red
vec4 yellow(1.0, 1.0, 0.0, 1.0);  // yellow
vec4 green(0.0, 1.0, 0.0, 1.0); // green
vec4 blue(0.0, 0.0, 1.0, 1.0);  // blue
vec4 magenta(1.0, 0.0, 1.0, 1.0);  // magenta
vec4 white(1.0, 1.0, 1.0, 1.0);  // white
vec4 cyan(0.0, 1.0, 1.0, 1.0);   // cyan

point4 color_torso = point4(0, 0, 1, 1);

#define TORSO_HEIGHT 5.0		//����
#define TORSO_WIDTH 3.0

#define UPPER_ARM_HEIGHT 3.0	//�ϱ�
#define UPPER_ARM_WIDTH  0.5

#define LOWER_ARM_HEIGHT 2.0	//�±�
#define LOWER_ARM_WIDTH  0.3

#define UPPER_LEG_WIDTH  0.5	//����
#define UPPER_LEG_HEIGHT 3.0


#define LOWER_LEG_HEIGHT 2.0	//����
#define LOWER_LEG_WIDTH  0.3

#define HEAD_HEIGHT 1.0			//ͷ
#define HEAD_WIDTH 1.0

#define HAT_HEIGHT 1		// ñ��
#define HAT_WIDTH 1	

#define NECK_HEIGHT 0.2	//����
#define NECK_WIDTH	0.2

enum {
	Torso,
	Head1,
	Head2,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	RightUpperLeg,
	RightLowerLeg,
	LeftUpperLeg,
	LeftLowerLeg,
	NumJointAngles,
	Quit
};
GLfloat theta[NumJointAngles] = {
	0.0,    // Torso
	0.0,    // Head1
	0.0,    // Head2
	0.0,    // RightUpperArm
	0.0,    // RightLowerArm
	0.0,    // LeftUpperArm
	0.0,    // LeftLowerArm
	180.0,  // RightUpperLeg
	0.0,    // RightLowerLeg
	180.0,  // LeftUpperLeg
	0.0     // LeftLowerLeg
};
GLint angle = Head2;
int Index = 0;
GLuint buffer;

color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};


double random(int max);
void storeTrianglePoints(int a, int b, int c);
void initOFF(string filename);
static void SpecialKeyboard(int Key, int x, int y);
void mouseCB(int button, int state, int x, int y);
void mouseMotionCB(int x, int y);
FIBITMAP* getDib(string ptrFileName);
void initGround(string);
void displayGROUND();
void load_texture_FreeImage(std::string file_name, GLuint& m_texName);
void CreateTexture(string filename, GLuint &OBJ);
void init();
void Display();
void updateTheta(int axis, int sign);
void idle();
void keyboard(unsigned char key, int x, int y);
mat4 Transpose(mat4 matrix);
void mouseWheel(int, int, int, int);
void initSky();

///
void CreateBox(vector<vec3> cubePoints, ImgURL imgURL, CUBE &cube);
void displayBox(mat4 modelViewMatrix, CUBE& cube);
void Createcube(vector<vec3> cubeVertex, vector<vec3i> index, vector<vec2> uvIndex, GLuint face, string file, CUBE &cube);
void displaycube(GLuint face, mat4 m, CUBE &cube);

void drawCube(mat4 m);
void menu(int option);
void addMenuItem();
void displayRobot(mat4);
void initRobot();
void right_lower_leg(vec4 color_torso);
void right_upper_leg(vec4 color_torso);
void left_lower_leg(vec4 color_torso);
void left_upper_leg(vec4 color_torso);
void right_lower_arm(vec4 color_torso);
void right_upper_arm(vec4 color_torso);
void left_lower_arm(vec4 color_torso);
void left_upper_arm(vec4 color_torso);
void head(vec4 color_torso);
void torso(vec4 color_torso);
void colorcube(void);
void quad(int a, int b, int c, int d);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // ����֧��˫�ػ������Ȳ���
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowWidth, windowHeight);

	mainWindow = glutCreateWindow("2017151024");
	srand(time(0));
	glewExperimental = GL_TRUE;
	glewInit();
	glutDisplayFunc(Display);

	init();

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeyboard);
	
	glutMouseWheelFunc(mouseWheel);
	glutMouseFunc(mouseCB);

	glutMotionFunc(mouseMotionCB);
	glutCreateMenu(menu);
	addMenuItem();
	cout << "init over" << endl;

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}


//���������
double random(int max) {
	return 1.0*(rand() % (max + 1));
}
// �洢ÿ�����ǵĸ������㲢Ϊ�㸳ɫ
void storeTrianglePoints(int a, int b, int c)
{

	points.push_back(vertices1[a]);
	points.push_back(vertices1[b]);
	points.push_back(vertices1[c]);

	colors.push_back(vertexColors[a]);
	colors.push_back(vertexColors[b]);
	colors.push_back(vertexColors[c]);
}


void initOFF(string filename)
{

	// �洢ÿ�������εĶ���λ�ú���ɫ��Ϣ��
	vertices1.clear();
	vertexColors.clear();
	points.clear();
	colors.clear();
	fstream input(filename);
	string off;
	input >> off;
	input >> nVertices >> nFaces >> nLine;
	for (int i = 0;i != nVertices;i++) {
		double x, y, z;
		input >> x >> y >> z;
		vertices1.push_back(vec4(x, y, z, 1));
	}
	for (int i = 0;i != nVertices;i++) {
		vertexColors.push_back(
			vec4(random(nFaces) / nFaces,
				random(nFaces) / nFaces,
				random(nFaces) / nFaces,
				1.0
			)
		);
	}

	for (int i = 0;i != nFaces;i++) {
		int t;
		input >> t;
		double a, b, c;
		input >> a >> b >> c;
		storeTrianglePoints(a, b, c);
	}
	vector<vec3> nNormals;

	for (int i = 0;i != points.size();i++) {
		nNormals.push_back(vec3(points[i].x, points[i].y,points[i].z));
	}


	glGenBuffers(1, &cube);
	glBindBuffer(GL_ARRAY_BUFFER, cube);
	glBufferData(GL_ARRAY_BUFFER, (points.size() +  colors.size())* sizeof(vec4) + nNormals.size()*sizeof(vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (points.size() + colors.size())* sizeof(vec4), nNormals.size() * sizeof(vec3), &nNormals[0]);

}

static void SpecialKeyboard(int Key, int x, int y)
{
	return;
	 direction = vec3(
		-cos(verticalAngle*M_PI / 180.0) * sin(horizontalAngle*M_PI / 180.0),
		sin(verticalAngle*M_PI / 180.0),
		-cos(verticalAngle*M_PI / 180.0) * cos(horizontalAngle*M_PI / 180.0)
	);
	 right0 =  vec3(
		cos(horizontalAngle*M_PI/180.0),
		0,
		-sin(horizontalAngle*M_PI/180.0)
	);

	 direction = normalize(direction);
	 right0 = normalize(right0);

	switch (Key) {
	case GLUT_KEY_UP:
		position += direction  * speed;
		break;
	case GLUT_KEY_RIGHT:
		position += right0  * speed;
		break;
	case GLUT_KEY_DOWN:
		position -= direction  * speed;
		break;
	case GLUT_KEY_LEFT:
		position -= right0  * speed;
		break;
	}
	glutPostRedisplay();//���ô����ػ�
}

void mouseWheel(int button, int direction, int x, int y) {
	if (direction < 0) {
		initialFoV += 1.0*wheelSpeed;
	}
	else {
		initialFoV -= 1.0*wheelSpeed;
	}
}


//��������¼�
void mouseCB(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
			mouseX = x;
			mouseY = y;
		}
		else if (state == GLUT_UP)
		{
			mouseLeftDown = false;
		}
	}
}

//�������λ��
void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown == true)
	{
		horizontalAngle += mouseSpeed * float(x - mouseX);
		verticalAngle += mouseSpeed * float(y - mouseY);
		mouseX = x;
		mouseY = y;
		keyboard(0, 0, 0);
	}
}

FIBITMAP* getDib(string ptrFileName) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(ptrFileName.c_str());
	if (fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(ptrFileName.c_str());
	}
	if (fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif))
	{
		FIBITMAP* dib = FreeImage_Load(fif, ptrFileName.c_str(), 0);
		return dib;
	}
}

void load_texture_FreeImage(std::string file_name, GLuint& m_texName)
{

	FIBITMAP *dib = getDib(file_name.c_str());

	//3 ת��Ϊrgb 24ɫ;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 ��ȡ����ָ��
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);
	
	
	glBindTexture(GL_TEXTURE_2D, m_texName);
	glTexImage2D(
		GL_TEXTURE_2D, //! ָ���Ƕ�άͼƬ
		0, //! ָ��Ϊ��һ����,���������mipmap,��lod,����ľͲ��ü�����,Զ��ʹ�ý�С������
		GL_RGB, //! �����ʹ�õĴ洢��ʽ
		width, //! ���,��һ����Կ�,��֧�ֲ����������,����Ⱥ͸߶Ȳ���2^n��
		height, //! ���,��һ����Կ�,��֧�ֲ����������,����Ⱥ͸߶Ȳ���2^n��
		0, //! �Ƿ�ı�
		GL_BGR_EXT, //! ���ݵĸ�ʽ,bmp��,windows,����ϵͳ�д洢��������bgr��ʽ
		GL_UNSIGNED_BYTE, //! ������8bit����
		pixels
	);
	/**
	* �ͷ��ڴ�
	*/
	FreeImage_Unload(dib);
}

float getPix(BYTE *pixels, int i, int k) {
	return 1.0*(int(pixels[imgHeight*i + k]));
}

void initGround(string file)
{
	fstream in(file);
	in >> imgHeight >> imgWidth;

	//�������� UV���� ������
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			int pix;
			in >> pix;
			ground_points.push_back(vec3(verticeScale* 1.0*(i), yScale* 1.0*pix / 255.0f, verticeScale * 1.0*(j)));
			GROUND_UV.push_back(vec2(1.0*j / imgWidth, 1.0*(imgHeight - i) / imgHeight));
			NORMALS.push_back(vec3(verticeScale* 1.0*(i), yScale* 1.0*pix / 255.0f, verticeScale * 1.0*(j)));
		}
	}

	// ����

	for (int i = 0; i < imgHeight - 1; i++){
		for (int j = 0; j < imgWidth - 1; j++){

			INDEXS.push_back(vec3i(i*imgWidth + j,i*imgWidth + j + 1,(i + 1)*imgWidth + j));
			INDEXS.push_back(vec3i((i + 1)*imgWidth + j, i*imgWidth + j + 1, (i + 1)*imgWidth + j + 1));
		}
	}

	// ������
	
	vector<vec3> normals;
	for (int i = 0;i != ground_points.size();i++) {
		normals.push_back(vec3(0, 0, 0));
	}

	//������������
	for (size_t i = 0; i < INDEXS.size(); i++){
		vec3i pIndex = INDEXS[i];

		vec3 p1 = ground_points[pIndex.a];
		vec3 p2 = ground_points[pIndex.b];
		vec3 p3 = ground_points[pIndex.c];

		vec3 n1 = p1 - p2;
		vec3 n2 = p1 - p3;
		vec3 N = cross(n1, n2);

		float xN = N.x;
		float yN = N.y;
		float zN = N.z;
		//���㷨�߸���
		normals[pIndex.a][0] += xN;
		normals[pIndex.a][1] += yN;
		normals[pIndex.a][2] += zN;

		normals[pIndex.b][0] += xN;
		normals[pIndex.b][1] += yN;
		normals[pIndex.b][2] += zN;

		normals[pIndex.c][0] += xN;
		normals[pIndex.c][1] += yN;
		normals[pIndex.c][2] += zN;

	}


	// ����
	CreateTexture("./image/bottom.jpg", TexBO); //�ײ�
	CreateTexture("./image/t1.jpg", TexBO2);//�ϲ�

	glGenBuffers(1, &GROUND_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_VBO);
	glBufferData(GL_ARRAY_BUFFER, ground_points.size() * sizeof(vec3), &ground_points[0], GL_STATIC_DRAW);

	glGenBuffers(1, &GROUND_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GROUND_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEXS.size() * sizeof(vec3i), &INDEXS[0], GL_STATIC_DRAW);


	glGenBuffers(1, &GROUND_UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_UVBO);
	glBufferData(GL_ARRAY_BUFFER, GROUND_UV.size() * sizeof(vec2), &GROUND_UV[0], GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

	//��ʼ�����
	position = vec3(0.5*verticeScale*(imgWidth - 1), 0.5*yScale*Boxheight, 0.5*verticeScale*(imgHeight - 1));
	direction = vec3(-0.5*verticeScale*(imgWidth - 1), -0.5*yScale*Boxheight, -0.5*verticeScale*(imgHeight - 1));
	GLuint depthID = glGetUniformLocation(programID, "depth");
	glUniform1f(depthID, 1.0 * 140 / 255.0f);
}


//������ͼ
void CreateTexture(string filename, GLuint &OBJ)
{

	glGenTextures(1, &OBJ);
	
	//����FreeImage��������
	load_texture_FreeImage(filename.c_str(), OBJ);

	//�ظ�����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//�˲�
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
	glGenerateMipmap(GL_TEXTURE_2D);
}


void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	programID = InitShader("vshader.glsl", "fshader.glsl");
	MatrixID = glGetUniformLocation(programID, "matrix");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vertexUVID = glGetAttribLocation(programID, "vertexUV");
	NormalID = glGetAttribLocation(programID, "Normal");
	TextureID = glGetUniformLocation(programID, "myTexture");
	TextureID2 = glGetUniformLocation(programID, "myTexture2");
	MatrixID2 = glGetUniformLocation(programID, "gWorld");
	isCubeID = glGetUniformLocation(programID, "isCube");
	colorID = glGetAttribLocation(programID, "color");
	modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");

	m_dirLightLocation.Color = glGetUniformLocation(programID, "gDirectionalLight.Color");
	m_dirLightLocation.AmbientIntensity = glGetUniformLocation(programID, "gDirectionalLight.AmbientIntensity");
	m_dirLightLocation.Direction = glGetUniformLocation(programID, "gDirectionalLight.Direction");
	m_dirLightLocation.DiffuseIntensity = glGetUniformLocation(programID, "gDirectionalLight.DiffuseIntensity");
	m_dirLightLocation.lightPos = glGetUniformLocation(programID, "gDirectionalLight.lightPos");


	initGround("./data/data_2png.txt");
	initSky();
	initOFF("./off/sphere.off");
	initRobot();
}

void initSky() {
	GLfloat y = yScale * Boxheight;
	GLfloat z = verticeScale*1.0*(imgHeight - 1);
	GLfloat x = verticeScale*1.0*(imgWidth - 1);
	vector<vec3> v = {
		vec3(0,y,z),vec3(0,0,z),vec3(x,y,z),vec3(x,0,z),
		vec3(x,y,0),vec3(x,0,0),vec3(0,y,0),vec3(0,0,0)
	};

	ImgURL imgURL;	string BASC_URL = "./image/sky/";
	imgURL.back_img = BASC_URL+ "back.png";
	imgURL.bottom_img = BASC_URL+"bottom.png";
	imgURL.front_img = BASC_URL + "front.png";
	imgURL.left_img = BASC_URL + "left.png";
	imgURL.right_img = BASC_URL + "right.png";
	imgURL.top_img = BASC_URL + "top.png";
	CreateBox(v, imgURL, SKY_CUBE);
}


void displayGROUND() {

	GLuint testID = glGetUniformLocation(programID, "test");
	glUniform1f(testID, 1.0*((TEXTURE2) % imgWidth) / imgWidth);
	glUniform1i(isCubeID, 2);

	//���ݶ��㡢������UV  vec2
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_VBO); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 3, GL_FLOAT, GL_FALSE, 0, 0); //���߹�����������bufer�е�����  
	
	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_UVBO);
	glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(NormalID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBO);
	glVertexAttribPointer(NormalID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//������ͼ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexBO);
	glUniform1i(TextureID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexBO2);
	glUniform1i(TextureID2, 1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GROUND_IBO);

	//����
	glDrawElements(GL_TRIANGLES,INDEXS.size() * 3,GL_UNSIGNED_INT,(void*)0);
}

void Display() {
	// �����ɫ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(vPositionID); //������������
	float FoV = 1.0*initialFoV*M_PI / 180.0;
	Camera::projMatrix = Camera::perspective(FoV, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	Camera::viewMatrix = Camera::lookAt(
		vec4(position,1),
		vec4(position + direction,1),
		vec4(up,0)
	);
	Camera::modelMatrix = mat4(1);

	mat4 modelViewProjMatrix =  Camera::projMatrix*Camera::viewMatrix* Camera::modelMatrix;
	//���ݾ���
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, Camera::modelMatrix);
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, Camera::viewMatrix);
	glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, Camera::projMatrix);


	//���ݹ���	
	vec4 center(verticeScale*1.0*(imgWidth - 1)*0.5, yScale* Boxheight, verticeScale*1.0*(imgHeight - 1)*0.5, 1);
	lightPos =(center + translate);
	if(!canCout)
		cout << lightPos << endl;

	//�����Դλ��

	glUniform3f(lightPosID, lightPos.x, lightPos.y, lightPos.z);

	//��������
	glUniform3f(m_dirLightLocation.Color, 1, 1, 1);
	glUniform3f(m_dirLightLocation.Direction, 0, 0, 0);

	glUniform1f(m_dirLightLocation.AmbientIntensity, 0.2);//�����ⷴ��ϵ��
	glUniform1f(m_dirLightLocation.DiffuseIntensity, 0.2);//������ϵ��

	//���Ƶ���
	displayGROUND();

	//������պ�
	displayBox(mat4(1), SKY_CUBE);

	//���ƻ�����1

	vec3 cameraPos = position + direction;
	mat4 m1 = Translate(cameraPos.x, cameraPos.y, cameraPos.z)*Scale(.2, .2, .2);//*RotateY(rotateTheta[1]);//�Ƶ����
	displayRobot(m1);
	
	//���ƻ�����2

	mat4 m2 = Translate(0.5*verticeScale*(imgWidth - 1)-0.5,1, 0.5*verticeScale*(imgHeight - 1))*RotateY(rotateTheta[1]);
	displayRobot(m2);


	//����������
	
	//������1
	mat4 m = Translate(0.5*verticeScale*(imgWidth - 1), 0.5*yScale*Boxheight, 0.5*verticeScale*(imgHeight - 1))*RotateY(rotateTheta[1]);
	drawCube(m);
	//������2
	m *= Translate(position.x, position.y, position.z)*Scale(4,4,4);
	//drawCube(m);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutSwapBuffers();

}

void drawCube(mat4 m) {
	glUniform1i(isCubeID, 1);//����ȷ�������ǲ������������ɫ

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, m);
	glBindBuffer(GL_ARRAY_BUFFER, cube); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec4)));

	glEnableVertexAttribArray(NormalID);
	glVertexAttribPointer(NormalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((colors.size()+ points.size()) * sizeof(vec4)));


	glDrawArrays(GL_TRIANGLES, 0, points.size());
}


mat4 Transpose(mat4 matrix) {
	mat4 t;
	for (int i = 0;i != 4;i++) {
		for (int j = 0;j != 4;j++) {
			t[i][j] = matrix[j][i];
		}
	}
	return t;
}


// ͨ��Deltaֵ����Theta
void updateTheta(int axis, int sign) {
	switch (currentTransform) {
	case TRANSFORM_SCALE:
		scaleTheta[axis] += sign * scaleDelta;
		break;
	case TRANSFORM_ROTATE:
		rotateTheta[axis] += sign * rotateDelta;
		break;
	case TRANSFORM_TRANSLATE:
		translateTheta[axis] += sign * translateDelta;
		break;
	}
}

void idle() {
	frame++;
	if (frame%2000 == 0) {
		if (frame % 20000 == 0) {
			TEXTURE2++;
\
			if (abs(theta[LeftUpperLeg]-150)<0.001) {
				theta[LeftUpperLeg] = theta[RightUpperLeg];
				theta[RightUpperLeg] = 150;
			}
			else {
				theta[RightUpperLeg] = theta[LeftUpperLeg];				
				theta[LeftUpperLeg] = 150;
			}

		}
		//translate.x++;
		updateTheta(currentAxis, currentDirection);
		glutPostRedisplay();
	}
	
}

void keyboard(unsigned char key, int x, int y) {

	direction = vec3(
		-cos(verticalAngle*M_PI / 180.0) * sin(horizontalAngle*M_PI / 180.0),
		sin(verticalAngle*M_PI / 180.0),
		-cos(verticalAngle*M_PI / 180.0) * cos(horizontalAngle*M_PI / 180.0)
	);
	right0 = vec3(
		cos(horizontalAngle*M_PI / 180.0),
		0,
		-sin(horizontalAngle*M_PI / 180.0)
	);

	direction = normalize(direction);
	right0 = normalize(right0);
	up = cross(right0, direction);
	switch (key) {
	case 'w':
		position += direction  * speed;
		break;
	case 'd':
		position += right0  * speed;
		break;
	case 's':
		position -= direction  * speed;
		break;
	case 'a':
		position -= right0  * speed;
		break;
	}



	switch (key) {
	case 'x':
		translate.x -= 0.1;
		break;
	case 'X':
		translate.x += 0.1;
		break;
	case 'y':
		translate.y -= 0.1;
		break;
	case 'Y':
		translate.y += 0.1;
		break;
	case 'z':
		translate.z -= 0.1;
		break;
	case 'Z':
		translate.z += 0.1;
		break;
	case ' ':
		canCout = !canCout;
		break;
	case 'r':
		theta[angle] += 5.0;
		if (theta[angle] > 360.0) { theta[angle] -= 360.0; }
		break;
	case 'R':
		theta[angle] -= 5.0;
		if (theta[angle] < 0.0) { theta[angle] += 360.0; }
		break;
	}
	glutPostRedisplay();
}



void quad(int a, int b, int c, int d)
{
	cube_colors[Index] = vertex_colors[a];
	cube_points[Index] = cube_vertices[a]; 
	cube_normals[Index] = vec3(cube_vertices[a].x, cube_vertices[a].y, cube_points[a].z);
	Index++;
	cube_colors[Index] = vertex_colors[a]; 
	cube_points[Index] = cube_vertices[b]; 
	cube_normals[Index] = vec3(cube_vertices[b].x, cube_vertices[b].y, cube_points[b].z);
	Index++;
	cube_colors[Index] = vertex_colors[a]; 
	cube_points[Index] = cube_vertices[c]; 
	cube_normals[Index] = vec3(cube_vertices[c].x, cube_vertices[c].y, cube_points[c].z);
	Index++;
	cube_colors[Index] = vertex_colors[a]; 
	cube_points[Index] = cube_vertices[a]; 
	cube_normals[Index] = vec3(cube_vertices[a].x, cube_vertices[a].y, cube_points[a].z);
	Index++;
	cube_colors[Index] = vertex_colors[a]; 
	cube_points[Index] = cube_vertices[c]; 
	cube_normals[Index] = vec3(cube_vertices[c].x, cube_vertices[c].y, cube_points[c].z);
	Index++;
	cube_colors[Index] = vertex_colors[a]; 
	cube_points[Index] = cube_vertices[d]; 
	cube_normals[Index] = vec3(cube_vertices[d].x, cube_vertices[d].y, cube_points[d].z);
	Index++;
}

void colorcube(void)//�������ɵ�λ���������������
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void torso(vec4 color_torso)
{
	mvstack.push(model_view);//���游�ڵ����

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(colorID, 1, color_torso);
	
	float lx = -lightPos[0];
	float ly = -(lightPos[1] + 2);
	float lz = -lightPos[2];

	mat4 shadowProjMatrix(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0, 0.0, -ly
	);

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view* Translate(0,-(LOWER_LEG_HEIGHT+UPPER_LEG_HEIGHT),0) * shadowProjMatrix);
	glUniform4fv(colorID, 1, black);

	// ������Ӱ
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);


	model_view = mvstack.pop();//�ָ����ڵ����
}


void head(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_upper_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_lower_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void right_upper_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void right_lower_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void left_upper_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_lower_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void right_upper_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void right_lower_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void initRobot() {

	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors)+sizeof(cube_normals),NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),cube_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors), sizeof(cube_normals), cube_normals);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void displayRobot(mat4 modelViewProjMatrix) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform1i(isCubeID, 1);//����ȷ�������ǲ������������ɫ
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube_points)));
	glEnableVertexAttribArray(NormalID);
	glVertexAttribPointer(NormalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube_points)+sizeof(cube_colors)));

	GLfloat left = -10.0, right = 10.0;
	GLfloat bottom = -5.0, top = 15.0;
	GLfloat zNear = -10.0, zFar = 10.0;

	GLfloat aspect = GLfloat(windowWidth) / windowHeight;

	if (aspect > 1.0) {
		left *= aspect;
		right *= aspect;
	}
	else {
		bottom /= aspect;
		top /= aspect;
	}

	mat4 projection = Camera::ortho(left, right, bottom, top, zNear, zFar);

	model_view = modelViewProjMatrix* projection*Translate(0,0,0)*RotateY(theta[Torso]);//* RotateY(theta[Torso]);//���ɱ任����
	torso(blue);//���ɻ���

	mvstack.push(model_view);//�������ɱ任����
	model_view *= Translate(0, TORSO_HEIGHT, 0)*RotateZ(theta[Head2])*RotateY(theta[Head1]);
	head(green);//ͷ������
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(-0.5*TORSO_WIDTH, TORSO_HEIGHT, 0)*RotateX(theta[LeftUpperArm]);
	left_upper_arm(red);//���ϱۻ���
	model_view *= Translate(0, UPPER_ARM_HEIGHT, 0)*RotateX(theta[LeftLowerArm]);
	left_lower_arm(yellow);//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(0.5*TORSO_WIDTH, TORSO_HEIGHT, 0)*RotateZ(theta[RightUpperArm]);
	right_upper_arm(red);//���ϱۻ���
	model_view *= Translate(0, UPPER_ARM_HEIGHT, 0)*RotateZ(theta[RightLowerArm]);
	right_lower_arm(yellow);//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(-0.5*TORSO_WIDTH, 0, 0)*RotateX(theta[LeftUpperLeg]); //�����Ⱦֲ��任����;
	left_upper_leg(cyan);//�����Ȼ���
	model_view *= Translate(0, UPPER_LEG_HEIGHT, 0)*RotateX(theta[LeftLowerLeg]);//�����Ⱦֲ��任����;
	left_lower_leg(magenta);//�����Ȼ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(0.5*TORSO_WIDTH, 0, 0)*RotateX(theta[RightUpperLeg]);//�����Ⱦֲ��任����;
	right_upper_leg(cyan);//�����Ȼ���
	model_view *= Translate(0, UPPER_LEG_HEIGHT, 0)*RotateX(theta[RightLowerLeg]);//�����Ⱦֲ��任����;
	right_lower_leg(magenta);//�����Ȼ���
	model_view = mvstack.pop();//�ָ����ɱ任����

}

void addMenuItem() {
	glutAddMenuEntry("torso", Torso);
	glutAddMenuEntry("head1", Head1);
	glutAddMenuEntry("head2", Head2);
	glutAddMenuEntry("right_upper_arm", RightUpperArm);
	glutAddMenuEntry("right_lower_arm", RightLowerArm);
	glutAddMenuEntry("left_upper_arm", LeftUpperArm);
	glutAddMenuEntry("left_lower_arm", LeftLowerArm);
	glutAddMenuEntry("right_upper_leg", RightUpperLeg);
	glutAddMenuEntry("right_lower_leg", RightLowerLeg);
	glutAddMenuEntry("left_upper_leg", LeftUpperLeg);
	glutAddMenuEntry("left_lower_leg", LeftLowerLeg);
	glutAddMenuEntry("quit", Quit);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}


void menu(int option)
{
	if (option == Quit) {
		exit(EXIT_SUCCESS);
	}

	angle = option;
}






void CreateBox(vector<vec3> cubePoints, ImgURL imgURL,CUBE &cube) {
	vector<vec3> p = {
		vec3(-1,1,0),vec3(-1,-1,0),vec3(1,-1,0),vec3(1,1,0)
	};

	vector<vec3i> in = {vec3i(0,1,2),vec3i(0,3,2)};
	vector<vec2> uvs = { vec2(0,1),vec2(0,0),vec2(1,0),vec2(1,1) };//uvs�б�������붥����������Ӧ���������꣬������ʵ�ʵ�����ʵ�ʵ���������������ȡ������

	vector<vec4> indexs = {
		vec4(0,1,3,2),vec4(2,3,5,4),vec4(6,7,5,4),
		vec4(6,7,1,0),vec4(6,0,2,4),vec4(7,1,3,5)
	};

	vector<vector<vec3>> cubeVertexs;

	for (int i = 0;i != 6;i++) {
		vector<vec3> points;
		points.push_back(cubePoints[indexs[i][0]]);
		points.push_back(cubePoints[indexs[i][1]]);
		points.push_back(cubePoints[indexs[i][2]]);
		points.push_back(cubePoints[indexs[i][3]]);
		cubeVertexs.push_back(points);
	}
	Createcube(cubeVertexs[0], in, uvs, FRONT, imgURL.front_img, cube);
	Createcube(cubeVertexs[1], in, uvs, RIGHT, imgURL.right_img, cube);
	Createcube(cubeVertexs[2], in, uvs, BACK, imgURL.back_img, cube);
	Createcube(cubeVertexs[3], in, uvs, LEFT, imgURL.left_img, cube);
	Createcube(cubeVertexs[4], in, uvs, TOP, imgURL.top_img, cube);
	Createcube(cubeVertexs[5], in, uvs, BOTTOM, imgURL.bottom_img, cube);
	
}


void displayBox(mat4 modelViewMatrix, CUBE& cube) {

	displaycube(FRONT, modelViewMatrix, cube);
	displaycube(RIGHT, modelViewMatrix, cube);
	displaycube(BACK, modelViewMatrix, cube);
	displaycube(LEFT, modelViewMatrix, cube);
	displaycube(TOP, modelViewMatrix, cube);
	displaycube(BOTTOM, modelViewMatrix, cube);
}


void Createcube(vector<vec3> cubeVertex, vector<vec3i> index, vector<vec2> uvs, GLuint face, string file,CUBE &cube) {


	CreateTexture(file, cube.cubeTexBO[face]);

	glGenBuffers(1, &cube.cubeVBO[face]);// ����
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, cubeVertex.size() * sizeof(vec3), &cubeVertex[0], GL_STATIC_DRAW);


	glGenBuffers(1, &cube.cubeIBO[face]);// ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.cubeIBO[face]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(vec3i), &index[0], GL_STATIC_DRAW);

	glGenBuffers(1, &cube.cubeUVBO[face]);// UV
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeUVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
										
	glGenBuffers(1, &cube.cubeNormal[face]);//������
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeNormal[face]);
	glBufferData(GL_ARRAY_BUFFER, cubeVertex.size() * sizeof(vec3), &cubeVertex[0], GL_STATIC_DRAW);

	
}

void displaycube(GLuint face, mat4 m,CUBE &cube) {
	glUniform1i(isCubeID, 0);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, m);

	//���ݶ��㡢������UV
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeVBO[face]); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //���߹�����������bufer�е�����  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.cubeIBO[face]);


	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeUVBO[face]);
	glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//������ͼ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube.cubeTexBO[face]);
	glUniform1i(TextureID, 0);

	//����
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

