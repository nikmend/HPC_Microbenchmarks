# HPC_Microbenchmarks
Study case for matrix multiplication using High performance computing

## Introducción

La multiplicación de matrices es una operación fundamental en muchas aplicaciones científicas y de ingeniería, incluidos los gráficos por computadora, el procesamiento de imágenes y el aprendizaje automático. Como la multiplicación de matrices implica numerosos cálculos, puede ser computacionalmente intensivo y llevar mucho tiempo para matrices grandes. La computación paralela de OpenMP se puede utilizar para acelerar la multiplicación de matrices mediante la distribución de la carga de trabajo entre varios subprocesos. Este enfoque permite el uso eficiente de varios núcleos dentro de una arquitectura de memoria compartida, lo que da como resultado tiempos de ejecución más rápidos y un rendimiento mejorado. Paralelizar la multiplicación de matrices usando OpenMP puede reducir significativamente el tiempo de procesamiento requerido para cálculos de matrices grandes, lo que permite un procesamiento de datos más rápido y eficiente en una variedad de aplicaciones. OpenMP proporciona una manera simple y eficaz de paralelizar la multiplicación de matrices y es una herramienta valiosa para mejorar el rendimiento de las aplicaciones informáticas intensivas. 


## Tabla de Contenidos
* [Autores](#autores)
* [Objetivo](#objetivo)
* [Recursos](#recursos)
* [Descripción de la solución](#descripción-de-la-solución)
* [Analisis de resultados](#descripción-de-la-solución)

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

## Descripción de la solución

### Diseño experimental

Se realizaron un total de 3600 experimentos en los que se probaron algoritmos de multiplicación de matrices variando los siguientes parametros:

| Parametro   | Descripción | Niveles | 
|----------|-------------|-------------|
| Algoritmo de multiplicación | Variación de la forma de apuntar a los datos | MM1F,MM1FU,MM2F |
| Tamaño de las matrices | Corresponde al tamaño de la matriz cuadrada | 1000,2000,4000,8000 |
| Número de threads empleados | Número de threads empleados en el nodo trabajador | 1,2,4,8,10,12,14,16,18,20 |
| Métodos | Métodos de ejecución | MPI, OpenMP |

### Algoritmos
#### MM1F

Añadir imagen 

#### MM1FU

Añadir imagen 
#### MM2F

Añadir imagen 

### Métodos de ejecución
#### MPI
#### OpenMP

## Analisis de resultados







