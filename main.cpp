#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "ObstacleRunner.h"
#define MAX_STATEFUT 24
#define MAX_STATE 648
typedef struct reg_t{
    state sts;
    int id;
    double value;
}reg_t;
typedef struct fila_t{
    reg_t *V ;
}fila_t;

fila_t *fila_st = NULL;
fila_t *fila_prox = NULL;
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
    printf("posicion %d %d \n",d, v );
    return ac;

}
fila_t *begin_f(fila_t *fila, int max){
  fila =(fila_t *)malloc(sizeof(fila_t)*1);
  fila->V =  (reg_t *)malloc(sizeof(reg_t)*max);

  for(int i = 0; i < max; i++){
    fila->V[i].sts = state();
    fila->V[i].id = 0;
    fila->V[i].value = 0.0;
  }
  return fila;
}
int cont =0; // conta rodadas
void process(int){

  st = corredor->getState();
  //coloca o estado do programa na fila de estados
  fila_st->V[cont].sts = st;
  fila_st->V[cont].id = cont;
  //repete para cada proximo estado
  //maximo d estados são 24
 // while(1){
      //escolho uma ação dada o estado
    //  ac = chooseAction(st);
     // break;
  //}
  printf("estado \n ");
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
    //fila para estados do programa
    fila_st = begin_f(fila_st, MAX_STATEFUT);
    //fila para proximos estados a partir do estado s
    fila_prox = begin_f(fila_prox,MAX_STATE);

    corredor = new map(DIFICULTLEVELEASY);
    corredor->setCurrentMap();
    maps = 1;
    corredor->initializeMap(argc, argv);

    free(fila_prox);
    free(fila_st);
    return 0;
}
