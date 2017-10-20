#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "ObstacleRunner.h"

map *corredor=NULL;
state st;
action ac;
int maps=0;
int acs=0;
int boom=0;
int suc=0;

action chooseAction(state st) {
    int d = ac.getPolicyDirection();
    int v = ac.getPolicyVelocity();
    ac.setAction(d,v);

    return ac;

}
void process(int){


  st = corredor->getState();
  st.print();
  ac = chooseAction(st);
  acs++;
  ac.print();
  corredor->move(ac);
  if (corredor->succeeded()) {
    suc++;
    printf("Sucesso\n");
  }
  if (corredor->exploded()) {
    boom++;
    printf("Explodiu\n");
  }
  desenha();
  if (corredor->succeeded() || corredor->exploded()) {
    corredor->print();
    delete corredor;
    acs=0;
    if (maps >= 100) {
      printf("Resumo:\n\tSucessos=%d\n\tExplosoes=%d\n",suc,boom);
      exit(EXIT_SUCCESS);
    }
    corredor = new map(DIFICULTLEVELEASY);
    corredor->setCurrentMap();
    maps++;
  }

  // Estes comandos devem sempre terminar a função
  // Pode trocar o valor do tempo se quizer que programa
  // rode mais rápido ou lentamente
  glutPostRedisplay();
  glutTimerFunc(100,process,1);

}

int main(int argc, char **argv)
{
    srand (time(NULL));
    corredor = new map(DIFICULTLEVELEASY);
    corredor->setCurrentMap();
    maps = 1;
    corredor->initializeMap(argc, argv);

    return 0;
}
