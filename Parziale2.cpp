#include "Figures.h"
#include "Lib.h"
#include "ShaderMaker.h"
#include "Structs.h"
#include "Light.h"
#include "Camera.h"
#include "gestioneTexture.h"
#include "load_meshes_assimp.hpp"
#include "iostream"

int width = 1024;
int height = 800;

int w_up = width;
int h_up = height;

float angolo = 0.0f;

int last_mouse_pos_Y;
int last_mouse_pos_X;
bool movingTrackball = false;
bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;
float Theta = -90.0f;
float Phi = 0.0f;

int selectedObj = -1;
bool showAnchors = false;

static unsigned int programId, programIdcube, programId1, programIdr, MatrixProj, MatView, MatModel;
static unsigned int MatrixProjS, MatViewS, MatrixProjR , MatViewR, MatModelR;
static unsigned int loc_view_pos, loc_view_posR, loc_cubemapR, loc_time, loc_texture, lsceltaVS, lsceltaFS;

unsigned int textureMuro, cubemapTexture, textureMarble, texturePavimento, textureStucco, textureDivano;
unsigned int textureMetal;

mat4 Projection, Model, View;

string Meshdir = "Meshes/";
string Texdir = "Textures/";

Cube* sky;
vector<Mesh*> scene;
vector<Mesh*> mirrors;
vector<ObjMeshVector*> sceneObj;

Sphere* lightbulb;

vector<std::string> faces
{
	(Texdir + "right.jpg"),
	(Texdir + "left.jpg"),
	(Texdir + "top.jpg"),
	(Texdir + "bottom.jpg"),
	(Texdir + "front.jpg"),
	(Texdir + "back.jpg")
};

LightShaderUniform light_unif = {};
Light light;
Camera ViewSetup;
Perspective PerspectiveSetup;


void update(int a)
{
	//angolo += 0.5;
	glutPostRedisplay();
	glutTimerFunc(10, update, 0);
}

void resize(int w, int h)
{
	//Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ci? che ? nel mondo
	 //Se l'aspect ratio del mondo ? diversa da quella della finestra devo mappare in modo diverso 
	 //per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h)   //Se ridimensioniamo la larghezza della Viewport
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_up = (float)w;
		h_up = w / AspectRatio_mondo;
	}
	else {  //Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo 
			//e la finestra sullo schermo sono uguali
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_up = h * AspectRatio_mondo;
		h_up = (float)h;
	}
}

vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {

	mouse_y = height - mouse_y;

	// mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = (2.0f * mouse_y) / height - 1.0;
	float z = 1.0f;
	vec3  ray_nds = vec3(x, y, z);
	// Nel clip space hanno coordinate: nota bene la terza coordinata uguale a -1
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// Le coordinate Nell' eye space si ottengono premoltiplicando per l'inversa della matrice Projection.

	vec4 ray_eye = inverse(Projection) * ray_clip;

	//Direzione del raggio
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// le coordinate nel world space: si ottengono premoltiplicando per l'inversa della matrice di Vista 
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - cc;

	if (delta < 0)  //Il raggio non interseca la sfera
		return false;
	//Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione 
	//pi? vicina all'osservatore (la t pi? piccola)
	if (delta > 0.0f) {
		//calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		//Caso di intersezioni dietro l'osservatore
		if (t_a < 0.0) {
			if (t_b < 0)
				return false;
		}

		return true;
	}
	//Caso in cui il raggio ? tangente alla sfera: un interesezione con molteplicit? doppia.
	if (delta == 0) {
		float t = -b + sqrt(delta);
		if (t < 0)
			return false;
		*intersection_distance = t;
		return true;
	}

	return false;
}

vec3 getTrackBallPoint(float x, float y)
{
	//Dalla posizione del mouse al punto proiettato sulla semisfera con centro l'origine e raggio 1

	float Delta, tmp;
	vec3 point;
	//map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	//Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
	Delta = 1.0f - tmp;
	if (Delta > 0.0f)
		point.z = sqrt(Delta);
	else
		point.z = 0;

	return point;
}

