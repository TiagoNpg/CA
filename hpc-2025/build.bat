@echo off
rem Builds container image for Computação Avançada 2025/26 @ ISCTE-IUL

echo Building container for Computação Avançada 2025/26 @ ISCTE-IUL

docker build --force-rm --tag iscte/hpc2025 .
