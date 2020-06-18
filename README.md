# Projeto-Monitoramento-Ambientes

- Desenvolvido por: Lucas Coelho de Almeida

- Objetivo: Firmware de sistema que sustenta v�rios servi�os: auferi��o de alta precis�o de v�rias vari�veis e sensores, sevidor web com dashboard atualizada em tempo real, servidor SNMP e agente-zabbix. O objetivo do projeto � monitorar uma ampla gama de vari�veis em qualquer ambiente, al�m de ser escal�vel com outros m�dulos e metodologias. Transforma a infraestrutura f�sica em ativos gerenci�veis.
 
- Forma de uso: Configurar o firmware para o dimensionamento do projeto, realizar a montagem do sistema e carregar o programa na placa. Configura��es de rede e data/hora podem ser feitas via acesso � porta serial (indica-se o Putty).

- OBS.1: O uso, a compreens�o, configura��o, modifica��es e a implementa��o do projeto n�o s�o nada triviais, entre em contato caso precise de suporte.

- Obs.2: Testado e funcional com a vers�o 1.89 da IDE Arduino.

- Obs.3: O sistema usa o OID "1.3.6.1.4.1.49191." que est� cadastrado na IANA sob o ramo enterprise (Internet Assigned Numbers Authority).

- Obs.4: Vers�o 18.4 do sistema.

- Obs.5: Necess�ria altera��o na biblioteca Agentuino para funcionamento adequado.

- Obs.6: O agente-zabbix funciona corretamente at� a vers�o 3. A partir da vers�o 4, n�o funciona mais devido o envio ser em texto simples, deveria trocar pelo formul�rio padr�o do Zabbix. D� prefer�ncia pelo uso do SNMP para solu��es prontas e do telnet para integra��es novas de programas em desenvolvimento.