#include "ObstacleRunner.h"
#include <time.h>
#include <GL/glut.h>

float moveprob[RUNNERVELOCITIES][RUNNERSHIFTS] =
            {{ 0.50, 0.90, 1.00, 0.00, 0.00, 0.00, 0.00, 0.00},
             { 0.10, 0.20, 0.55, 0.85, 0.95, 1.00, 0.00, 0.00},
             { 0.00, 0.00, 0.05, 0.20, 0.60, 0.80, 0.92, 1.00}};

float policydir [NUMBERDIRECTIONS]= {0.15,0.64,0.79,0.84,0.89,0.90,0.95,1.00};
float policyvel [RUNNERVELOCITIES]= {0.6,0.8,1.0};

map * currentMap = NULL;

GLfloat vert[8][3] = { { -0.7,-0.7,0.7 },{ 0.7,-0.7, 0.7 } ,{ 0.7,0.7,0.7 },{ -0.7,0.7,0.7 },
                       { -0.7,-0.7,-0.7 },{ 0.7,-0.7,-0.7 },{ 0.7,0.7,-0.7 },{ -0.7,0.7,-0.7 } };

GLfloat black[3] = {0.00,0.00,0.00};
GLfloat grey[3] = {0.75,0.75,0.75};
GLfloat green[3] = {0.00,1.00,0.00};
GLfloat red[3] = {1.00,0.00,0.00};
GLfloat lightblue[3] = {0.00,0.00,1.00};
GLfloat darkblue[3] = {0.00,0.00,0.40};

char * getNameDirection(int d){

    switch (d) {
      case 0: return "N";
      case 1: return "NE";
      case 2: return "E";
      case 3: return "SE";
      case 4: return "S";
      case 5: return "SW";
      case 6: return "W";
      case 7: return "NW";
    }
    return "Unknown";
}
char * getNameDistance(int d){

    switch (d) {
      case 0: return "Close";
      case 1: return "Normal";
      case 2: return "Far";
    }
    return "Unknown";
}
char * getNameVelocity(int v){

    switch (v) {
      case 0: return "Slow";
      case 1: return "Normal";
      case 2: return "Quick";
    }
    return "Unknown";
}

action::action() : dir(0), vel(0) {
}
void action::setAction(int d, int v) {

    dir = d;;
    vel = v;
}
int action::getPolicyDirection() {

  int i;

  float val = randomFloatValue(0,1);

  for (i=0; i < NUMBERDIRECTIONS; i++)
    if (val < policydir[i])
        return i;
}
int action::getPolicyVelocity() {
  int i;

  float val = randomFloatValue(0,1);

  for (i=0; i < RUNNERVELOCITIES; i++)
    if (val < policyvel[i])
        return i;
}
void action::print(){
    printf("Action:\n\tMove Dir=%s\n\tVelocity=%s\n\n",
            getNameDirection(dir), getNameVelocity(vel));
}

state::state() : left(0), front(0), right(0){
}
void state::setState(int d, int t, int l, int m, int r) {
    dir = d;
    left = l;
    front = m;
    right = r;
    target = t;
}

int state::leftDir(int d){
    return (d + 7) % 8;
}
int state::rightDir(int d){
    return (d + 1) % 8;
}
void state::print(){
    printf("State:\n\tMove Dir=%s\n\tDist Target=%s\n\tDist Left=%s\n\tDist Front=%s\n\tDist Right=%s\n\n",
            getNameDirection(dir), getNameDistance(target), getNameDistance(left),
            getNameDistance(front), getNameDistance(right) );
}


