/*Programa que genera una matriz de spins aleatorios (-1 ó 1),
y que después cambia los valores de cada uno de los elementos si
se cumple que un numero aleatorio uniforme generado entre 0 y 1 (xi)
es menor que p, que se evalúa según la función minimo*/

const int N=64;//Tamaño de la red (cuadrada), variable global
double minimo(short int matriz[][N], double temp, int col, int fil); //Prototipo de la función minimo
void lectura(double &_temp, int &monte); //Prototipo de la función lectura
double magnetizacion(short int m[][N]); //Prototipo de la función magnetización

/*Comando de compilación
g++ -I /usr/include/gsl ising.cpp -o ising.exe -lm -lgsl -lgslcblas*/

/*BIBLIOTECAS*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
// Incluimos la gsl y su biblioteca de nº aleatorios
#include <gsl/gsl_rng.h>


using namespace std;

int main (void)
{
  //Declaración de variables
  short int spins[N][N];
  int i, j, rango=100000000, u, pasosMontecarlo, fila, columna, enecuadrado, k, semilla;
  double temperatura, p, xi, magnet;
  ofstream matriz_salida, magnetizacion_salida;

  //Variables para números aleatorios
  srand(time(NULL)); //Me da una semilla aleatoria que generará al nº aleatorio "semilla"
  semilla=rand(); // Semilla para el generador, con un valor aleatorio
  cout << "El valor de la semilla es: "<<semilla<<endl;
  const gsl_rng_type * rng_type; // Tipo de generador
  gsl_rng * rng; // Generador de número aleatorios

  // Inicialización del generador
  // ========================================
  gsl_rng_env_setup(); // Inicializa las variables internas de la biblioteca
  gsl_rng_default_seed = semilla; // Fija la semilla
  rng_type = gsl_rng_default; // Selecciona el tipo de generador por defecto
  rng = gsl_rng_alloc (rng_type); // Crea un generador de tipo rng_type

  //Lectura de datos
  lectura(temperatura, pasosMontecarlo);
  enecuadrado=pasosMontecarlo*N*N;

  //Abro los ficheros de escritura
  matriz_salida.open("columnas.txt");
  magnetizacion_salida.open("magnetizacion.txt");
  if(!(matriz_salida.is_open()||magnetizacion_salida.is_open())) cout << "Error al abrir un fichero \n\n\n";//Si no se abriese
  matriz_salida << "#En la primera columna tenemos la posición i de s[i][j] y en la segunda la posición j" << endl;
  matriz_salida << "#Guardamos solo las posiciones en las que hay un 1, en las que hay un -1 no se guarda"<<endl;

  //Formación inicial de la matriz de spines de manera aleatoria
  for(i=0;i<N;i++)
  {
      for(j=0;j<N;j++)
      {
          //Genero un numero aleatorio, u, en un rango muy grande
          u=gsl_rng_uniform_int(rng,rango);
          if (u<rango/2)//Si u cae aleatoriamente en la primera mitad del rango, entonces es 1
          {
            spins[i][j]=1;
            matriz_salida << i << "\t" << j << endl;
          }
          else spins[i][j]=-1; //Si no, entonces es -1
      }
  }
  matriz_salida <<endl<<endl;

  //IMPLEMENTO EL ALGORITMO
  int paso=20;
  for(i=1; i<=enecuadrado; i++)
  {
    //Obtengo una casilla de la matriz aleatoriamente
    fila=gsl_rng_uniform_int(rng,N);
    columna=gsl_rng_uniform_int(rng,N);
    //Obtengo el valor de p = min(1, exp −[∆E/T])
    p=minimo(spins, temperatura, columna, fila);
    //Genero el número aleatorio entre 0 y 1
    xi=gsl_rng_uniform(rng);
    //Cambio signo si xi<p
    if (xi<p) spins[fila][columna]*=-1;

    //Imprimo matriz cambiada (o no)
    for(j=0; j<N; j++)
    {
      for(k=0; k<N; k++)
      {
        if(spins[j][k]==1 && i/(N*N)==paso) //Cada 20 pasos montecarlo, imprime en el fichero
        {
         matriz_salida << j << "\t" << k << endl;
        }
      }
    }
    if(i/(N*N)==paso) matriz_salida <<endl << endl;

    if(i/(N*N)==paso) //Si ya he dado 20 pasos montecarlo analiza la magnetizacion y aumenta paso en 20
    {
        //Magnetización promedio:
        magnet=magnetizacion(spins);
        magnetizacion_salida << magnet <<endl;
        cout << "Ya he dado " << paso << " pasos montecarlo\n";
        paso=paso+20;
    }
  }

  //Cierro los ficheros
  matriz_salida.close();
  magnetizacion_salida.close();

  // Liberamos el espacio asignado al generador de números aleatorios
  gsl_rng_free (rng);
  return 0;
}


/*Funcion que lee los datos necesarios para el programa*/
void lectura(double &t, int &monte)
{
  do
  {
      cout << "Introduzca una temperatura entre 0 y 5: ";
      cin >> t;
      //Informo que el valor es erroneo para preguntarle de nuevo si es el caso
      if(t<0 || t>5) cout <<"\n\nValor erróneo, fuera del intervalo\n\n";
  } while(t<0 || t>5); //Pedir esto mientras de un valor erroneo de T fuera del intervalo [0,5]

  cout << "Introduzca cuantos pasos montecarlo desea dar: ";
  cin >> monte;
  return;
}


/*Función que calcula el min(1, exp −[∆E/T]).
La matriz no se pasa por referencia porque no quiero guardar los cambios*/
double minimo(short int matriz[][N], double temp, int col, int fil)
{
  //Declaración de variables locales
  short int E, fil_mas, fil_menos, col_mas, col_menos;
  double exponencial, mini;

  //Condiciones periódicas en las columnas
  //if-else para ver si estan en el borde izquierdo
  if (col==0) col_menos=matriz[fil][N-1];
  else col_menos=matriz[fil][col-1];
  //if-else para ver si estan en el borde derecho
  if (col==N-1) col_mas=matriz[fil][0];
  else col_mas=matriz[fil][col+1];

  //Condiciones periódicas en las filas
  //if-else para ver si estan en el borde de arriba
  if (fil==0) fil_menos=matriz[N-1][col];
  else fil_menos=matriz[fil-1][col];
  //if-else para ver si estan en el borde de abajo
  if (fil==N-1) fil_mas=matriz[0][col];
  else fil_mas=matriz[fil+1][col];


  //Cálculo de ∆E
  E=2.0*matriz[fil][col]*(fil_mas+fil_menos+col_mas+col_menos);
  exponencial=exp(-1.*E/temp);
  mini=min(1.0, exponencial);
  return mini;
}


/*Función que calcula la magnetización promedio del sistema*/
double magnetizacion(short int m[][N])
{
    //Declaración de variables
    double mag, suma;
    int i, j;

    suma=0.0;
    for(i=0;i<N;i++)
    {
      for(j=0;j<N;j++)
        suma+=m[i][j];
    }
    mag=(1.0/(N*N))*suma;
    return mag;
}
