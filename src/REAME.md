# Requisitos previos
Debe estar instalado en el sistema: 
1. Los drivers con aceleración 3d del fabricante con soporte para OpenGL
2. Un entornos de programación para lenguaje C/C++. En linux con tener el GCC y G++ podría ser suficiente. 
Se pueden utilizar entornos como VScode o CLion. 
En Windows se aconseja Visual Studio 2019/2022 y en Mac el XCode. Otros compiladores como el MinGW tamibén podrian funcionar.
3. CMake instalado y accesible desde el terminal o consola de comandos.

# Puesta en funcionamiento del proyecto
1. Una vez descomprimido el zip y acceder mediante el terminal al directorio build.
En entornos Linux con GCC/G++ instalado ejecutar
cmake .. -DOpenGL_GL_PREFERENCE=GLVND
make -j4

2. En entornos windows con visual studio 2019/2022
Abrir diréctamente el fichero de solución de VS
Darle a compilar

#  Ejecución
En el directio /build/bin se habrán generado los binarios correspondientes al proyecto
En general los podeis lanzar desde dentro de vuestro entornio de programacion (Eg CLion, Visual Studio, XCode etc.)
También se pueden lanzar haciendo doble click sobre cada binario

# Notas finales
En el zip hay un total de 4 prácticas y un proyecto final.
Cada uan de las prácticas esta estructurada con un fichero basico y otro pensado para que hagáis las pruebas y las respectivas entregas
El fichero básico en cada una de las prácticas esta nombrado como GpO_01.cpp, GpO_02.cpp etc.
El fichero de entrega, que es una copia del fichero básico en el que se realizarán los ejercicios, está nombreado como GpO_0X_entrega.cpp
Esto nos permitirá dejar intacto el fichero original y hacer los ejercicios en el fichero de entrega.
Todos los ejercicios parten del mismo código que el de la práctica 01, pero en cada laboratorio se actualizarán, simplemente hay que sobreescribir los ficheros correspondientes y volver a compilar.
