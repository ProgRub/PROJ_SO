Para fazer build do projeto executar o seguinte comando no terminal: make all
Para eliminar os ficheiros previamente compilados executar o seguinte comando no terminal: make clean
Para eliminar os ficheiros previamente compilados E fazer build do projeto ao mesmo tempo executar o seguinte comando no terminal: make cleanAndBuild

Os dados da configuração encontram-se no ficheiro "simulador.conf". A ordem dos dados no ficheiro NÃO PODE SER ALTERADA para garantir o correto carregamento dos dados e consequente correto funcionamento da simulação.
As probabilidades deverão ser um valor entre 0 e 1. Os dados relacionados com tempo têm a sua unidade na descrição da unidade (minutos, horas ou dias). Todos os dados têm a sua própira descrição curta.
O número de médicos da simulação é equivalente ao tamanho do hospital.
Para especificar os "picos" na simulação estes devem ser separados por ", " e cada pico deve ter o dia de início na esquerda de um "-" e à direita a duração do pico.
Exemplo: 3-5, 9-4. Um pico começa no dia 3 e tem a duração de 5 dias (acaba no dia 8) e outro pico começa no dia 9 e tem a duração de 4 dias (acaba no dia 12)

O ficheiro "config.h" contém as bibliotecas utilizadas, os dados relativos aos sockets, as estruturas e as constantes. Os ficheiros "monitor.c" e "simulador.c" implementam, respetivamente, o monitor e o simulador.
Os dados da simulação podem ser encontrados em "Relatorio.txt".