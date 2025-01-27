# Campus das Coisas - IoT architecture for IPS campus
<img src="images\KerlinkWirnetiStation.jpeg" alt="Gateway Kerlink Wirnet iStation" width="300" height="300" style="border-radius:10px;">
<div class="image-container">
  <img alt="Static Badge" src="https://img.shields.io/badge/LoRa-blue">
  <img alt="Static Badge" src="https://img.shields.io/badge/LoRaWAN-blue">
  <img alt="Static Badge" src="https://img.shields.io/badge/TheThingsNetwork-%232980b9%20">
  <img alt="Static Badge" src="https://img.shields.io/badge/TagoIO-%232980b9%20">
  <img alt="Static Badge" src="https://img.shields.io/badge/AppInventor-orange">
  <img alt="Static Badge" src="https://img.shields.io/badge/SmartCampus-green">
  <img alt="Static Badge" src="https://img.shields.io/badge/IoT-green">
</div>


> IoT network based on LoRa communication for a smart campus.

### Introduction

Design and implementation of an IoT network on the Setúbal campus of the Polytechnic Institute of Setúbal, with the aim of promoting research and development of IoT projects. The implementation of this network enabled the application of various devices and sensors throughout the campus, transforming it into a smarter campus. This IoT network is based on LoRa long-range communication and the use of cloud servers, which are open-source and accessible to the community. In order to test the network's potential and range, a commercial LoRa device was implemented and a prototype was developed for communication tests, which made it possible to build a coverage map for the campus and the surrounding area, with a maximum range of 17.5 km being obtained in the tests carried out.

## System architecture

<img src="images\SystemArchitectureLoRaNetwork.png" alt="System Architecture LoRa Network" height="300">

This system architecture aims to implement a high capacity gateway capable of managing a large network and connect it to a community network server like The Things Network, facilitating its use by users and preparing the implemented IoT network for many future applications.

## Projects and Devices
### fieldTester

### LoRaGrid
This project aimed to develop an Internet of Things (IoT) framework for IPS, focusing on monitoring electricity consumption at the School of Technology of Setúbal, using a network of sensors with a LoRa interface. The primary focus of the implementation was the design of an automated and scalable network, making it easy and practical to add new sensors to the network. This project included the development of a web application where the data collected from the sensors is processed and displayed clearly and objectively to users, using tools for building charts and ensuring proper access control for data security. To achieve this, a LoRa sensor prototype was designed and connected to The Things Network server, which wirelessly sends the data to the final application. The application was built using the React framework and communicates with a database to fetch and update information according to user navigation. From this work, it was concluded that an alternative to The Things Network server is necessary to make the institute's platform independent of third-party services, as well as modifying the web application to enable remote control of the sensors.

## ☕ Usando <nome_do_projeto>

Para usar <nome_do_projeto>, siga estas etapas:

```
<exemplo_de_uso>
```

Adicione comandos de execução e exemplos que você acha que os usuários acharão úteis. Forneça uma referência de opções para pontos de bônus!

## 📫 Contribuindo para <nome_do_projeto>

Para contribuir com <nome_do_projeto>, siga estas etapas:

1. Bifurque este repositório.
2. Crie um branch: `git checkout -b <nome_branch>`.
3. Faça suas alterações e confirme-as: `git commit -m '<mensagem_commit>'`
4. Envie para o branch original: `git push origin <nome_do_projeto> / <local>`
5. Crie a solicitação de pull.

Como alternativa, consulte a documentação do GitHub em [como criar uma solicitação pull](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request).

## 🤝 Colaboradores

Agradecemos às seguintes pessoas que contribuíram para este projeto:

<table>
  <tr>
    <td align="center">
      <a href="#" title="defina o título do link">
        <img src="https://avatars3.githubusercontent.com/u/31936044" width="100px;" alt="Foto do Iuri Silva no GitHub"/><br>
        <sub>
          <b>Iuri Silva</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="#" title="defina o título do link">
        <img src="https://s2.glbimg.com/FUcw2usZfSTL6yCCGj3L3v3SpJ8=/smart/e.glbimg.com/og/ed/f/original/2019/04/25/zuckerberg_podcast.jpg" width="100px;" alt="Foto do Mark Zuckerberg"/><br>
        <sub>
          <b>Mark Zuckerberg</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="#" title="defina o título do link">
        <img src="https://miro.medium.com/max/360/0*1SkS3mSorArvY9kS.jpg" width="100px;" alt="Foto do Steve Jobs"/><br>
        <sub>
          <b>Steve Jobs</b>
        </sub>
      </a>
    </td>
  </tr>
</table>

## 😄 Seja um dos contribuidores

Quer fazer parte desse projeto? Clique [AQUI](CONTRIBUTING.md) e leia como contribuir.

## 📝 Licença

Esse projeto está sob licença. Veja o arquivo [LICENÇA](LICENSE.md) para mais detalhes.