map::map(int dl, int x, int y) : end(0), level(dl), mult(1), xsz(x), ysz(y) {

  xsz = (xsz < DEFAULTMINIMUNSIZE) ? DEFAULTMINIMUNSIZE : xsz;
  ysz = (ysz < DEFAULTMINIMUNSIZE) ? DEFAULTMINIMUNSIZE : ysz;


  mult = ((float)xsz * ysz)/(DEFAULTSIZE*DEFAULTSIZE);

  switch (level) {
    case DIFICULTLEVELDIF : wall10 = DLEASYWALL10 * DIFWALLMULT * mult;
                            wall06 = DLEASYWALL06 * DIFWALLMULT * mult;
                            wall03 = DLEASYWALL03 * DIFWALLMULT * mult;
                            wall01 = DLEASYWALL01 * DIFWALLMULT * mult;
                            break;
    case DIFICULTLEVELAVG : wall10 = DLEASYWALL10 * AVGWALLMULT * mult;
                            wall06 = DLEASYWALL06 * AVGWALLMULT * mult;
                            wall03 = DLEASYWALL03 * AVGWALLMULT * mult;
                            wall01 = DLEASYWALL01 * AVGWALLMULT * mult;
                            break;
    case DIFICULTLEVELEASY:
    default               : wall10 = DLEASYWALL10 * mult;
                            wall06 = DLEASYWALL06 * mult;
                            wall03 = DLEASYWALL03 * mult;
                            wall01 = DLEASYWALL01 * mult;
                            break;

  }


  int i, j, xi, yi, dir, sz, n;

  maparray = new int *[ysz];
  for(i=0; i < ysz; i++) {
     maparray[i] = new int[xsz];
     for (j=0; j < xsz; j++)
         maparray[i][j] = 0;
  }

  for(i=0; i < 4; i++) {
    switch (i) {
        case 0: sz = 10; n = wall10; break;
        case 1: sz = 06; n = wall06; break;
        case 2: sz = 03; n = wall03; break;
        case 3: sz = 01; n = wall01; break;
    }
    for (j = 0; j < n; j++) {
        xi = randomIntValue(0,xsz-1);
        yi = randomIntValue(0,ysz-1);
        dir = randomIntValue(0,NUMBERDIRECTIONS-1);
        while (!testOk(yi,xi,dir,sz)) {
            xi = randomIntValue(0,xsz-1);
            yi = randomIntValue(0,ysz-1);
            dir = randomIntValue(0,NUMBERDIRECTIONS-1);
        }
        createWall(yi,xi,dir,sz);
    }

  }

  initializeRunner();
  initializeObjective();
  initializeState();
}

map::~map() {

  int i;

  for(i=0; i < ysz; i++)
     delete [] maparray[i];
  maparray = NULL;

}

void map::setCurrentMap() {
    currentMap = this;
}

int map::getXShift(int d) {
    switch (d) {
        case 0:
        case 4: return 0;
        case 1:
        case 2:
        case 3: return 1;
        case 5:
        case 6:
        case 7: return -1;
    }
}

