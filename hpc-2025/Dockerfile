# Create image with:
#
# > docker build --force-rm --tag iscte/hpc2025 .
#
# Run image with:
# 
# > docker run --interactive --tty --hostname hpc \
#    --volume "$PWD/labs":/home/student/labs \
#    --name hpc --rm iscte/hpc2025
#
# Publish image with:
#
# > docker push iscte/hpc2025
#
# You will need to login to your account first

FROM debian

RUN echo 'APT::Install-Suggests "0";' >> /etc/apt/apt.conf.d/00-docker
RUN echo 'APT::Install-Recommends "0";' >> /etc/apt/apt.conf.d/00-docker

# 

RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update \
  && apt install -y \
    build-essential \
    mpi-default-dev \
    locales \
    vim \
    gdb \
    file \
    python3 \
  && rm -rf /var/lib/apt/lists/*

# Set the locale
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

RUN useradd -ms /bin/bash student

USER student
WORKDIR /home/student


