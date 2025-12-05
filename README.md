# Sis-Op

Contiene la implementación de la Tarea 2 para la asignatura de Sistemas Operativos, la cual se divide en dos módulos principales:
Sincronización con Barrera Reutilizable: Implementación de una primitiva de sincronización avanzada utilizando el patrón Monitor.
Simulador Simple de Memoria Virtual: Desarrollo de un simulador de paginación que incluye la traducción de direcciones virtuales y la aplicación del algoritmo de reemplazo de páginas Clock.
Parte I: Sincronización con Barrera Reutilizable

Esta sección del proyecto se centró en la implementación de una Barrera Reutilizable diseñada para coordinar un número N de hebras concurrentes en múltiples puntos de encuentro o etapas.Implementación de la Barrera

La barrera se implementó como un monitor, utilizando la estructura reusable_barrier_t, que incluye los siguientes componentes clave:
N: Número total de hebras a sincronizar.
Count: Contador de hebras que han llegado al punto de encuentro.
Etapa: Identificador de la etapa actual para permitir la reutilización de la barrera.
Mutex: Para garantizar la exclusión mutua al acceder a las variables compartidas.
Cond: Variable de condición utilizada para la espera de las hebras.
Funciones Principales
barrier_init(reusable_barrier_t *b, int num_threads): Inicializa la barrera y sus primitivas de sincronización.
barrier_destroy(reusable_barrier_t *b): Libera los recursos (mutex y variable de condición).
barrier_wait(reusable_barrier_t *b): La función principal de sincronización. Incrementa Count y, si es la última hebra en llegar (Count == N), incrementa Etapa, resetea Count y despierta a las demás hebras. De lo contrario, la hebra espera.
Aplicación de Verificación

Se desarrolló un programa de verificación que acepta los siguientes parámetros por línea de comandos:
N: Número de hebras (por defecto, 5).
E: Número de etapas (por defecto, 4).
Cada hebra ejecuta un ciclo de E etapas, simulando trabajo asíncrono y sincronizándose rigurosamente en la barrera para garantizar que ninguna hebra avance a la siguiente etapa hasta que todas hayan alcanzado el punto de encuentro.

Parte II: Simulador Simple de Memoria Virtual

Este módulo modela el funcionamiento de la memoria virtual con un esquema de paginación y un algoritmo de reemplazo.Actividad 1: Simulador de Traducción de Direcciones

Se implementó el módulo de Traducción de Direcciones Virtuales (DV) para obtener la Dirección Física (DF).
Procesamiento de Direcciones: La función parse_address() maneja direcciones en formato decimal y hexadecimal.
Separación de Componentes: La DV se separa en Número de Página Virtual (npv) y Desplazamiento (offset) utilizando la lógica clásica de la MMU:
offset = DV & (PAGE_SIZE − 1)
npv = DV >> b (donde b son los bits de desplazamiento).
Construcción de Dirección Física: La función make_physical() construye la DF a partir del número de marco (frame) y el offset:
DF = (frame << b) | offset
Estructura Base: Se definió page_entry para la tabla de páginas (con campos frame y valid) y frame_entry para la tabla de marcos (incluyendo el bit de uso).
Modo Verbose: La función verbose_print() permite visualizar el estado detallado de cada traducción (DV, npv, offset, HIT/FALLO, frame, DF).
Algoritmo de Reemplazo: Clock

El simulador implementa el algoritmo Clock, una aproximación eficiente al algoritmo LRU (Least Recently Used), que utiliza un bit de uso para cada marco y un puntero circular.

Flujo del Algoritmo (en caso de Fallo de Página):
Se busca primero un marco libre.
Si no hay marcos libres, se activa el recorrido circular del algoritmo Clock:
Si el bit de uso es 1, se limpia (se pone a 0) y el puntero avanza.
Si el bit de uso es 0, el marco es seleccionado como víctima y se reemplaza.
El bit de uso se actualiza a 1 para los accesos exitosos (HIT).
Metodología y Resultados

Se realizaron experimentos variando el número de marcos físicos (N) y utilizando distintas trazas de accesos.
Traza	Tamaño Página (Bytes)	Marcos Físicos (N)	Fallos de Página	Tasa de Fallos (%)
trace1.txt	8	8	8073	98.55%
trace1.txt	8	16	7943	96.96%
trace1.txt	8	32	7713	94.15%
trace2.txt	4096	8	7649	93.37%
trace2.txt	4096	16	7138	87.13%
trace2.txt	4096	32	6142	74.98%
Conclusiones Clave:
Rendimiento del Clock: Los resultados confirman que la tasa de fallos disminuye a medida que aumenta la cantidad de marcos físicos, lo cual es coherente con la teoría de la gestión de memoria.
Localidad Temporal: El algoritmo Clock demostró ser capaz de capturar la localidad temporal en las trazas, ofreciendo un rendimiento cercano al LRU con un menor costo computacional.