int map::getYShift(int d) {
    switch (d) {
        case 2:
        case 6: return 0;
        case 3:
        case 4:
        case 5: return 1;
        case 0:
        case 1:
        case 7: return -1;
    }

}
int map::distanceLevel(int d) {
    if (d < 3)
        return 0;
    if (d < 10)
        return 1;
    return 2;
}
int map::getWallDistance(int y, int x, int d) {
    int xshift,yshift;
    int i;

    xshift = getXShift(d);
    yshift = getYShift(d);

    i=0;

    int j,k;
    j=y; k=x;;

    for (i=0; ; i++) {

        if (j < 0 || j >= ysz)
            return i-1;
        if (k < 0 || k >= xsz)
            return i-1;
        if (maparray[j][k] > 0)
            return i;
        switch (d) {
          case 1: if (j > 1 && maparray[j-1][k] > 0 && k < xsz-1 && maparray[j][k+1] > 0)
                      return i;
                  break;
          case 3: if (j < ysz-1 && maparray[j+1][k] > 0 && k < xsz-1 && maparray[j][k+1] > 0)
                      return i;
                  break;
          case 5: if (j < ysz-1 && maparray[j+1][k] > 0 && k > 1 && maparray[j][k-1] > 0)
                      return i;
                  break;
          case 7: if (j > 1 && maparray[j-1][k] > 0 && k > 1 && maparray[j][k-1] > 0)
                      return i;
                  break;
          defaul: break;
        }
        j+=yshift;
        k+=xshift;

    }

}
bool map::testOk(int y, int x, int d, int sz) {
    switch (d) {
        case 0: if (y - sz >= 0)
                    return true;
                break;
        case 1: if (x + sz < xsz && y - sz >= 0)
                    return true;
                break;
        case 2: if (x + sz < xsz)
                    return true;
                break;
        case 3: if (x + sz < xsz && y + sz < ysz)
                    return true;
                break;
        case 4: if (y + sz < ysz)
                    return true;
                break;
        case 5: if (x - sz >= 0 && y + sz < ysz)
                    return true;
                break;
        case 6: if (x - sz >= 0)
                    return true;
                break;
        case 7: if (x - sz >= 0 && y - sz >= 0)
                    return true;
                break;
    }

    return false;

}
bool map::success(int y, int x, int d, int sz) {
  int xshift,yshift;
  int i;

  xshift = getXShift(d);
  yshift = getYShift(d);

  for (i=0; i < sz; i++)
    if ( xrun + i * xshift == currentMap->getXSize()-1 && yrun + i * yshift == 0)
      return true;
  return false;
}
void map::markSuccess(int y, int x) {
  int i;

  maparray[0][xsz-3] = maparray[0][xsz-2] = maparray[0][xsz-1]
                     = maparray[1][xsz-1] = maparray[2][xsz-2]
                     = maparray[2][xsz-1] = maparray[3][xsz-1] = -9;
}
bool map::explosion(int y, int x, int d, int sz) {

    if (sz >= getWallDistance(y, x, d))
        return true;
    return false;
}
void map::markExplosion(int y, int x) {
  int i;

  // printf("marca explosao\n");
  if (testOk(y,x,0,2))
      maparray[y-1][x] = maparray[y-2][x] = -9;
  else if (testOk(y,x,0,1))
      maparray[y-1][x] = -9;
  if (testOk(y,x,1,2))
      maparray[y-1][x+1] = maparray[y-2][x+2] = -9;
  else if (testOk(y,x,1,1))
      maparray[y-1][x+1] = -9;
  if (testOk(y,x,2,2))
      maparray[y][x+1] = maparray[y][x+2] = -9;
  else if (testOk(y,x,2,1))
      maparray[y][x+1] = -9;
  if (testOk(y,x,3,2))
      maparray[y+1][x+1] = maparray[y+2][x+2] = -9;
  else if (testOk(y,x,3,1))
      maparray[y+1][x+1] = -9;
  if (testOk(y,x,4,2))
      maparray[y+1][x] = maparray[y+2][x] = -9;
  else if (testOk(y,x,4,1))
      maparray[y+1][x] = -9;
  if (testOk(y,x,5,2))
      maparray[y+1][x-1] = maparray[y+2][x-2] = -9;
  else if (testOk(y,x,5,1))
      maparray[y+1][x-1] = -9;
  if (testOk(y,x,6,2))
      maparray[y][x-1] = maparray[y][x-2] = -9;
  else if (testOk(y,x,6,1))
          maparray[y][x-1] = -9;
  if (testOk(y,x,7,2))
      maparray[y-1][x-1] = maparray[y-2][x-2] = -9;
  else if (testOk(y,x,7,1))
      maparray[y-1][x-1] = -9;
}
void map::createWall(int y, int x, int d, int sz) {

  int xshift,yshift;
  int i,j,k;

  xshift = getXShift(d);
  yshift = getYShift(d);

  j=k=0;
  for(i=0; i < sz; i++) {
    maparray[y+j][x+k] = sz;
    j+=yshift;
    k+=xshift;
  }
}
void map::initializeState() {

    int dl = cur.leftDir(drun);
    int dr = cur.rightDir(drun);


    cur.setState(drun, targetDistance(yrun, xrun),
                       distanceLevel(getWallDistance(yrun, xrun, dl)),
                       distanceLevel(getWallDistance(yrun, xrun, drun)),
                       distanceLevel(getWallDistance(yrun, xrun, dr)));

}
state map::getNextState(action ac) {

  state s;
  int v = ac.getVelocity();
  int x,y, shift;

  switch (v) {
    case 0: shift = 2;
    case 1: shift = 3;
    case 2: shift = 5;
  }
  int d = ac.getDirection();

  int xshift = getXShift(d);
  int yshift = getYShift(d);

  y = yrun + yshift * shift;
  x = xrun + xshift * shift;

  int dl = cur.leftDir(d);
  int dr = cur.rightDir(d);

  s.setState(d, targetDistance(y, x),
            distanceLevel(getWallDistance(y, x, dl)),
            distanceLevel(getWallDistance(y, x, d)),
            distanceLevel(getWallDistance(y, x, dr)));

  return s;
}
void map::initializeRunner() {

    xrun = 0;
    yrun = ysz-1;
    drun = 1;
}
int map::targetDistance(int y, int x) {

   int v = max(xsz-x-1,y);
   if (v <= 5)
    return 0;
   if (v <= 15)
    return 1;
   return 2;
}
bool map::onRunner(int y, int x) {

   if (x == xrun && y == yrun)
        return true;
   return false;
}
bool map::onRunnerPath(int y, int x) {

    switch (drun) {
        case 0: if (x == xrun && y <= yrun && y >= yrun - RUNNERPATHSIZE + 1)
                    return true;
                break;
        case 1: if (x >= xrun && x <= xrun + RUNNERPATHSIZE - 1 && y <= yrun && y >= yrun - RUNNERPATHSIZE + 1 && x - xrun == yrun - y)
                    return true;
                break;
        case 2: if (x >= xrun && x <= xrun + RUNNERPATHSIZE - 1 && y == yrun)
                    return true;
                break;
        case 3: if (x >= xrun && x <= xrun + RUNNERPATHSIZE - 1 && y >= yrun && y <= yrun + RUNNERPATHSIZE - 1 && x - xrun == y - yrun)
                    return true;
                break;
        case 4: if (x == xrun && y >= yrun && y <= yrun + RUNNERPATHSIZE - 1)
                    return true;
                break;
        case 5: if (x <= xrun && x >= xrun - RUNNERPATHSIZE + 1 && y >= yrun && y <= yrun + RUNNERPATHSIZE - 1 && xrun - x == y - yrun)
                    return true;
                break;
        case 6: if (x <= xrun && x >= xrun - RUNNERPATHSIZE + 1 && y == yrun)
                    return true;
                break;
        case 7: if (x <= xrun && x >= xrun - RUNNERPATHSIZE + 1 && y <= yrun && y >= yrun - RUNNERPATHSIZE + 1 && xrun - x == yrun - y)
                    return true;
                break;
        }
    return false;
}
void map::initializeObjective() {

    xobj = xsz -1;
    yobj = 0;
}
bool map::move(action ac) {
    int i = ac.getVelocity();
    int j;
    int shift;
    float val = randomFloatValue(0.00, 1.00);
    for(j=0; j < RUNNERSHIFTS; j++)
        if (val <= moveprob[i][j]) {
            shift = j+1;
            break;
        }
    drun = ac.getDirection();

    if (success(yrun,xrun,drun,shift)) {
        end = 2;
        markSuccess(yrun,xrun);
        return true;
    }
    if (explosion(yrun,xrun,drun,shift)){
        end = 1;
        markExplosion(yrun,xrun);
        return false;
      }
    int xshift = getXShift(drun);
    int yshift = getYShift(drun);
//    printf("shift=%d x=%d y=%d",shift,xshift,yshift);
    for (i=0; i < shift; i++) {
      maparray[yrun][xrun] = -1;
      yrun+=yshift;
      xrun+=xshift;
    }
    int dl = cur.leftDir(drun);
    int dr = cur.rightDir(drun);

    cur.setState(drun, targetDistance(yrun, xrun),
              distanceLevel(getWallDistance(yrun, xrun, dl)),
              distanceLevel(getWallDistance(yrun, xrun, drun)),
              distanceLevel(getWallDistance(yrun, xrun, dr)));
    // print();
    // cur.print();
    return true;

}
void map::print() {

    int i,j;

    printf("     ");
    for (j=0; j < xsz; j=j+10)
        printf("%d          ",j / 10);
    printf("\n     ");
    for (j=0; j < xsz; j++)
            printf("%d",j % 10);
    printf("\n     ");
    for (j=0; j < xsz; j++)
            printf("-");
    printf("\n");
    for (i=0; i < ysz; i++) {
        printf("%03d |",i);
        for (j=0; j < xsz; j++) {
            if (onRunner(i,j))
                printf("#");
            else if (onRunnerPath(i,j))
                printf("o");
            else if (maparray[i][j] == 0)
                printf(" ");
            else if (maparray[i][j] < 0)
                printf(".");
            else
                 printf("*");
        }
        printf("| %03d\n",i);
    }
    printf("     ");
    for (j=0; j < xsz; j++)
            printf("-");
    printf("\n     ");
    for (j=0; j < xsz; j++)
            printf("%d",j % 10);
    printf("\n     ");
    for (j=0; j < xsz; j=j+10)
        printf("%d          ",j / 10);
    printf("\n");

}
void map::initializeMap(int argc, char **argv) {

  glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(DEFAULTWIDTH, DEFAULTHEIGHT);
	glutInitWindowPosition(10,10);
	glutCreateWindow("Map");
	glutDisplayFunc(desenha);
  glutTimerFunc(40,process,1);
  //glutIdleFunc(process);
	inicializa();
	glutMainLoop();
}

