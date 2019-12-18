#include "Angel.h"
#include "mat.h"
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



//////////////////////////////////////////////////////////////////////////////////
//   ����
vector<vec3> ground_points;
vector<vec3> POINTS1;
vector<vec3> INDEXS;
GLuint GROUND_IBO;
GLuint GROUND_VBO;
GLuint GROUND_UVBO;
vector<vec2> GROUND_UV;
int TEXTURE2 = 0;



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
float verticeScale = 0.1f;
float yScale = 10.0f;
int imgWidth;
int imgHeight;
static  GLfloat *vertices;
static unsigned int *indices;
static  GLfloat *uvs;
static GLfloat *normals;
GLuint VBO;//���㻺�����
GLuint IBO;//�����������
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
vector<vec4> points;
vector<vec4> colors;
int nVertices, nFaces, nLine;
// ����ĸ�����
vector<vec4> vertices1;
// ÿ������Ķ�Ӧ��ɫ
vector<vec4> vertexColors;
GLuint cube;
GLuint colorID;


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
void generate(string filename);
static void SpecialKeyboard(int Key, int x, int y);
void mouseCB(int button, int state, int x, int y);
void mouseMotionCB(int x, int y);
FIBITMAP* getDib(string ptrFileName);
void CreateVertexBuffer(string file);
void CreateIndexBuffer();
void load_texture_FreeImage(std::string file_name, GLuint& m_texName);
void CreateTexture(string filename, GLuint &OBJ);
void CreateNormal();
void init();
void Display();
void updateTheta(int axis, int sign);
void idle();
void keyboard(unsigned char key, int x, int y);
void drawGround();
mat4 Transpose(mat4 matrix);
void mouseWheel(int, int, int, int);
void CreateSky(vector<vec3> skyVertex, vector<vec3> index, vector<vec2> uvIndex, GLuint face, string file);
void displaySky(GLuint face, mat4 m);

void CreateBox();
void displayBox(mat4 modelViewMatrix);

///
void CreateBox(vector<vec3> cubePoints, ImgURL imgURL, CUBE &cube);
void displayBox(mat4 modelViewMatrix, CUBE& cube);
void Createcube(vector<vec3> cubeVertex, vector<vec3> index, vector<vec2> uvIndex, GLuint face, string file, CUBE &cube);
void displaycube(GLuint face, mat4 m, CUBE &cube);



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
void Create();
void displayGROUND(mat4 m);

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

void generate(string filename)
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
	//cout << position << direction << " " << verticalAngle << " " << horizontalAngle << endl;
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


float getPix(BYTE *pixels, int i, int k) {
	return 1.0*(int(pixels[imgHeight*i + k]));
}

//��������

void CreateVertexBuffer(string file)
{
	fstream in(file);
	in >> imgHeight >> imgWidth;
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			int pix;
			in >> pix;
			ground_points.push_back(vec3(verticeScale* 1.0*(i), yScale* 1.0*pix / 255.0f, verticeScale * 1.0*(j)));
		}
	}
	//��ʼ�����
	position = vec3(0.5*verticeScale*(imgWidth - 1), 0.5*yScale*Boxheight, 0.5*verticeScale*(imgHeight - 1));
	direction = vec3(-0.5*verticeScale*(imgWidth - 1), -0.5*yScale*Boxheight, -0.5*verticeScale*(imgHeight - 1));
}

