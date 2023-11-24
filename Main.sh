#!/bin/bash

mostrar_menu() {
    echo "Menú:"
    echo "1) Mostrar codigo"
    echo "2) Compilar programa"
    echo "3) Ejecutar programa"
    echo "4) Salir"
    read -p "Elija una opción: " opcion
}

opcion=0
while [ "$opcion" -ne 4 ]; do
    mostrar_menu
    numAsistentesVuelo=0
    case $opcion in
    1)
        if [ -e "VueloSim.c" ]; then
            cat VueloSim.c
        else
            echo "ERROR: El archivo VueloSim.c no existe"
        fi
        cat VueloSim.c
        ;;
    2)
        if [ -e "VueloSim.c" ]; then
            echo "Compilando..."
            gcc -o programa VueloSim.c
            echo "Dando permisos de ejecucion..."
            chmod +x programa
        else
            echo "Error: El archivo VueloSim.c no existe en este directorio."
        fi
        ;;
    3)
        if [ -e "programa" ]; then
            if [ -x "programa" ]; then
                read -p "Elige el numero de asistentes de vuelo: " numAsistentesVuelo
                if [ "$numAsistentesVuelo" -ge 1 ]; then
                    echo "Ejecutando..."
                    ./programa "$numAsistentesVuelo"
                    break
                else
                    echo "ERROR: El numero de asistentes tiene que ser >= 1"
                fi
                
            else
                echo "ERROR: el archivo no tiene permisos de ejecución"
            fi
        else
            echo "ERROR: El archivo no existe. Prueba a compilarlo antes"
        fi
        ;;
    4)
        echo "Saliendo del programa..."
        exit
        ;;
    *)
        echo "Opción no válida"
        ;;
    esac
done