void mouseActiveMotion(int x, int y)
{

	float velocity = 40;
	float pi = glm::pi<float>();
	// Spostamento tramite  trackball del vettore posizione Camera 
	if (!movingTrackball) {
		return;
	}
	vec3 corrente = normalize(getTrackBallPoint(x, y));
	vec3 precedente = normalize(getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y));

	vec3 rotation_vec = cross(precedente, corrente);

	//Calcoliamo il coseno dell'angolo tra il segmento che collega l'origine e il click corrente e il segmento
	//che collega l'origine ed il click corrente
	float cosangolo = dot(precedente, corrente);

	//Il prodotto scalare tra due vettori normalizzati che hanno lunghezza 1,
	// pu? variare tra -1 ed 1.
	//Per problemi numerici dovuti al fatto che lavoriamo con i numeri finiti, per evitare che acos
	// dia risulato nan, nel caso in cui cosangolo risulti maggiore di 1 rimettiamolo ad 1
	if (cosangolo > 1)
		cosangolo = 1.0;
	// nel caso in cui cosangolo risulti minore di -1 rimettiamolo a -1
	if (cosangolo < -1)
		cosangolo = -1.0;

	//Calcoliamo l'angolo utilizzando l'arcocoseno
	float theta = (acos(cosangolo)) * velocity;

	//Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
	quat rotation = angleAxis(theta, rotation_vec);
	//Trasformiamo il quaternione in matrice di rotazione
	mat4 rotation_matrix = toMat4(rotation);
	//Aggiorniamo direzione e posizione della telecamera
	ViewSetup.direction = ViewSetup.position - ViewSetup.target;
	ViewSetup.position = ViewSetup.target + rotation_matrix * ViewSetup.direction;

	//Memorizzo l' ultima posizione del mouse

	last_mouse_pos_X = x;

	last_mouse_pos_Y = y;

	glutPostRedisplay();

}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		//Con il tasto sinistro premuto si attiva la modalit? di trackball
		if (state == GLUT_DOWN) { movingTrackball = true; }
		if (state == GLUT_UP) { movingTrackball = false; }

		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
	}

	if (button == GLUT_RIGHT_BUTTON) {

		//Con il tasto destro si selezionano gli oggetti nella scena
		if (state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			float raggio_sfera = 1;
			float xmouse = x;
			float ymouse = y;
			vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

			selectedObj = -1;
			float closest_intersection = 0.0f;
			for (int i = 0; i < scene.size(); i++)
			{
				float t_dist = 0.0f;
				//Interseco il raggio che esce dalla camera nella direzione del mouse con la sfera centrata nell'ancora di tutti gli oggetti 
				//posizionati nel mondo per individuare se c'? intersezione con l'oggetto
				if (ray_sphere(ViewSetup.position, ray_wor, scene[i]->ancora_world, raggio_sfera, &t_dist))
				{
					if (selectedObj == -1 || t_dist < closest_intersection)
					{
						selectedObj = i;
						closest_intersection = t_dist;
					}
				}
			}
			if (selectedObj > -1)
				printf("Selezionato %s [%d]\n", scene[selectedObj]->name.c_str(), selectedObj);
			else
				cout << "Nessun oggetto selezionato" << endl;
		}
	}

	glutPostRedisplay();
}

void my_passive_mouse(int xpos, int ypos)
{
	float alfa = 0.05; //serve ridimensionare l'offset tra due posizioni successive del mosue
	ypos = height - ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	xoffset *= alfa;
	yoffset *= alfa;
	Theta += xoffset;   //Aggiorno l'angolo Theta sommandogli l'offset della posizione x del mouse
	Phi += yoffset;  //Aggiorno l'angolo Phi sommandogli l'offset della posizione y del mouse 

	// Facciamo si' che l'angolo di Phi vari tra -90 e 90.
	if (Phi > 89.0f)
		Phi = 89.0f;
	if (Phi < -89.0f)
		Phi = -89.0f;

	//Ricavo le coordinate x,y,z del punto che sulla sfera individuato dagli angoli Theta e Phi
	vec3 front;
	front.x = cos(radians(Theta)) * cos(radians(Phi));
	front.y = sin(radians(Phi));
	front.z = sin(radians(Theta)) * cos(radians(Phi));
	//Considero la direzione normalizzata (nota la quarta componente uguale a 0 indica una direzione
	ViewSetup.direction = vec4(normalize(front), 0.0); //Aggiorno la direzione della telecamera
	ViewSetup.target = ViewSetup.position + ViewSetup.direction; //aggiorno il punto in cui guarda la telecamera

	glutPostRedisplay();
}

