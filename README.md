# Simulador de caché
Implementación por Francisco Segura, Facundo Cardogna y Sofía Alvarado

En este trabajo práctico se implementó un **simulador de caché parametrizado**. Con la posiblidad de configurar las características de la caché simulada (tamaño, número de sets y asociatividad).
El programa principal lee y simula una secuencia de operaciones desde un archivo de trazas.

## Línea de comandos

La interfaz del programa en la línea de comandos es:

```
$ ./cachesim tracefile.xex C E S [ -v n m ]
```

Los cuatro primeros argumentos son:

1.  el archivo de traza a simular
2.  el tamaño de la caché _C_, en bytes
3.  la asociatividad de la caché, _E_
4.  el número de sets de la caché, _S_

Ejemplo de invocación:

```
$ ./cachesim blowfish.xex 2048 4 8
```

El parámetro `-v` es opcional pero, de estar presente, siempre aparece en quinto lugar, seguido de dos números enteros. Su presencia activa un “modo verboso” en que se imprime información detallada para un subconjunto de las operaciones.
