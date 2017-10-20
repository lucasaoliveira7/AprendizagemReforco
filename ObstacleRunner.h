#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#define DIFICULTLEVELEASY   0
#define DIFICULTLEVELAVG    1
#define DIFICULTLEVELDIF    2

#define DLEASYWALL20              1
#define DLEASYWALL10              2
#define DLEASYWALL06              5
#define DLEASYWALL03             10
#define DLEASYWALL01             20
#define AVGWALLMULT               3
#define DIFWALLMULT               8

#define RUNNERVELOCITIES         3
#define RUNNERSHIFTS             8
#define NUMBERDIRECTIONS         8

#define DEFAULTSIZE              70
#define DEFAULTMINIMUNSIZE       10
#define RUNNERPATHSIZE            5

#define DEFAULTWIDTH            700
#define DEFAULTHEIGHT           700


#define max(x,y) ( x > y ? x : y)
#define min(x,y) ( x < y ? x : y)
inline int randomIntValue(int lower , int upper )
                { int val = rand();
                  if (val == RAND_MAX)
                       return upper;
                  return (int)(lower + (float)val / RAND_MAX * (upper - lower + 1));
                  }

inline float randomFloatValue(float lower , float upper )
                { int val = rand();
                  if (val == RAND_MAX)
                       return upper;
                  return (lower + (float)val / RAND_MAX * (upper - lower));
                  }

//void process(void);
void process(int);
void desenha(void);
void inicializa(void);
void quadrado(GLfloat [3], int, int, GLfloat [3], GLfloat [3], GLfloat [3], GLfloat [3])
;
void cubo(GLfloat [3], int, int);
char * getNameVelocity(int );
char * getNameDirection(int);
char * getNameDistance(int);

class action {
public:
    action();
    ~action(){};
    void setAction(int,int);
    int getPolicyDirection();
    int getPolicyVelocity();
    int getVelocity() { return vel;};
    int getDirection() { return dir;};
    void print();

private:
    int dir;
    int vel;



};

class state {
public:
    state();
    ~state(){};
    int leftDistance() { return left;};
    int rightDistance() { return right;};
    int frontDistance() { return front;};
    int targetDistance() { return target;};
    void setState(int,int,int,int,int);
    int rightDir(int);
    int leftDir(int);
    void print();

private:
    int dir, target, left, front, right;


};

class map {
public:
    map(int = DIFICULTLEVELEASY, int = DEFAULTSIZE, int = DEFAULTSIZE);
    ~map();
    state getState() { return cur;};
    state getNextState(action);
    bool move(action);
    void setCurrentMap();
    void print();
    int getPositionContent(int x, int y) { return maparray[y][x];};
    int getXSize() { return xsz;};
    int getYSize() { return ysz;};
    bool onRunner(int,int);
    void initializeMap(int, char**);
    bool exploded() { return (end == 1);};
    bool succeeded() { return (end == 2);};

private:
    int level;
    float mult;
    int xsz, ysz;
    int wall10, wall06, wall03, wall01;
    int ** maparray;
    int xrun, yrun, drun;
    int xobj, yobj;
    state cur;
    int end;

    bool testOk(int, int, int, int);
    void createWall(int, int, int, int);
    void initializeRunner();
    void initializeObjective();
    void initializeState();
    bool onRunnerPath(int,int);
    int getXShift(int);
    int getYShift(int);
    int getWallDistance(int, int, int);
    bool success(int, int, int, int);
    bool explosion(int, int, int, int);
    int distanceLevel(int);
    void markSuccess(int,int);
    void markExplosion(int,int);
    int targetDistance(int,int);

};