void keyboardEvent(unsigned char key, int x, int y)
{
	//cout << ViewSetup.position.x << " " << ViewSetup.position.y << " " << ViewSetup.position.z << endl;
	switch (key)
	{
	case 'a':
		ViewSetup.moveLeft();
		break;
	case 's':
		ViewSetup.moveBack();
		break;
	case 'd':
		ViewSetup.moveRight();
		break;
	case 'w':
		ViewSetup.moveForward();
		break;
	case 'q':
		ViewSetup.moveUp();
		break;
	case 'e':
		ViewSetup.moveDown();
		break;
	case 'c':
		selectedObj = (selectedObj + 1) % scene.size();
		printf("Selezionato %s [%d]\n", scene[selectedObj]->name.c_str(), selectedObj);
		break;
	case 'v':
		showAnchors = !showAnchors;
		break;
	case 'l':
		if(light.vector.w == 1) light.vector = { 0.0f, -1.0f, -1.0f, 0.0f };
		else light.vector = { -3.0f, 6.f, 1.0f, 1.0f };

	default:
		break;
	}

	glutPostRedisplay();
}

void main_menu_func(int option)
{
}

void material_menu_function(int option)
{
	if (selectedObj > -1) {
		switch (option) {
		case EMERALD:
			scene[selectedObj]->material = new Emerald();
			break;
		case BRASS:
			scene[selectedObj]->material = new Brass();
			break;
		case REDPLASTIC:
			scene[selectedObj]->material = new RedPlastic();
			break;
		case SNOWWHITE:
			scene[selectedObj]->material = new SnowWhite();
			break;
		case PINK:
			scene[selectedObj]->material = new Pink();
			break;
		case DARKWOOD:
			scene[selectedObj]->material = new DarkWood();
			break;
		case REDBRICK:
			scene[selectedObj]->material = new RedBrick();
			break;
		case STUCCO:
			scene[selectedObj]->material = new Stucco();
			break;
		case PARQUET:
			scene[selectedObj]->material = new Parquet();
			break;
		case FABRIC:
			scene[selectedObj]->material = new Fabric();
			break;


		}
	}
}

void shader_menu_function(int option) {
	if (selectedObj > -1)
		scene[selectedObj]->setShading((ShaderOption)option);
}

