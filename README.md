# Atividade 10-02

Este é o repositório que armazena a tarefa solicitada no dia 10/02. Todos os arquivos necessários à importação já foram criados, de modo que basta seguir as instruções abaixo para executá-lo em seu dispositivo.

## Como Usar

1. Para acessar a atividade armazenada, clone este repositório para seu dispositivo executando o seguinte comando no terminal de um ambiente adequado, como o VS Code, após criar um repositório local: 
git clone https://github.com/nivaldojunior037/Atividade-10-02-EmbarcaTech

2. Após isso, abra a pasta que contém os arquivos clonados em um ambiente como o VS Code e compile o código existente para que os demais arquivos necessários sejam criados em seu dispositivo

3. Ao fim da compilação, será gerado um arquivo .uf2 na pasta build do seu repositório. Esse arquivo deve ser copiado para a BitDogLab em modo BOOTSEL para que ele seja corretamente executado. 

### Como Executar o Código

1. O código foi projetado para que, ao movimentar o joystick da placa BitDogLab, o quadrado inicialmente centralizado no display se movimente de acordo com a movimentação dos potenciômetros. 

2. A luminosidade dos LEDs vermelho e azul do conjunto RGB central também são controlados por meio do valor convertido da tensão nos potenciômetros. Quando o joystick está em sua posição de repouso, os LEDs deverão ficar apagados. Quando ele for movimentado para cima ou para baixo, o LED azul mostra variação de luminosidade. O mesmo ocorre para o LED vermelho quando o joystick é movimentado para a esquerda ou para a direita. 

3. Quando o botão A é pressionado, o PWM dos LEDs do conjunto RGB tem seu estado alternado entre ligado ou desligado. A movimentação do quadrado no display, contudo, permanece normal. 

4. Quando o botão do joystick é pressionado, o LED verde do conjunto RGB tem seu estado alternado entre aceso e apagado. Além disso, também surge um novo retângulo no display da placa, dentro do retângulo já presente inicialmente. Assim, é possível ver uma borda mais grossa próxima às bordas do display. 

#### Link do vídeo

Segue o link do Drive com o vídeo onde é demonstrada a utilização do código: https://drive.google.com/drive/folders/1dYV0IJeQRpKGUEiStQvewQ0GKkADP9aX?usp=sharing

##### IMPORTANTE

É possível ver no código, na definição das constantes, que o potenciômetro do eixo X do joystick está conectado à porta GPIO 27, enquanto o do eixo Y está na GPIO 26. Isso foi um ponto que dificultou a execução do código, baseado na explicação dada onde o eixo X estaria na porta 26 e o eixo Y estaria na porta 27. Considerando a explicação dada, os eixos ficaram invertidos, caso que só foi resolvido com a troca das portas GPIO. 

Caso em seu dispositivo BitDogLab a movimentação do quadrado no display esteja com eixos invertidos, vá até o arquivo AtvADC.c e troque #define VRX_PIN 27  e #define VRY_PIN 26 por #define VRX_PIN 26 e #define VRY_PIN 27, nas linhas 22 e 23 do código. Além disso, troque, na linha 109, adc_select_input(1) por adc_select_input(0). Faça o processo inverso na linha 131. Isso deve garantir a correção dos eixos para o seu caso.