void add(GLfloat v[3], GLfloat v1[3],int x, int y) {
  v[0] = v1[0]+x;
  v[1] = v1[1]+y;
}

void cubo(GLfloat color[3], int x, int y) {
  quadrado(color, x, y, vert[7], vert[6], vert[5], vert[4]); // fundos
  quadrado(color, x, y, vert[0], vert[1], vert[5], vert[4]); // base
  quadrado(color, x, y, vert[1], vert[5], vert[6], vert[2]); // dir
  quadrado(color, x, y, vert[0], vert[3], vert[7], vert[4]); // esq
  quadrado(color, x, y, vert[3], vert[2], vert[6], vert[7]); // topo
  quadrado(color, x, y, vert[0], vert[1], vert[2], vert[3]); // frente
  quadrado(color, x, y, vert[7], vert[6], vert[5], vert[4]); // fundos
}

void quadrado(GLfloat color[3], int x, int y, GLfloat v1[3],
              GLfloat v2[3], GLfloat v3[3], GLfloat v4[3]) {
  GLfloat v[3];
  glColor3fv(color);
  add(v,v1,x,y);
  glVertex3fv(v);
  add(v,v2,x,y);
  glVertex3fv(v);
  add(v,v3,x,y);
  glVertex3fv(v);
  add(v,v4,x,y);
  glVertex3fv(v);
}

