FROM ubuntu:latest

RUN apt-get update && apt-get install build-essential siege -y

COPY . /app
WORKDIR /app

RUN make

EXPOSE 8081

CMD ./webserv /app/conf.d/default.conf




