# Simulação de Rede CAN em Veículo com FreeRTOS e ESP32

Este projeto é uma simulação de uma rede Controller Area Network (CAN) em um veículo, implementada usando FreeRTOS em uma placa ESP32. O objetivo é interligar diversos subsistemas automotivos, como motor, frenagem, airbag, entre outros, e simular seu comportamento e comunicação. Também se encontra um arquivo python, responsável por "puxar" os dados proveniente da porta COM e também o tratamento destes dados para poder se gerar uma média de cada atributo de tempo de resposta.

## Características

- **Multithreading**: Utiliza as capacidades de multitarefa do FreeRTOS para simular operações paralelas dos subsistemas do veículo.
- **Sincronização de Tarefas**: Emprega semáforos para gerenciar o acesso a recursos compartilhados e evitar condições de corrida.
- **Simulação de Sensores**: Inclui a simulação de interações do usuário através de um touchpad.
- **Monitoramento em Tempo Real**: Fornece feedback em tempo real sobre o estado de cada subsistema, incluindo velocidade e consumo de combustível.

## Pré-requisitos

- Hardware: Placa ESP32.
- Software: ESP-IDF, um ambiente de desenvolvimento para ESP32.