void buildOpenGLMenu()
{
	int materialSubMenu = glutCreateMenu(material_menu_function);

	glutAddMenuEntry("Emerald", MaterialOption::EMERALD);
	glutAddMenuEntry("Brass", MaterialOption::BRASS);
	glutAddMenuEntry("Red plastic", MaterialOption::REDPLASTIC);
	glutAddMenuEntry("Snow white", MaterialOption::SNOWWHITE);
	glutAddMenuEntry("Pink", MaterialOption::PINK);
	glutAddMenuEntry("Dark wood", MaterialOption::DARKWOOD);
	glutAddMenuEntry("Parquet", MaterialOption::PARQUET);
	glutAddMenuEntry("Red brick", MaterialOption::REDBRICK);
	glutAddMenuEntry("White stucco", MaterialOption::STUCCO);
	glutAddMenuEntry("White fabric", MaterialOption::FABRIC);

	int shaderSubMenu = glutCreateMenu(shader_menu_function);
	glutAddMenuEntry("None", ShaderOption::NONE);
	glutAddMenuEntry("Phong", ShaderOption::PHONG);
	glutAddMenuEntry("Blinn-Phong", ShaderOption::BLINNPHONG);
	glutAddMenuEntry("Gouraud", ShaderOption::GOURAUD);
	glutAddMenuEntry("Toon", ShaderOption::CARTOON);

	glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("", -1);
	//glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
	//glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
	glutAddSubMenu("Material", materialSubMenu);
	glutAddSubMenu("Shader", shaderSubMenu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void INIT_CAMERA_PROJECTION(void)
{
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(-3.0, 7.0, 41.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);


	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

void INIT_Illuminazione()
{
	//Setup della luce
	light.vector = { -3.0f, 6.f, 1.0f, 1.0f };
	//light.vector = { 0.0f, -1.0f, -1.0f, 0.0f };
	light.color = { 0.6,0.6,0.5 };
	light.power = 2.f;
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader.glsl";
	char* fragmentShader = (char*)"fragmentShader.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	vertexShader = (char*)"vertexShader_Cubemap.glsl";
	fragmentShader = (char*)"fragmentShader_Cubemap.glsl";

	programId1 = ShaderMaker::createProgram(vertexShader, fragmentShader);
	
	
	vertexShader = (char*)"vertexShader_riflessione.glsl";
	fragmentShader = (char*)"fragmentShader_riflessione.glsl";

	programIdr = ShaderMaker::createProgram(vertexShader, fragmentShader);
	
}

void buildScene() {
	Sphere* sphere;
	Torus* torus;
	Cone* cone;
	Pyramid* pyr;

	Composite* cubocomp;
	ObjMeshVector* guitar = new ObjMeshVector("Guitar 1", Meshdir + "guitar.obj");
	ObjMeshVector* guitar2 = new ObjMeshVector("Guitar 2", Meshdir + "guel.obj");
	ObjMeshVector* guitar3 = new ObjMeshVector("Guitar 3", Meshdir + "guel2.obj");

	ObjMeshVector* drawer = new ObjMeshVector("Drawer 1", Meshdir + "untitled.obj");
	ObjMeshVector* drawer2 = new ObjMeshVector("Drawer 2", Meshdir + "untitled.obj");
	ObjMeshVector* drums = new ObjMeshVector("Drum kit", Meshdir + "drumkit.obj");

	textureMuro = loadTexture((Texdir + "muromattoni.jpg").c_str(), 0);
	textureMarble = loadTexture((Texdir + "marble.jpg").c_str(), 0);
	textureDivano = loadTexture((Texdir + "fabric.jpg").c_str(), 0);
	texturePavimento = loadTexture((Texdir + "wood-floor.jpg").c_str(), 0);
	textureStucco = loadTexture((Texdir + "white-paint-stucco.jpg").c_str(), 0);
	textureMetal = loadTexture((Texdir + "metal.png").c_str(),0);


	cubemapTexture = loadCubemap(faces, 0);

	sky = new Cube("Skybox");

	sphere = new Sphere("Sphere", new RedPlastic());
	sphere->setShading(PHONG);
	sphere->translate(-6.0, 0, 0.0);
	sphere->texture = textureMarble;
	scene.push_back(sphere);

	torus = new Torus("Torus", new Emerald());
	torus->setShading(PHONG);
	torus->translate(-3.0, 0, -3.0);
	torus->texture = textureMetal;
	scene.push_back(torus);

	cone = new Cone("Cone", new Pink());
	cone->setShading(PHONG);
	cone->translate(0.0, 0, 2.0);
	cone->scale(1, -1, 1);
	cone->texture = textureMetal;
	scene.push_back(cone);

	pyr = new Pyramid("Pyramid", new Brass());
	pyr->setShading(PHONG);
	pyr->translate(-3, 0, 6);
	pyr->texture = textureMetal;
	//scene.push_back(pyr);
	
	PlaneSubdivided* pavement;
	pavement = new PlaneSubdivided("Pavimento", 128, { 1,1 }, new Parquet());
	pavement->setShading(BLINNPHONG);
	pavement->scale(40, 1, 50);
	pavement->translate(-3, -2, 0);
	pavement->texture = texturePavimento;
	scene.push_back(pavement);

	PlaneSubdivided* roof;
	roof = new PlaneSubdivided("Soffitto", 128, { 4,4 }, new Stucco());
	roof->setShading(BLINNPHONG);
	roof->scale(40, 1, 50);
	roof->translate(-3, 8.9, 0);
	roof->rotateX(180);
	roof->texture = textureStucco;
	scene.push_back(roof);

	PlaneSubdivided* wallBack;
	wallBack = new PlaneSubdivided("Muro dietro", 64, { 8,4 }, new RedBrick());
	wallBack->setShading(BLINNPHONG);
	wallBack->scale(40, 12, 1);
	wallBack->translate(-3, 3, -24);
	wallBack->rotateX(90);
	wallBack->texture = textureMuro;
	scene.push_back(wallBack);

	PlaneSubdivided* wallFront;
	wallFront = new PlaneSubdivided("Muro davanti", 64, { 8,4 }, new RedBrick());
	wallFront->setShading(BLINNPHONG);
	wallFront->scale(40, 12, 1);
	wallFront->translate(-3, 3, 20);
	wallFront->rotateX(-90);
	//wallFront->rotateY(180);
	wallFront->texture = textureMuro;
	//scene.push_back(wallFront);

	Composite* wallLeft = new Composite("Muro sx", new RedBrick());
	wallLeft->setShading(BLINNPHONG);
	wallLeft->scale(40, 12, 16);
	wallLeft->translate(-23, 3, -17);
	wallLeft->rotateX(90);
	wallLeft->rotateZ(-90);
	wallLeft->texture = textureMuro;

	PlaneSubdivided* wallLeft1;
	wallLeft1 = new PlaneSubdivided("Muro sx 1", 64, { 4,4 });
	PlaneSubdivided* wallLeft2;
	wallLeft2 = new PlaneSubdivided("Muro sx finestra sotto", 32, { 4,1.2 });
	wallLeft2->translate(-0.95, -0.0001, 0.35);
	wallLeft2->scale(1, 1, 0.3);
	PlaneSubdivided* wallLeft3;
	wallLeft3 = new PlaneSubdivided("Muro sx 2", 64, { 4,4 });
	wallLeft3->translate(-1.9, 0, 0);
	PlaneSubdivided* wallLeft4;
	wallLeft4 = new PlaneSubdivided("Muro sx finestra sopra", 32, { 4,0.4 });
	wallLeft4->translate(-0.95, 0, -0.45);
	wallLeft4->scale(1, 1, 0.1);
	wallLeft->add(wallLeft1);
	wallLeft->add(wallLeft2);
	wallLeft->add(wallLeft3);
	wallLeft->add(wallLeft4);
	scene.push_back(wallLeft);

	Cylinder* windowLeft1 = new Cylinder("Finestra sx 1", new Brass());
	windowLeft1->setShading(BLINNPHONG);
	windowLeft1->translate(-23, 0.7, -9);
	windowLeft1->rotateZ(90);
	windowLeft1->rotateY(90);
	windowLeft1->scale(0.1, 0.1, 14.6);
	Cylinder* windowLeft2 = new Cylinder("Finestra sx 2", new Brass());
	windowLeft2->setShading(BLINNPHONG);
	windowLeft2->translate(-23, 0.6, -9);
	windowLeft2->scale(0.1, 7.3, 0.1);
	Cylinder* windowLeft3 = new Cylinder("Finestra sx 3", new Brass());
	windowLeft3->setShading(BLINNPHONG);
	windowLeft3->translate(-23, 0.6, 5.7);
	windowLeft3->scale(0.1, 7.3, 0.1);
	Cylinder* windowLeft4 = new Cylinder("Finestra sx 4", new Brass());
	windowLeft4->setShading(BLINNPHONG);
	windowLeft4->translate(-23, 7.8, -9);
	windowLeft4->rotateZ(90);
	windowLeft4->rotateY(90);
	windowLeft4->scale(0.1, 0.1, 14.6);

	scene.push_back(windowLeft1);
	scene.push_back(windowLeft2);
	scene.push_back(windowLeft3);
	scene.push_back(windowLeft4);

	Composite* wallRight = new Composite("Muro dx", new RedBrick());
	wallRight->setShading(BLINNPHONG);
	wallRight->scale(40, 12, 16);
	wallRight->translate(16, 3, 14);
	wallRight->rotateX(90);
	wallRight->rotateZ(90);
	wallRight->texture = textureMuro;

	PlaneSubdivided* wallRight1;
	wallRight1 = new PlaneSubdivided("Muro sx 1", 64, { 4,4 });
	PlaneSubdivided* wallRight2;
	wallRight2 = new PlaneSubdivided("Muro sx finestra sotto", 32, { 4,1.2 });
	wallRight2->translate(-0.95, -0.0001, 0.35);
	wallRight2->scale(1, 1, 0.3);

	PlaneSubdivided* wallRight3;
	wallRight3 = new PlaneSubdivided("Muro sx 2", 64, { 4,4 });
	wallRight3->translate(-1.9, 0, 0);
	PlaneSubdivided* wallRight4;
	wallRight4 = new PlaneSubdivided("Muro sx finestra sopra", 32, { 4,0.4 });
	wallRight4->translate(-0.95, 0, -0.45);
	wallRight4->scale(1, 1, 0.1);

	wallRight->add(wallRight1);
	wallRight->add(wallRight2);
	wallRight->add(wallRight3);
	wallRight->add(wallRight4);
	scene.push_back(wallRight);

	Cylinder* windowRight1 = new Cylinder("Finestra dx 1", new Brass());
	windowRight1->setShading(BLINNPHONG);
	windowRight1->translate(16, 0.7, -9 + 0.3);
	windowRight1->rotateZ(90);
	windowRight1->rotateY(90);
	windowRight1->scale(0.1, 0.1, 14.6);
	Cylinder* windowRight2 = new Cylinder("Finestra dx 2", new Brass());
	windowRight2->setShading(BLINNPHONG);
	windowRight2->translate(16, 0.6, -9 + 0.3);
	windowRight2->scale(0.1, 7.3, 0.1);
	Cylinder* windowRight3 = new Cylinder("Finestra dx 3", new Brass());
	windowRight3->setShading(BLINNPHONG);
	windowRight3->translate(16, 0.6, 5.7 + 0.3);
	windowRight3->scale(0.1, 7.3, 0.1);
	Cylinder* windowRight4 = new Cylinder("Finestra dx 4", new Brass());
	windowRight4->setShading(BLINNPHONG);
	windowRight4->translate(16, 7.8, -9 + 0.3);
	windowRight4->rotateZ(90);
	windowRight4->rotateY(90);
	windowRight4->scale(0.1, 0.1, 14.6);

	scene.push_back(windowRight1);
	scene.push_back(windowRight2);
	scene.push_back(windowRight3);
	scene.push_back(windowRight4);

	cubocomp = new CubeComposite("Cube composite", new Fabric());
	cubocomp->setShading(PHONG);
	cubocomp->texture = textureDivano;
	cubocomp->translate(14, -1, 5);
	cubocomp->scale(3, 2, 10);
	scene.push_back(cubocomp);

	loadAssImp(guitar->filename.c_str(), guitar->model3D);
	guitar->translate(5, 1.5, -23);
	guitar->scale(7.5, 7.5, 7.5);
	guitar->build();
	scene.push_back(guitar);	
	loadAssImp(guitar2->filename.c_str(), guitar2->model3D);
	guitar2->translate(-5, 5, -23);
	guitar2->scale(2.4,2.4,2.4);
	guitar2->rotateZ(90);
	guitar2->rotateY(90);


	guitar2->build();
	scene.push_back(guitar2);
	loadAssImp(guitar3->filename.c_str(), guitar3->model3D);
	guitar3->translate(-15, 0.2, -18.2);
	guitar3->scale(2, 2, 2);
	guitar3->build();
	scene.push_back(guitar3);

	loadAssImp(drawer->filename.c_str(), drawer->model3D);
	drawer->rotateY(90);
	drawer->translate(-21, -2, -12);
	drawer->scale(1.3, 1.3, 1);
	drawer->build();
	scene.push_back(drawer);

	loadAssImp(drawer2->filename.c_str(), drawer2->model3D);
	drawer2->rotateY(90);
	drawer2->translate(-21, -2, -17);
	drawer2->scale(1.3, 1.3, 1);
	drawer2->build();
	scene.push_back(drawer2);

	loadAssImp(drums->filename.c_str(), drums->model3D);
	drums->rotateY(-30);
	drums->translate(10, -2, -13);
	drums->scale(0.05, 0.05, 0.05);
	drums->build();
	scene.push_back(drums);

	lightbulb = new Sphere("Light bulb", new SnowWhite());
	lightbulb->setShading(NONE);
	lightbulb->translate(light.vector.x, light.vector.y, light.vector.z);
	lightbulb->scale(0.1, 0.1, 0.1);	
	
}

void renderSkybox() {
	glDepthMask(GL_FALSE);
	glUseProgram(programId1);
	glUniform1i(glGetUniformLocation(programId1, "skybox"), 0);
	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));

	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));
	// skybox cube
	glBindVertexArray(sky->VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, sky->indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDepthMask(GL_TRUE);

}

