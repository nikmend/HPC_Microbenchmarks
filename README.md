# HPC_Microbenchmarks
Study case for matrix multiplication using High performance computing

## Introducción

La multiplicación de matrices es una operación fundamental en muchas aplicaciones científicas y de ingeniería, incluidos los gráficos por computadora, el procesamiento de imágenes y el aprendizaje automático. Como la multiplicación de matrices implica numerosos cálculos, puede ser computacionalmente intensivo y llevar mucho tiempo para matrices grandes. La computación paralela de OpenMP se puede utilizar para acelerar la multiplicación de matrices mediante la distribución de la carga de trabajo entre varios subprocesos. Este enfoque permite el uso eficiente de varios núcleos dentro de una arquitectura de memoria compartida, lo que da como resultado tiempos de ejecución más rápidos y un rendimiento mejorado. Paralelizar la multiplicación de matrices usando OpenMP puede reducir significativamente el tiempo de procesamiento requerido para cálculos de matrices grandes, lo que permite un procesamiento de datos más rápido y eficiente en una variedad de aplicaciones. OpenMP proporciona una manera simple y eficaz de paralelizar la multiplicación de matrices y es una herramienta valiosa para mejorar el rendimiento de las aplicaciones informáticas intensivas. 


## Tabla de Contenidos
* [Autores](#autores)
* [Objetivo](#objetivo)
* [Recursos](#recursos)
* [Descripción de la solución](#descripcion-de-la-solucion)
* [Analisis de resultados](#analisis-de-resultados)

## Autores
A continuación se presenta el listado de autores y su respectivo correo electronico.

| Organización   | Nombre del Miembro | Correo electronico | 
|----------|-------------|-------------|
| PUJ-Bogota | Sebastián Pineda| juanspineda@javeriana.edu.co|
| PUJ-Bogota  |  Camilo Cano | c-cano@javeriana.edu.co |
| PUJ-Bogota  |  Daniel Duque   | daniel_duque@javeriana.edu.co | 
| PUJ-Bogota  |  Nicolas Mendez   | nicolas_mendez@javeriana.edu.co |


## Objetivo
Analizar el impacto de la paralelización de los algoritmos, evaluando el rendimiento de los mismos al variar la cantidad de hilos  

## Recursos 
Todo el experimento se llevó a cabo en una sola maquina con las siguientes especificaciones 

![imagen](https://github.com/jpined93/HPC_Microbenchmarks/assets/101982334/6b47b867-21d9-405e-98c0-608d55c9bfdb)

## Descripción de la solución

### Diseño experimental

Para el estudio del benchmarking de los procesos OpenMP Y MPI, se realizarán análisis de anovas, como forma de contrastar influencias de las variables seleccionadas, dado que son demasiados niveles y deben ser evaluados al tiempo. No se optó por un diseño experimental de cuadro latino o 2^n. Se toman por muestra por nivel de 30 repeticiones, esto se hace con la finalidad de poder hacer un análisis por regresión y tener una cantidad de puntos deseables para revisar supuestos de normalidad.  

Como resultado se plantean un total de 3600 experimentos, estos fueron repartidos de la siguiente manera: 

| Parametro   | Descripción | Niveles | 
|----------|-------------|-------------|
| Algoritmo de multiplicación | Variación de la forma de apuntar a los datos | MM1F,MM1FU,MM2F |
| Tamaño de las matrices | Corresponde al tamaño de la matriz cuadrada | 1000,2000,4000,8000 |
| Número de threads empleados | Número de threads empleados en el nodo trabajador | 1,2,4,8,10,12,14,16,18,20 |
| Métodos | Métodos de ejecución | MPI, OpenMP |

### Algoritmos
#### MM1F

Nos centraremos en los siguientes fragmentos que son los que cambian entre los diferentes códigos. Tomamos como línea de base el MM1f en donde vemos que las matrices se multiplican por la iteración respectiva de los valores uno por uno en el loop que recorre las matrices y agrega los datos que encuentra en valores intermedios, luego en otro loop los agrega en una variable S, y por último reemplaza el valor S de esa operación en la ubicación correspondiente en la matriz de resultados. Por supuesto esto se hace para todas las operaciones necesarias en la iteración.  

![imagen](https://github.com/jpined93/HPC_Microbenchmarks/assets/101982334/e1495328-41b8-43fa-a540-aa2f3167f35c)


#### MM1FU

En esta operación nos encontramos con una estructura similar a la anterior, la diferencia que notamos es que en lugar de hacerlo un dato a la vez hace la operación con cuatro valores continuos, si estamos en lo correcto, esto permitiría acceder de manera más sencilla a la memoria disponible dentro del procesador, pues es más probable que los valores contiguos estén ya preparados para usar. 

![imagen](https://github.com/jpined93/HPC_Microbenchmarks/assets/101982334/df655b29-aa63-44f3-ad89-25849ab69184)

#### MM2F

![imagen](https://github.com/jpined93/HPC_Microbenchmarks/assets/101982334/b4ab14c2-bb25-42a7-8c77-91403b070845)

### Métodos de ejecución
#### MPI
#### OpenMP

## Analisis de resultados







