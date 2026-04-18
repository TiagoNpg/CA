@echo off
rem Launches Docker image mounting local folder "labs"

echo Computação Avançada 2025/26 @ ISCTE-IUL

docker run --interactive --tty --hostname hpc ^
    --volume "%cd%/labs":/home/student/labs ^
    --name hpc --rm iscte/hpc2025
