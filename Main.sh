#!/bin/bash

# Funcion encargada de mostrar el menu y de comrpobar que la opcion elegida sea valida
showMenu() {
    while true; do
        echo "Menú:"
        echo "1) Mostrar codigo"
        echo "2) Compilar programa"
        echo "3) Ejecutar programa"
        echo "4) Salir"
        read -p "Elija una opción: " opcion
        opcion=$(echo "$opcion" | tr -d '[:space:]')

        if validateOption; then
            break
        else
            echo "ERROR: Opcion no valida, solo se permiten opciones de [1-4]"
        fi
    done

}

#Funcion encargada de mostrar el codigo del programa
showProgramCode() {
    if [ -e "VueloSim.c" ]; then
        cat VueloSim.c
    else
        echo "ERROR: El archivo VueloSim.c no existe"
    fi
}

#Funcion encargada de compilar el programa y de darle permisos de ejecucion
#Tambien que comprueba que exista el programa en C
compileProgram() {
    if [ -e "VueloSim.c" ]; then
        echo "Compilando..."
        gcc -o programa VueloSim.c
        echo "Dando permisos de ejecucion..."
        chmod +x programa
    else
        echo "Error: El archivo VueloSim.c no existe en este directorio."
    fi
}

#Funcion encargada de ejecutar el programa
#Tambien que comprueba que el programa este compilado
#y los parametros sean correctos

executeProgram() {
    if [ -e "programa" ]; then
        if [ -x "programa" ]; then
            read -p "Elige el numero de asistentes de vuelo: " numAsistentesVuelo

            if [[ "$numAsistentesVuelo" =~ ^[0-9]+$ ]]; then
                if [ "$numAsistentesVuelo" -ge 1 ]; then
                    echo "Ejecutando..."
                    ./programa "$numAsistentesVuelo"

                else
                    echo "ERROR: El numero de asistentes tiene que ser >= 1"
                fi

            else
                echo "ERROR: El parametro tiene que ser un numero"
            fi

        else
            echo "ERROR: el archivo no tiene permisos de ejecución"
        fi
    else
        echo "ERROR: El archivo no existe. Prueba a compilarlo antes"
    fi
}

#Funcion encargada de salir del programa
exitProgram() {
    echo "Saliendo del programa..."
    exit
}

#Funcion encargada de validar la opcion pasada en el menu
validateOption() {
    if [[ "$opcion" =~ ^[0-9]+$ ]]; then
        return 0
    else
        return 1
    fi
}

#Bucle principal del programa
opcion=0
while [ "$opcion" -ne 4 ]; do
    showMenu
    numAsistentesVuelo=0

    if validateOption; then
        case $opcion in
        1)
            showProgramCode
            ;;
        2)
            compileProgram
            ;;
        3)
            executeProgram
            break
            ;;
        4)
            exitProgram

            ;;
        *)
            echo "ERROR: Opción no válida[1-4]"
            ;;
        esac
    else
        echo "ERROR: La opcion tiene que ser un numero"
    fi

done
