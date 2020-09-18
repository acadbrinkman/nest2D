FROM python:3.6

ENV PYTHONUNBUFFERED 1

RUN \
  apt-get update && \
  apt-get install -y apt-utils && \
  apt-get install -y binutils libproj-dev gdal-bin && \
  apt-get install -y cmake libboost-all-dev libpolyclipping-dev && \
  rm -fr /var/lib/apt/list/*

RUN mkdir /code 
WORKDIR /code 

RUN pip install shapely

COPY . /code
RUN pip install . 