void CreateIndexBuffer()
{
	GLfloat W = imgWidth;
	GLfloat H = imgHeight;
	int f = 0;
	for (int i = 0; i < imgHeight - 1; i++){
		for (int j = 0; j < imgWidth - 1; j++){

			POINTS1.push_back(ground_points[i*imgWidth + j]);
			POINTS1.push_back(ground_points[i*imgWidth + j + 1]);
			POINTS1.push_back(ground_points[(i + 1)*imgWidth + j]);

			POINTS1.push_back(ground_points[(i + 1)*imgWidth + j]);
			POINTS1.push_back(ground_points[i*imgWidth + j + 1]);
			POINTS1.push_back(ground_points[(i + 1)*imgWidth + j + 1]);

			INDEXS.push_back(vec3(i*imgWidth + j,i*imgWidth + j + 1,(i + 1)*imgWidth + j));
			INDEXS.push_back(vec3((i + 1)*imgWidth + j, i*imgWidth + j + 1, (i + 1)*imgWidth + j + 1));

			GROUND_UV.push_back(vec2(1.0*((j + f) % imgWidth) / W, 1.0*(H - i) / H));
			GROUND_UV.push_back(vec2(1.0*((j + 1 + f) % imgWidth) / W, 1.0*(H - i) / H));
			GROUND_UV.push_back(vec2(1.0*((j + f) % imgWidth) / W, 1.0*(H - i - 1) / H));

			GROUND_UV.push_back(vec2(1.0*((j + 1 + f) % imgWidth) / W, 1.0*(H - i) / H));
			GROUND_UV.push_back(vec2(1.0*((j + f) % imgWidth) / W, 1.0*(H - i - 1) / H));
			GROUND_UV.push_back(vec2(1.0*((j + 1 + f) % imgWidth) / W, 1.0*(H - i - 1) / H));
		}
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
		0, //! ָ��Ϊ��һ����,����������mipmap,��lod,����ľͲ��ü�����,Զ��ʹ�ý�С������
		GL_RGB, //! ������ʹ�õĴ洢��ʽ
		width, //! ����,��һ����Կ�,��֧�ֲ����������,�����Ⱥ͸߶Ȳ���2^n��
		height, //! ����,��һ����Կ�,��֧�ֲ����������,�����Ⱥ͸߶Ȳ���2^n��
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
	glGenerateMipmap(GL_TEXTURE_2D);
	
}

//��������
void CreateNormal()
{
	for (int i = 0;i != INDEXS.size();i++) {
		vec3 index = INDEXS[i];
		vec3 point1 = POINTS1[index[0]];
		vec3 point2 = POINTS1[index[1]];
		vec3 point3 = POINTS1[index[2]];

		vec3 n1 = point1 - point2;
		vec3 n2 = point1 - point3;
		vec3 N = normalize(cross(n1, n2));

		NORMALS.push_back(N);
	}
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

	m_dirLightLocation.Color = glGetUniformLocation(programID, "gDirectionalLight.Color");
	m_dirLightLocation.AmbientIntensity = glGetUniformLocation(programID, "gDirectionalLight.AmbientIntensity");
	m_dirLightLocation.Direction = glGetUniformLocation(programID, "gDirectionalLight.Direction");
	m_dirLightLocation.DiffuseIntensity = glGetUniformLocation(programID, "gDirectionalLight.DiffuseIntensity");
	m_dirLightLocation.lightPos = glGetUniformLocation(programID, "gDirectionalLight.lightPos");

	CreateVertexBuffer("./data/data_2png.txt");
	CreateIndexBuffer();
	CreateTexture("./image/t2.jpg", TexBO);//�ײ�
	CreateTexture("./image/t1.jpg", TexBO2);
	CreateNormal();
	CreateBox();
	Create();
	
	generate("./off/cow.off");

	glGenBuffers(1, &cube);
	glBindBuffer(GL_ARRAY_BUFFER, cube);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4)+ colors.size() * sizeof(vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);

	initRobot();
}