void drawScene(void)
{
	//Utilizzo il program shader per il disegno

	glUseProgram(programId);

	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));

	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSkybox();

	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.

	//Passo al Vertex Shader il puntatore alla matrice View, che sar� associata alla variabile Uniform mat4 Projection
   //all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);
	
	glUseProgram(programIdr);
	for (int i = 0; i < mirrors.size(); i++) {
		glUniformMatrix4fv(MatrixProjR, 1, GL_FALSE, value_ptr(Projection));
		glUniformMatrix4fv(MatModelR, 1, GL_FALSE, value_ptr(mirrors[i]->Model));
		glUniformMatrix4fv(MatViewR, 1, GL_FALSE, value_ptr(View));
		glUniform3f(loc_view_posR, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
		glBindVertexArray(mirrors[i]->VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, (mirrors[i]->indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}


	//Cambio program per renderizzare tutto il resto della scena
	glUseProgram(programId);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Passo allo shader il puntatore a  colore luce, posizione ed intensit�
	glUniform4f(light_unif.light_vector_pointer, light.vector.x, light.vector.y, light.vector.z, light.vector.w);
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	//Passo allo shader il puntatore alla posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);

	for (int k = 0; k < scene.size(); k++)
	{
		scene[k]->show(lsceltaVS, lsceltaFS, light_unif, loc_texture, MatModel);
	}

	if (light.vector.w == 1) lightbulb->show(lsceltaVS, lsceltaFS, light_unif, loc_texture, MatModel);
	
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardEvent);
	//glutPassiveMotionFunc(my_passive_mouse);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glutTimerFunc(10, update, 0);
	glewExperimental = GL_TRUE;
	glewInit();


	//Inizializzazione setup illuminazione, materiali
	INIT_Illuminazione();
	//Inizializzazione setup Shader
	INIT_SHADER();
	//Inizializzazione VAO
	buildScene();
	//Inizializzazione setup telecamera
	INIT_CAMERA_PROJECTION();
	//Menu collegato al tasto centrale
	buildOpenGLMenu();

	//Abilita l'uso del Buffer di Profondit� per la gestione dell'eliminazione dlele superifici nascoste
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	MatrixProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	MatView = glGetUniformLocation(programId, "View");
	loc_time = glGetUniformLocation(programId, "time");
	lsceltaVS = glGetUniformLocation(programId, "sceltaVS");
	lsceltaFS = glGetUniformLocation(programId, "sceltaFS");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");
	loc_texture = glGetUniformLocation(programId, "id_tex");
	//location delle variabili uniformi per la gestione della luce
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");
	light_unif.light_vector_pointer = glGetUniformLocation(programId, "light.vector");
	//location delle variabili uniformi per la gestione dei materiali
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");

	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	MatViewS = glGetUniformLocation(programId1, "View");
	MatModelR = glGetUniformLocation(programIdr, "Model");
	MatViewR = glGetUniformLocation(programIdr, "View");
	MatrixProjR = glGetUniformLocation(programIdr, "Projection");
	loc_view_posR = glGetUniformLocation(programIdr, "ViewPos");
	loc_cubemapR = glGetUniformLocation(programIdr, "cubemap");
	
	glutMainLoop();
}