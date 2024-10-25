### Exercise 2

**a) List and explain the sequence of calls performed by the program in x86-64/prog.s**

----

-
    - O primeiro **syscall(257)** feito na linha 16 é o **openat** onde nos argumentos passados, o primeiro (-100) é a diretória onde o ficheiro se encontra, no caso a diretoria atual , o segundo é o nome do ficheiro a abrir ("/etc/os-release") e o terceiro são as flags do ficheiro, neste caso temos a flag APPEND, pois o valor do argumento é 0.

    - O segundo **syscall(8)** feito na linha 24 é o **lseek** onde nos argumentos passados, o primeiro é o file descriptor do ficheiro aberto anteriormente, o segundo o offset a ser usado na função e o terceiro o modo de seek, neste caso o modo de seek é o SEEK_END, pois o valor do argumento é 2. Com este syscall é possível saber o tamanho do ficheiro.

    - O terceiro **syscall(9)** feito na linha 35 é o **mmap** onde nos argumentos passados, o primeiro é o endereço de memória onde o ficheiro vai ser mapeado, é passado 0 pois o sistema operativo é que escolhe o endereço, o segundo é o tamanho do ficheiro a ser mapeado, neste caso é o tamanho do ficheiro obtido no syscall anterior, o terceiro é o modo de proteção da memória, neste caso é o PROT_READ pois o ficheiro é apenas para leitura, o quarto é o modo de flags, neste caso é o MAP_PRIVATE pois o ficheiro é privado, o quinto é o file descriptor do ficheiro aberto anteriormente (openat) e o sexto argumento é o offset do ficheiro a ser mapeado, neste caso é 0.

    - O quarto **syscall(1)** feito na linha 41 é o **write** onde nos argumentos passados, o primeiro é o file descriptor do ficheiro onde vai ser escrito, no caso é o stdout, o segundo é o endereço de memória onde o ficheiro foi mapeado, obtido através do mmap, o terceiro é o tamanho do ficheiro a ser escrito, neste caso é o tamanho do ficheiro obtido no syscall anterior(lseek).

    - O quinto **syscall(231)** feito na linha 45 é o **exit_group** onde nos argumentos passados, o primeiro é o valor de retorno do programa, neste caso é 0, pois o programa terminou com sucesso.