void CreateBox() {
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

void displayBox(mat4 modelViewMatrix) {

	displayBox(modelViewMatrix, SKY_CUBE);
}


void Create() {

	glGenBuffers(1, &GROUND_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_VBO);
	glBufferData(GL_ARRAY_BUFFER, POINTS1.size() * sizeof(vec3), &POINTS1[0], GL_STATIC_DRAW);

	glGenBuffers(1, &GROUND_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GROUND_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEXS.size() * sizeof(vec3), &INDEXS[0], GL_STATIC_DRAW);


	glGenBuffers(1, &GROUND_UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_UVBO);
	glBufferData(GL_ARRAY_BUFFER, GROUND_UV.size() * sizeof(vec2), &GROUND_UV[0], GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBO);
	glBufferData(GL_ARRAY_BUFFER, NORMALS.size() * sizeof(vec3), &NORMALS[0], GL_STATIC_DRAW);
}

void displayGROUND(mat4 m) {

	GLuint testID = glGetUniformLocation(programID, "test");
	glUniform1f(testID, 1.0*((TEXTURE2) % imgWidth) / imgWidth);
	glUniform1i(isCubeID, 2);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, m);

	//���ݶ��㡢������UV
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_VBO); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //���߹�����������bufer�е�����  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GROUND_IBO);



	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(NormalID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, GROUND_UVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	
	glEnableVertexAttribArray(NormalID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	

	//������ͼ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexBO);
	glUniform1i(TextureID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexBO2);
	glUniform1i(TextureID2, 1);

	//����
	glDrawArrays(GL_TRIANGLES, 0, POINTS1.size()*3);
}
/**/
void CreateSky(vector<vec3> skyVertex,vector<vec3> index,vector<vec2> uvIndex,GLuint face,string file) {

	CreateTexture(file, sky.SkyTexBO[face]);

	glGenBuffers(1, &sky.SkyVBO[face]);
	glBindBuffer(GL_ARRAY_BUFFER, sky.SkyVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, skyVertex.size() * sizeof(vec3), &skyVertex[0], GL_STATIC_DRAW);

	glGenBuffers(1, &sky.SkyIBO[face]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sky.SkyIBO[face]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(vec3), &index[0], GL_STATIC_DRAW);

	glGenBuffers(1, &sky.SkyUVBO[face]);
	glBindBuffer(GL_ARRAY_BUFFER, sky.SkyUVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, uvIndex.size() * sizeof(vec2), &uvIndex[0], GL_STATIC_DRAW);
}


void displaySky(GLuint face, mat4 m) {
	
	glUniform1i(isCubeID, 0);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, m);

	//���ݶ��㡢������UV
	glBindBuffer(GL_ARRAY_BUFFER, sky.SkyVBO[face]); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //���߹�����������bufer�е�����  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sky.SkyIBO[face]);


	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, sky.SkyUVBO[face]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//������ͼ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky.SkyTexBO[face]);
	glUniform1i(TextureID, 0);

	//����
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Display() {
	// �����ɫ���� 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(vPositionID); //������������
	glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, mat4(1));
	float FoV = 1.0*initialFoV*M_PI / 180.0;
	Camera::projMatrix = Camera::perspective(FoV, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	Camera::viewMatrix = Camera::lookAt(
		position,
		position + direction,
		vec4(up,0)
	);
	Camera::modelMatrix = mat4(1); Scale(2, 2, 2);
	mat4 modelViewProjMatrix = Camera::modelMatrix*Camera::viewMatrix*Camera::projMatrix;
	//mat4 modelViewProjMatrix = RotateX(10)*mat4(1)*RotateY(rotateTheta[1])*Scale(0.02, 0.02, 0.02);

	//���ݹ���	
	vec4 center = (verticeScale*1.0*(imgWidth - 1)*0.5, yScale* Boxheight, verticeScale*1.0*(imgHeight - 1)*0.5, 1);
	vec4 lightPos = RotateY(rotateTheta[1])*modelViewProjMatrix*center + translate;
	if(!canCout)
		cout << lightPos << endl;
	//lightPos = vec4(3.69218, 6.55898, 5.20812, 2.02508);
	glUniform3f(m_dirLightLocation.Color, 1, 1, 1);
	glUniform3f(m_dirLightLocation.Direction, 0, 0, 0);
	glUniform4f(m_dirLightLocation.lightPos, lightPos.x, lightPos.y, lightPos.z, 1);
	glUniform1f(m_dirLightLocation.AmbientIntensity, 0.5);//�����ⷴ��ϵ��
	glUniform1f(m_dirLightLocation.DiffuseIntensity, 0.5);//������ϵ��

	//���Ƶ���
	displayGROUND(modelViewProjMatrix);
	displayBox(modelViewProjMatrix);
	displayRobot(modelViewProjMatrix);
	
	//����������
	vec3 at = normalize( direction)+translate;
	mat4 m = Translate(at.x, at.y, at.z)*RotateY(rotateTheta[1]);
	//m = Transpose(modelViewProjMatrix)*m;
	
	m = Transpose(modelViewProjMatrix)*Translate(0.5*verticeScale*(imgWidth - 1), 0.5*yScale*Boxheight, 0.5*verticeScale*(imgHeight - 1))*RotateY(rotateTheta[1]);
	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, m);
	glBindBuffer(GL_ARRAY_BUFFER, cube); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform1i(isCubeID, 1);//����ȷ�������ǲ������������ɫ
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec4)));
	glDrawArrays(GL_TRIANGLES, 0, points.size());


	m *= Translate(position.x, position.y, position.z)*Scale(4,4,4);
	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, m);
	glBindBuffer(GL_ARRAY_BUFFER, cube); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform1i(isCubeID, 1);//����ȷ�������ǲ������������ɫ
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec4)));
	glDrawArrays(GL_TRIANGLES, 0, points.size());


	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutSwapBuffers();

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
		}

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

