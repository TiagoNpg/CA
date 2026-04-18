# Lab. 1 - Instalação de Software
## Computação Avançada 2025/26

Nesta sessão vamos instalar o software necessário para as sessões de laboratório da unidade curricular. Em particular vamos instalar:

+ **Docker Desktop** - Criar e correr aplicações em "containers"
+ Imagem Docker da cadeira

O editor de texto recomendado é o VS Code.

### 0. Identificar a arquitetura do computador

A generalidade dos computadores utilizados atualmente usam o processadores x86_64 (também chamados x64 ou AMD64) ou ARM64 (também chamados aarch64 ou Apple silicon). É importante instalar as versões de software corretas para a arquitetura que se está a utilizar.

Se não souber qual é a arquitetura do computador pode identificá-la fazendo:

+ **Windows**
  + Lançar a aplicação "Settings", e escolher "System > About"
+ **macOS**
  + Clicar no ícone  no canto superior esquerdo e escolher "About this Mac"

### 1. Instalar o Docker Desktop

O Docker Desktop pode ser obtido em https://www.docker.com/. Faça o download da versão do "Docker Desktop" adequada à sua arquitetura

#### Windows

1. Abrir o ficheiro `Docker Desktop Installer`

2. Use a configuração _default_

   + (Yes) - Use WSL 2 instead of Hyper-V (recommended)
   + (No) - Allow Windows containers to be used with this installation
   + (Yes) - Add shortcut to Desktop

3. Fechar e reiniciar
4. Lance a aplicação Docker
    + Deve ter sido instalado um ícone no Desktop
5. Pode ser necessário atualizar o "Windows Subsystem for Linux"
      + Permitir que a atualização seja instalada


Se o Docker der uma mensagem de erro do tipo "a versão do WSL é demasiado antiga", escolha "Try Again".

#### macOS

1. Abrir o ficheiro `Docker.dmg`
2. Arrastar a aplicação `Docker` para a pasta `Applications`
3. Lance a aplicação `Docker`
    + Quando surgir a mensagem "Docker is an app downloaded from the internet..." escolha "Open"


### 2. Instalação da imagem da cadeira

1. Faça o download do ficheiro `hpc-2025.zip` a partir da secção "Material de Apoio > Aulas de Laboratório".
   + Descomprima o ficheiro numa diretoria à sua escolha.
2. Abra um terminal (pode usar o VS Code) e navegue até à diretoria onde descomprimiu os ficheiros
   + É a diretoria onde está presente um ficheiro de nome `Dockerfile`
3. Execute o comando `build`:
	+ Windows: escreva `.\build` 
	+ macOS: escreva `./build`

O processo de instalação irá agora continuar automaticamente. Dependendo da ligação à internet este processo poderá demorar alguns minutos.

```text
$ ./build
Building container for Computação Avançada 2025/26 @ ISCTE-IUL
[+] Building 0.1s (11/11) FINISHED                                   docker:desktop-linux
 => [internal] load build definition from Dockerfile                                 0.0s
 => => transferring dockerfile: 989B                                                 0.0s
 => [internal] load metadata for docker.io/library/debian:latest                     0.0s
 => [internal] load .dockerignore                                                    0.0s
 => => transferring context: 2B                                                      0.0s
 => [1/7] FROM docker.io/library/debian:latest@sha256:5cf544fad978371b3df255b61e209  0.0s

...

 => => unpacking to docker.io/iscte/hpc2025:latest                                   0.0s

```

Este processo só terá de ser repetido se houver alguma atualização à imagem da cadeira.


### 3. Correr o "Container"

Para lançar um "container" com a imagem que acabou de construir, ou seja, lançar o ambiente de trabalho da cadeira basta lançar o comando `run` na diretoria em que construiu a imagem.

Execute o comando `run`:
+ Windows: escreva `.\run` 
+ macOS: escreva `./run`

A partir deste momento surge uma *prompt* `student@hpc:~$`:

```bash
Computação avançada 2025/26 @ ISCTE-IUL
student@hpc:~$
```

O ambiente de trabalho está pronto a ser utilizado.


### 4. Verificar a Instalação

Para testar a instalação vamos compilar 2 programas de teste, um em MPI e outro em OpenMP. Para este efeito basta executar o comando `make` na diretoria `labs/hello`:

```bash
./run
Computação avançada 2025/26 @ ISCTE-IUL
student@hpc:~$ cd labs/hello/
student@hpc:~/labs/hello$ make
mpicc mpi.c -o mpi
gcc -fopenmp openmp.c -o openmp
student@hpc:~/labs/hello$
```

Execute agora o programa OpenMP. Basta executá-lo como qualquer outro programa:

```bash
student@hpc:~/labs/hello$ ./openmp
Hello World from thread = 10
Hello World from thread = 7
Hello World from thread = 6
Hello World from thread = 9
Hello World from thread = 0
Number of threads = 14
Hello World from thread = 12
Hello World from thread = 11
Hello World from thread = 1
Hello World from thread = 13
Hello World from thread = 2
Hello World from thread = 3
Hello World from thread = 4
Hello World from thread = 5
Hello World from thread = 8
```

O número de _threads_ irá depender do hardware específico do seu computador. 

Para lançar o programa MPI é necessário utilizar um comando adicional (`mpirun`) que será responsável por lançar os vários processos e inicializar a comunicação entre eles. Por exemplo, para lançar 4 processos faríamos:

```bash
student@hpc:~/labs/hello$ mpirun -np 4 ./mpi
Processor 3 of 4: Hello World!
Processor 0 of 4: Hello World!
Processor 1 of 4: Hello World!
Processor 2 of 4: Hello World!
```

### 5. Configuração sistema

Para obtermos informação sobre o CPU do nosso computador e, em particular, o número de _cores_ disponíveis, podemos utilizar o comando `lscpu`:

```text
student@hpc:~/labs/hello$ lscpu
Architecture:                aarch64
  CPU op-mode(s):            64-bit
  Byte Order:                Little Endian
CPU(s):                      14
  On-line CPU(s) list:       0-13
Vendor ID:                   Apple
  Model name:                -
    Model:                   0
    Thread(s) per core:      1
    Core(s) per cluster:     14
    Socket(s):               -
    Cluster(s):              1
    Stepping:                0x0
    BogoMIPS:                48.00
    Flags:                   fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimd
                             hp cpuid asimdrdm jscvt fcma lrcpc dcpop sha3 asimddp sha512 
                             asimdfhm dit uscat ilrcpc flagm sb paca pacg dcpodp flagm2 fr
                             int bf16 bti afp

...
```

No sistema apresentado acima (Mac Book Pro) temos 14 _cores_ disponíveis.