void desenha(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  int i, j, xsz, ysz;

  xsz = currentMap->getXSize()-1;
  ysz = currentMap->getYSize()-1;

 int cnt;
  glBegin(GL_QUADS);
  for(i=0; i < xsz; i++)
    for (j=0; j < ysz; j++) {
      cnt = currentMap->getPositionContent(i,j);
      if (cnt == -9)
        if (currentMap->succeeded())
          cubo(green,2*i,2*(ysz-j));
        else
          cubo(red,2*i,2*(ysz-j));
      else if (i==xsz && j==0)
        cubo(green,2*i,2*(ysz-j));
      else if(currentMap->onRunner(j,i))
        cubo(darkblue,2*i,2*(ysz-j));
      else {
        if (cnt > 0)
          cubo(black,2*i,2*(ysz-j));
        else if (cnt == -1)
            cubo(lightblue,2*i,2*(ysz-j));
        else
            cubo(grey,2*i,2*(ysz-j));
      }
   }
  glEnd();
  glutSwapBuffers();
	glFinish();
}
void inicializa(void)
{
	// Define a cor de fundo da janela de visualiza??o como preta
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  gluOrtho2D(-20, 2*currentMap->getXSize()+20, -20, 2*currentMap->getYSize()+20);

	glEnable(GL_DEPTH_TEST);
}