void drawGround() {

}


void quad(int a, int b, int c, int d)
{
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[a]; Index++;
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[b]; Index++;
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[c]; Index++;
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[a]; Index++;
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[c]; Index++;
	cube_colors[Index] = vertex_colors[a]; cube_points[Index] = cube_vertices[d]; Index++;
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

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	//glUniform4fv(colorID, 1, color_torso);
	model_view = mvstack.pop();//�ָ����ڵ����
}


void head(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_upper_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_lower_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void right_upper_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void right_lower_arm(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void left_upper_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void left_lower_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}

void right_upper_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
	glUniform4fv(colorID, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniform4fv(colorID, 1, color_torso);
}


void right_lower_leg(vec4 color_torso)
{
	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//���ڵ�ֲ��任����

	glUniformMatrix4fv(MatrixID, 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors),NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),cube_colors);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void displayRobot(mat4 modelViewProjMatrix) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glUniform1i(isCubeID, 1);//����ȷ�������ǲ������������ɫ
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cube_points)));
	
	vec3 at = normalize(direction) + translate;
	mat4 m = Translate(at.x, at.y, at.z)*RotateY(rotateTheta[1]);
	//m = Transpose(modelViewProjMatrix)*m;

	vec3 center(0.5*verticeScale*(imgWidth - 1), 0.5*yScale*Boxheight, 0.5*verticeScale*(imgHeight - 1));
	vec3 cameraPos = position + direction+translate;
	//m = Transpose(modelViewProjMatrix)*Translate(center.x, center.y, center.z)*RotateY(rotateTheta[1]);//�Ƶ�����
	m = Transpose(modelViewProjMatrix)*Translate(cameraPos.x, cameraPos.y, cameraPos.z-1)*Scale(.5,.5,.5);//*RotateY(rotateTheta[1]);//�Ƶ����

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

	model_view = m* projection*Translate(0,00,0)*RotateY(theta[Torso]);//* RotateY(theta[Torso]);//���ɱ任����
	torso(blue);//���ɻ���

	mvstack.push(model_view);//�������ɱ任����
	model_view *= Translate(0, TORSO_HEIGHT, 0)*RotateZ(theta[Head2])*RotateY(theta[Head1]);
	head(green);//ͷ������
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(-0.5*TORSO_WIDTH, TORSO_HEIGHT, 0)*RotateZ(theta[LeftUpperArm]);
	left_upper_arm(red);//���ϱۻ���
	model_view *= Translate(0, UPPER_ARM_HEIGHT, 0)*RotateZ(theta[LeftLowerArm]);
	left_lower_arm(yellow);//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(0.5*TORSO_WIDTH, TORSO_HEIGHT, 0)*RotateZ(theta[RightUpperArm]);
	right_upper_arm(red);//���ϱۻ���
	model_view *= Translate(0, UPPER_ARM_HEIGHT, 0)*RotateZ(theta[RightLowerArm]);
	right_lower_arm(yellow);//���±ۻ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(-0.5*TORSO_WIDTH, 0, 0)*RotateZ(theta[LeftUpperLeg]); //�����Ⱦֲ��任����;
	left_upper_leg(cyan);//�����Ȼ���
	model_view *= Translate(0, UPPER_LEG_HEIGHT, 0)*RotateZ(theta[LeftLowerLeg]);//�����Ⱦֲ��任����;
	left_lower_leg(magenta);//�����Ȼ���
	model_view = mvstack.pop();//�ָ����ɱ任����

	mvstack.push(model_view); //�������ɱ任����
	model_view *= Translate(0.5*TORSO_WIDTH, 0, 0)*RotateZ(theta[RightUpperLeg]);//�����Ⱦֲ��任����;
	right_upper_leg(cyan);//�����Ȼ���
	model_view *= Translate(0, UPPER_LEG_HEIGHT, 0)*RotateZ(theta[RightLowerLeg]);//�����Ⱦֲ��任����;
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

	vector<vec3> index = {
		vec3(0,1,3),vec3(0,2,3),
		vec3(2,3,5),vec3(2,4,5),
		vec3(6,7,5),vec3(6,4,5),
		vec3(6,7,1),vec3(6,0,1),
		vec3(6,0,2),vec3(6,4,2),
		vec3(7,1,3),vec3(7,5,3)
	};

	vector<vec2> uv = { vec2(0, 1), vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0) };
	vector<vec3> boxPoints;//�����嶥������

	vector<vector<vec3>> cubeVertexs;
	vector<vector<vec3>> indexs;
	vector<vector<vec2>> uvss;

	for (int i = 0;i != 6;i++) {
		vector<vec3> points;
		vector<vec3> index1;

		index1.push_back(index[2 * i]);
		index1.push_back(index[2 * i + 1]);

		vec3 p1 = cubePoints[index[2 * i].x];
		vec3 p2 = cubePoints[index[2 * i].y];
		vec3 p3 = cubePoints[index[2 * i].z];
		points.push_back(p1);
		points.push_back(p2);
		points.push_back(p3);

		p1 = cubePoints[index[2 * i + 1].x];
		p2 = cubePoints[index[2 * i + 1].y];
		p3 = cubePoints[index[2 * i + 1].z];
		points.push_back(p1);
		points.push_back(p2);
		points.push_back(p3);

		cubeVertexs.push_back(points);
		indexs.push_back(index1);
	}

	Createcube(cubeVertexs[0], indexs[0], uv, FRONT, imgURL.front_img, cube);
	Createcube(cubeVertexs[1], indexs[1], uv, RIGHT, imgURL.right_img, cube);
	Createcube(cubeVertexs[2], indexs[2], uv, BACK, imgURL.back_img, cube);
	Createcube(cubeVertexs[3], indexs[3], uv, LEFT, imgURL.left_img, cube);
	Createcube(cubeVertexs[4], indexs[4], uv, TOP, imgURL.top_img, cube);
	Createcube(cubeVertexs[5], indexs[5], uv, BOTTOM, imgURL.bottom_img, cube);
	
	
}


