# Projeto-Monitoramento-Ambientes

- Desenvolvido por: Lucas Coelho de Almeida

- Objetivo: Firmware de sistema que sustenta vários serviços: auferição de alta precisão de várias variáveis e sensores, sevidor web com dashboard atualizada em tempo real, servidor SNMP e agente-zabbix. O objetivo do projeto é monitorar uma ampla gama de variáveis em qualquer ambiente, além de ser escalável com outros módulos e metodologias. Transforma a infraestrutura física em ativos gerenciáveis.
 
- Forma de uso: Configurar o firmware para o dimensionamento do projeto, realizar a montagem do sistema e carregar o programa na placa. Configurações de rede e data/hora podem ser feitas via acesso à porta serial (indica-se o Putty).

- OBS.1: O uso, a compreensão, configuração, modificações e a implementação do projeto não são nada triviais, entre em contato caso precise de suporte.

- Obs.2: Testado e funcional com a versão 1.89 da IDE Arduino.

- Obs.3: O sistema usa o OID "1.3.6.1.4.1.49191." que está cadastrado na IANA sob o ramo enterprise (Internet Assigned Numbers Authority).

- Obs.4: Versão 18.4 do sistema.

- Obs.5: Necessária alteração na biblioteca Agentuino para funcionamento adequado.

- Obs.6: O agente-zabbix funciona corretamente até a versão 3. A partir da versão 4, não funciona mais devido o envio ser em texto simples, deveria trocar pelo formulário padrão do Zabbix. Dê preferência pelo uso do SNMP para soluções prontas e do telnet para integrações novas de programas em desenvolvimento.