void displayBox(mat4 modelViewMatrix, CUBE& cube) {

	displaycube(FRONT, modelViewMatrix, cube);
	displaycube(RIGHT, modelViewMatrix, cube);
	displaycube(BACK, modelViewMatrix, cube);
	displaycube(LEFT, modelViewMatrix, cube);
	displaycube(TOP, modelViewMatrix, cube);
	displaycube(BOTTOM, modelViewMatrix, cube);
}


void Createcube(vector<vec3> cubeVertex, vector<vec3> index, vector<vec2> uvIndex, GLuint face, string file,CUBE &cube) {

	CreateTexture(file, cube.cubeTexBO[face]);

	glGenBuffers(1, &cube.cubeVBO[face]);// ����
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, cubeVertex.size() * sizeof(vec3), &cubeVertex[0], GL_STATIC_DRAW);

	glGenBuffers(1, &cube.cubeIBO[face]);// ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.cubeIBO[face]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(vec3), &index[0], GL_STATIC_DRAW);

	glGenBuffers(1, &cube.cubeUVBO[face]);// UV
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeUVBO[face]);
	glBufferData(GL_ARRAY_BUFFER, uvIndex.size() * sizeof(vec2), &uvIndex[0], GL_STATIC_DRAW);
}

void displaycube(GLuint face, mat4 m,CUBE &cube) {
	glUniform1i(isCubeID, 0);
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, m);

	//���ݶ��㡢������UV
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeVBO[face]); //��GL_ARRAY_BUFFER������ 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //���߹�����������bufer�е�����  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.cubeIBO[face]);


	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, cube.cubeUVBO[face]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//������ͼ����
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube.cubeTexBO[face]);
	glUniform1i(TextureID, 0);

	//����
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
