#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include<string.h>
#include <time.h>
#include <conio.h>

/*
 *Turma: CP116TIN1
 *Grupo: 13
 *Alan Cruz Roza RA: 200904
 *Daniel de Lima Ferreira RA: 200010
 *Matheus Henrique Santos Zacarias RA: 200064
 */

struct info_livro{
    char sigla;
    int reg;
}info_livro;

struct info_aluno{
    char sigla;
    char RA[7];
    int dia_ret; // dia � m�x=31
    int mes_ret; // mes: fev=28 dias
    int dia_dev; // mes: abril=jun=set=nov=30 dias
    int mes_dev; // os demais = 31 dias
}info_aluno;

typedef struct aluno{
    char nome[80];
    char RA[7];
    int emprestado;
    int reservado;
    struct info_livro tabela[4];
}aluno;

typedef struct livro{
    int reg;
    char titulo[80];
    char autor[80];
    struct info_aluno status[2];
}livro;

void cabecalho(int selec);
int menu(void);
void opcaoMenuPrincipal(int selec);
void aloca_aluno(aluno **paluno, int tam);
void aloca_livro(livro **plivro, int tam);
void grava_livro(livro *p, char *str, int pos);
void grava_aluno(aluno *p, char *str, int pos);
void cadastra_aluno(aluno *paluno);
void cadastraLivro(livro *plivro);
int opcaoConsulta(int selec);
void consultaTotalAlunos(aluno *p);
void consultaTotalLivros(livro *p);
int verifica_livro(void);
int verifica_aluno(void);
void busca_aluno_ra(aluno *p);
void busca_livro_status(livro *p, char status);
void busca_livro_titulo(livro *p);
int opcaoParcialLivros(void);
char opcaoStatusLivro (void);
int validaRA(char *RA);
int validaTexto (char *str);
void calculaData (livro *p, char selec, int diaDevolucao, int mesDevolucao);
int validaAluno (aluno *p);
void proibeAluno(void);
void emprestaLivro (aluno *pAluno, livro *pLivro, int condiAluno, int condiLivro);
int validaLivro(livro *pLivro);
int	 buscaPosAluno(aluno *p, char *RA);
int	 buscaPosLivro(livro *p, int reg);
void devolveLivro (aluno *pAluno, livro *pLivro);
void gotoxy(int x, int y);
int distanciaDias (int diaPrevisto, int mesPrevisto, int diaDevolucao, int mesDevolucao);
void deletaLivro(livro *p, aluno *pAluno);

int main (void){

    int opcao, consulta;
    char status;
    aluno *alunos = NULL;
    livro *livros = NULL;

    aloca_aluno(&alunos, 1);
    aloca_livro(&livros, 1);

    do{
        opcao = menu();
        switch(opcao){
            case 1:
                mostraCursor();
                cadastra_aluno(alunos);
            break;
            case 2:
                consulta = opcaoConsulta(2);
                if (consulta == 1){
                    consultaTotalAlunos(alunos);
                }
                else{
                    mostraCursor();
                    busca_aluno_ra(alunos);
                }
            break;
            case 3:
                mostraCursor();
                cadastraLivro(livros);
            break;
            case 4:
                consulta = opcaoConsulta(4);
                if (consulta == 1){
                    consultaTotalLivros(livros);
                }
                else{
                    consulta = opcaoParcialLivros();
                    if(consulta == 1){
                        mostraCursor();
                        busca_livro_titulo(livros);
                    }
                    else{
                        mostraCursor();
                        status = opcaoStatusLivro();
                        busca_livro_status(livros, status);
                    }
                }
            break;
            case 5:
                consulta = validaAluno(alunos);
                if (consulta == -1){
                    proibeAluno();
                }
                else if (consulta != 3){
                    status = validaLivro(livros);

                    if (status != 2){
                        emprestaLivro(alunos, livros, consulta, status);
                    }
                }
            break;
            case 6:
                devolveLivro(alunos, livros);
            break;
            case 7:
                deletaLivro(livros, alunos);
            break;
        }

    }while (opcao != 8);

    return 0;
}

void deletaLivro(livro *p, aluno *pAluno){

    FILE *fptr=NULL;
    int i, k, achou=-1,j;
    long int qtd;
    char titulo[80], tituloAux [80], *troca;

    troca = titulo;

    qtd = verifica_livro();
    setlocale(LC_ALL, "Portuguese");
    cabecalho(7);
    printf("\n Informe o T�tulo do livro: ");
    gets(titulo);
    fflush(stdin);

    for(i=0; i< strlen(titulo); i++){
        *(troca+i) = toupper(*(troca+i));
    }

    if((fptr=fopen("livro.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquivo");
    else{
        for(i=0;i<qtd;i++){

            fseek(fptr,i*sizeof(livro),0);
            fread(p,sizeof(livro),1,fptr);

            strcpy(tituloAux, p->titulo);
            troca = tituloAux;
            for(k=0; k< strlen(tituloAux); k++){
                *(troca+k) = toupper(*(troca+k));
            }

            if(strcmp(tituloAux,titulo) == 0){
                achou = i;
                printf("\n Nome do livro: %s", p-> titulo);
                printf("\n Autor: %s", p-> autor);
                printf("\n N�mero de registro: %d", p-> reg);
                printf ("\n ----------------------------------------- \n");
            }
        }
        if(achou == -1)
            printf("\n Nenhum livro encontrado");
        fclose(fptr);
    }

    int reg;
    printf("\n Informe o registro do livro a ser excluido: ");
    scanf("%i", &reg);

    int posLivro = buscaPosLivro(p, reg);

    strcpy(tituloAux, p->titulo);
    troca = tituloAux;
    for(k=0; k< strlen(tituloAux); k++){
        *(troca+k) = toupper(*(troca+k));
    }

    if (strcmp(tituloAux, titulo)){
        printf("\n O registro n�o bate com o titulo inserido! ");
        tiraCursor();
        printf("\n\n Pressione qualquer tecla para voltar para o menu...");
        getch();
    }

    strcpy(p->titulo, "@");

    int posAluno;

    if ((p->status)->sigla == 'E'){
        printf("\n O aluno ser� multado em: R$ 150,00");
        posAluno = buscaPosAluno(pAluno, (p->status)->RA);
        printf("\n Nome do aluno: %s", pAluno-> nome);
        printf("\n RA: %s", pAluno-> RA);

        for(i=0;i<3;i++){
            if(p->reg == ((pAluno->tabela+i)->reg)){
               (pAluno->tabela+i)->reg = NULL;
               (pAluno->tabela+i)->sigla = 'L';
               pAluno->emprestado --;
               break;
            }
        }
        grava_aluno(pAluno,"rb+",posAluno);

        strcpy((p->status)->RA, "");
        (p->status)->dia_ret = NULL;
        (p->status)->mes_ret = NULL;
        (p->status)->dia_dev = NULL;
        (p->status)->mes_dev = NULL;
        (p->status)->sigla = 'L';
        grava_livro(p,"rb+",posLivro);
    }



    if ((p->status+1)->sigla == 'R'){
        printf("\n A reserva do aluno ser� cancelada");
        posAluno = buscaPosAluno(pAluno, (p->status+1)->RA);
        printf("\n Nome do aluno: %s", pAluno-> nome);
        printf("\n RA: %s", pAluno-> RA);
        (pAluno->tabela+3)->reg = NULL;
        (pAluno->tabela+3)->sigla = 'L';
        pAluno->reservado --;
        grava_aluno(pAluno,"rb+",posAluno);
        strcpy((p->status+1)->RA, "");
        (p->status+1)->dia_ret = NULL;
        (p->status+1)->mes_ret = NULL;
        (p->status+1)->dia_dev = NULL;
        (p->status+1)->mes_dev = NULL;
        (p->status+1)->sigla = 'L';
        grava_livro(p,"rb+",posLivro);
    }

    printf("\n Livro excluido com sucesso!");
    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}
void devolveLivro (aluno *pAluno, livro *pLivro){

    int posAluno, posLivro, valorMulta, posAlunoReserva;
    FILE *fptrAluno=NULL;
    FILE *fptrLivro=NULL;
    int i, achou=-1, k, diaDevolucao, mesDevolucao, j, livros;
    long int qtdAluno, qtdLivro;
    char ra[7], titulo[80], tituloAux [80], *troca;


    qtdAluno = verifica_aluno();
    setlocale(LC_ALL, "Portuguese");
    cabecalho(6);
    printf("\n Informe o RA do Aluno: ");
    gets(ra);
    fflush(stdin);

    if((fptrAluno=fopen("aluno.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtdAluno;i++){
            fseek(fptrAluno,i*sizeof(aluno),0);
            fread(pAluno,sizeof(aluno),1,fptrAluno);
            if(strcmp(pAluno->RA,ra) == 0){
                achou = i;
                break;
            }
        }
        if(achou == -1){
            printf("\n Nenhum aluno encontrado");
            tiraCursor();
            printf("\n\n Pressione qualquer tecla para voltar para o menu...");
            getch();
            return;
        }
        fclose(fptrAluno);
    }

    /*---------------------------------------------------------------------------------*/

    printf("\n Informe o T�tulo do livro: ");
    gets(titulo);
    fflush(stdin);
    qtdLivro = verifica_livro();
    achou = -1;
    troca = titulo;

    for(i=0; i< strlen(titulo); i++){
        *(troca+i) = toupper(*(troca+i));
    }

    if((fptrLivro=fopen("livro.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquivo");
    else{
        for(i=0;i<qtdLivro;i++){

            fseek(fptrLivro,i*sizeof(livro),0);
            fread(pLivro,sizeof(livro),1,fptrLivro);
            strcpy(tituloAux, pLivro->titulo);
            troca = tituloAux;
            for(k=0; k< strlen(tituloAux); k++){
                *(troca+k) = toupper(*(troca+k));
            }

            if(strcmp(tituloAux,titulo) == 0){


                for(j=0;j<3;j++){
                    if (pLivro ->reg == ((pAluno->tabela+j)->reg)){
                        achou = i;
                        break;
                    }
                }

                if(achou != -1){
                    break;
                }

            }
        }
        if(achou == -1){
            printf("\n O llvro n�o foi encontrado ou n�o est� com o aluno.");
            tiraCursor();
            printf("\n\n Pressione qualquer tecla para voltar para o menu...");
            getch();
            return;
        }
        fclose(fptrLivro);
    }

    int pos = 46;
    char recebe [4];
    char *desloca = recebe;

    for(i=0;i<5;i++){

        cabecalho(6);
        pos++;

        if (pos == 49){
            pos ++;
        }

        printf("\n Informe o RA do Aluno: %s", pAluno->RA);
        printf("\n\n Informe o T�tulo do livro: %s", pLivro->titulo);
        printf("\n\n Digite a data de devolu��o do livro (dd/mm): ");
        gotoxy(49,9);
        printf("/");

        for (j=0;j<i;j++){

            if (j>1){
                gotoxy(47+j+1, 9);
                printf("%c", *(desloca+j));
            }
            else{
                gotoxy(47+j, 9);
                printf("%c", *(desloca+j));
            }

        }

        if (i<4){
            gotoxy(pos,9);
            *(desloca+i) = getch();
        }

    }

    char converte [2];

    strncat(converte, recebe, 2);
    diaDevolucao = atoi (converte);
    strcpy(converte, "");

    char *teste = converte;
    for (i=0; i<2; i++){
       *(teste+i) = *(desloca+2+i);
    }

    mesDevolucao = atoi (converte);

    valorMulta = distanciaDias(((pLivro->status)->dia_dev),((pLivro->status)->mes_dev), diaDevolucao, mesDevolucao);

    if (valorMulta == -10){
        printf("\n A data inserida n�o � v�lida");
        tiraCursor();
        printf("\n\n Pressione qualquer tecla para voltar para o menu...");
        getch();
        return;
    }

    if (valorMulta > 0){
        valorMulta = valorMulta * 3;
    }

    for(i=0;i<3;i++){
        if(pLivro ->reg == ((pAluno->tabela+i)->reg)){
           (pAluno->tabela+i)->reg = NULL;
           (pAluno->tabela+i)->sigla = 'L';
           pAluno->emprestado --;
           break;
        }
    }

    /*------------------------------------------------------------------------------------------------------------*/

    FILE *fptrAlunoReserva = NULL;
    aluno *alunoReserva = NULL;

    aloca_aluno(&alunoReserva, 1);
    achou = -1;

    if((fptrAlunoReserva=fopen("aluno.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtdAluno;i++){
            fseek(fptrAlunoReserva,i*sizeof(aluno),0);
            fread(alunoReserva,sizeof(aluno),1,fptrAlunoReserva);
            if(strcmp(alunoReserva->RA,((pLivro->status+1)->RA)) == 0){
                achou = i;
                break;
            }
        }
        fclose(fptrAlunoReserva);
    }

    if (achou != -1){
        if(alunoReserva->emprestado == 3){
            for(i=0;i<2;i++){
                (pLivro->status+i)->sigla = 'L';
                strcpy((pLivro->status+i)->RA, "");
                (pLivro->status+i)->dia_ret = NULL;
                (pLivro->status+i)->mes_ret = NULL;
                (pLivro->status+i)->dia_dev = NULL;
                (pLivro->status+i)->mes_dev = NULL;
            }

            (alunoReserva->tabela+3)->sigla = 'L';
            (alunoReserva->tabela+3)->reg = NULL;
            alunoReserva->reservado --;
            achou = -2;
        }
        else{
            i = 0;
            while(i < 3){
                if(((alunoReserva->tabela+i)->sigla) == 'L'){
                    break;
                }
                i++;
            }

            (alunoReserva->tabela+i)->sigla = 'E';
            (alunoReserva->tabela+i)->reg = pLivro->reg;
            (alunoReserva->tabela+3)->sigla = 'L';
            (alunoReserva->tabela+3)->reg = NULL;
            alunoReserva->emprestado ++;
            alunoReserva->reservado --;

            (pLivro->status)->sigla = 'E';
            strcpy((pLivro->status)->RA, alunoReserva->RA);
            calculaData(pLivro, 'B', diaDevolucao, mesDevolucao);

            (pLivro->status+1)->sigla = 'L';
            strcpy((pLivro->status+1)->RA, "");
            (pLivro->status+1)->dia_ret = NULL;
            (pLivro->status+1)->mes_ret = NULL;
            (pLivro->status+1)->dia_dev = NULL;
            (pLivro->status+1)->mes_dev = NULL;

        }
    }
    else{

        for(i=0;i<2;i++){
            (pLivro->status+i)->sigla = 'L';
            strcpy((pLivro->status+i)->RA, "");
            (pLivro->status+i)->dia_ret = NULL;
            (pLivro->status+i)->mes_ret = NULL;
            (pLivro->status+i)->dia_dev = NULL;
            (pLivro->status+i)->mes_dev = NULL;
        }
    }

    aluno *gravaAlunoReserva = NULL;
    aluno *gravaAluno = NULL;
    livro *gravaLivro = NULL;

    aloca_aluno(&gravaAluno, 1);
    aloca_aluno(&gravaAlunoReserva, 1);
    aloca_livro(&gravaLivro, 1);

    posAluno = buscaPosAluno(gravaAluno,pAluno->RA);
    posAlunoReserva = buscaPosAluno(gravaAlunoReserva, alunoReserva->RA);
    posLivro = buscaPosLivro(gravaLivro,pLivro->reg);

    *gravaAluno = *pAluno;
    *gravaLivro = *pLivro;
    *gravaAlunoReserva = *alunoReserva;

    grava_aluno(gravaAluno, "rb+", posAluno);
    grava_aluno(gravaAlunoReserva, "rb+", posAlunoReserva);
    grava_livro(gravaLivro, "rb+", posLivro);

    cabecalho(6);
    printf("\n Livro Devolvido com Sucesso! \n");
    if (valorMulta > 0){
        printf("\n Valor da Multa: R$ %d,00", valorMulta);
    }

    printf("\n Nome do aluno: %s", pAluno-> nome);
    printf("\n RA: %s", pAluno-> RA);
    printf("\n N�mero de livros emprestados: %d", pAluno-> emprestado);
    printf("\n N�mero de livros reservados: %d", pAluno-> reservado);
    printf ("\n ----------------------------------------- \n");


    if (achou == -2){
        printf ("\n O aluno na reserva n�o tem condi��es de emprestar o livro! \n");
        printf("\n Cancelando a reserva de: %s", alunoReserva-> nome);
        printf("\n RA: %s", alunoReserva-> RA);
        printf("\n N�mero de livros emprestados: %d", alunoReserva-> emprestado);
        printf("\n N�mero de livros reservados: %d", alunoReserva-> reservado);
        printf ("\n ----------------------------------------- \n");
    }
    else if (achou == -1){
        printf("\n Nome do livro: %s", pLivro-> titulo);
        printf("\n Autor: %s", pLivro-> autor);
        printf("\n N�mero de registro: %d", pLivro-> reg);
        printf ("\n ----------------------------------------- \n");
    }
    else{
        printf("\n Livro Emprestado com Sucesso! \n");
        printf("\n Emprestado para: %s", alunoReserva-> nome);
        printf("\n RA: %s", alunoReserva-> RA);
        printf("\n N�mero de livros emprestados: %d", alunoReserva-> emprestado);
        printf("\n N�mero de livros reservados: %d", alunoReserva-> reservado);
        printf ("\n ----------------------------------------- \n");
        printf("\n Nome do livro: %s", pLivro-> titulo);
        printf("\n Autor: %s", pLivro-> autor);
        printf("\n N�mero de registro: %d", pLivro-> reg);
        printf("\n Data do empr�stimo: %d/%d", (pLivro-> status) -> dia_ret, (pLivro-> status) -> mes_ret);
        printf("\n Data da devolu��o: %d/%d", (pLivro-> status) -> dia_dev, (pLivro-> status) -> mes_dev);
        printf ("\n ----------------------------------------- \n");
    }



    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();

}

int distanciaDias (int diaPrevisto, int mesPrevisto, int diaDevolucao, int mesDevolucao){

    int dias_mes [13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int *pDias = dias_mes;
    int retornaDias, qtdMeses, i;

    if ((diaDevolucao > *(pDias+mesDevolucao)) || mesDevolucao > 12 || mesDevolucao <= 0 || diaDevolucao <= 0){
        return -10;
    }

    if (mesPrevisto == mesDevolucao){
        retornaDias = diaDevolucao - diaPrevisto;
    }
    else{
        if((mesDevolucao < mesPrevisto) && ((mesDevolucao != 1) && (mesPrevisto != 12))){
            return -10;
        }

        printf("\n %i/%i ------ %i", diaDevolucao, mesDevolucao, *(pDias+mesDevolucao));
    getch();

        if (mesDevolucao > mesPrevisto){
            retornaDias = diaDevolucao + (*(pDias + mesPrevisto) - diaPrevisto);
            for(i=mesPrevisto; i< mesDevolucao; i++){
                retornaDias += *(pDias + i);
            }
        }
        else if (mesPrevisto == 12){
            retornaDias = diaDevolucao + (*(pDias + mesPrevisto) - diaPrevisto);
            for(i=1; i< mesDevolucao; i++){
                retornaDias += *(pDias + i);
            }
        }
        else{
            retornaDias = diaPrevisto + (*(pDias + mesDevolucao) - diaDevolucao);
            retornaDias *= -1;
        }


    }

    if(retornaDias < -7){
        return -10;
    }
    else{
        return retornaDias;
    }
}

void emprestaLivro (aluno *pAluno, livro *pLivro, int condiAluno, int condiLivro){

    aluno *gravaAluno = NULL;
    livro *gravaLivro = NULL;
    aloca_aluno(&gravaAluno, 1);
    aloca_livro(&gravaLivro, 1);
    int posAluno, posLivro, i=0;

    setlocale(LC_ALL, "Portuguese");
    cabecalho(5);

    if (condiLivro == 0 && ((condiAluno == 0) || (condiAluno == 2))){
        pAluno->emprestado++;

        while(i < 3){
            if(((pAluno->tabela+i)->sigla) == 'L'){
                break;
            }
            i++;
        }

        (pAluno->tabela+i)->sigla = 'E';
        (pAluno->tabela+i)->reg = pLivro->reg;
        (pLivro->status)->sigla = 'E';
        strcpy((pLivro->status)->RA, pAluno->RA);
        calculaData(pLivro, 'E', 0, 0);

        printf("\n Livro Emprestado com Sucesso! \n");
        printf("\n Emprestado para: %s", pAluno-> nome);
        printf("\n RA: %s", pAluno-> RA);
        printf("\n N�mero de livros emprestados: %d", pAluno-> emprestado);
        printf("\n N�mero de livros reservados: %d", pAluno-> reservado);
        printf ("\n ----------------------------------------- \n");
        printf("\n Nome do livro: %s", pLivro-> titulo);
        printf("\n Autor: %s", pLivro-> autor);
        printf("\n N�mero de registro: %d", pLivro-> reg);
        printf("\n Data do empr�stimo: %d/%d", (pLivro-> status) -> dia_ret, (pLivro-> status) -> mes_ret);
        printf("\n Data da devolu��o: %d/%d", (pLivro-> status) -> dia_dev, (pLivro-> status) -> mes_dev);
        printf ("\n ----------------------------------------- \n");
	}
    else if(condiLivro == 1 && ((condiAluno == 1) || (condiAluno == 2))){

         pAluno->reservado++;
        (pAluno->tabela+3)->sigla = 'R';
        (pAluno->tabela+3)->reg = pLivro->reg;

        (pLivro->status+1)->sigla = 'R';
        strcpy((pLivro->status+1)->RA, pAluno->RA);
        calculaData(pLivro, 'R', 0, 0);

   		printf("\n Livro Reservado com Sucesso! \n");
        printf("\n Reservado para: %s", pAluno-> nome);
        printf("\n RA: %s", pAluno-> RA);
        printf("\n N�mero de livros emprestados: %d", pAluno-> emprestado);
        printf("\n N�mero de livros reservados: %d", pAluno-> reservado);
        printf ("\n ----------------------------------------- \n");
        printf("\n Nome do livro: %s", pLivro-> titulo);
        printf("\n Autor: %s", pLivro-> autor);
        printf("\n N�mero de registro: %d", pLivro-> reg);
        printf("\n Data do empr�stimo: %d/%d", (pLivro-> status+1) -> dia_ret, (pLivro-> status+1) -> mes_ret);
        printf("\n Data da devolu��o: %d/%d", (pLivro-> status+1) -> dia_dev, (pLivro-> status+1) -> mes_dev);
        printf ("\n ----------------------------------------- \n");
    }
    else{
        printf("\n N�o foi poss�vel concluir o empr�stimo");
        tiraCursor();
        printf("\n\n Pressione qualquer tecla para voltar para o menu...");
        getch();
        return;
    }

    posAluno = buscaPosAluno(gravaAluno, pAluno->RA);
    posLivro = buscaPosLivro(gravaLivro, pLivro->reg);

    *gravaAluno = *pAluno;
    *gravaLivro = *pLivro;

    grava_aluno(gravaAluno, "rb+", posAluno);
    grava_livro(gravaLivro, "rb+", posLivro);

    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

int	 buscaPosAluno(aluno *p, char *RA){

    int i,achou=-1,qreg;
    FILE *fptr=NULL;

    qreg=verifica_aluno();

    if((fptr=fopen("aluno.bin","rb"))==NULL){
       printf("\nErro ao abrir o arquivo\n\n");
    }
    else{
        for(i=0;i<qreg;i++){
            fseek(fptr,i*sizeof(aluno),0);
            fread(p,sizeof(aluno),1,fptr);

            if(strcmp(p->RA,RA) == 0){
              achou=i;
              i=qreg;
            }
        }
      fclose(fptr);
    }

    return achou;
}

int	 buscaPosLivro(livro *p, int reg){

    int i,achou=-1,qreg;
    FILE *fptr=NULL;

    qreg=verifica_livro();

    if((fptr=fopen("livro.bin","rb"))==NULL){
       printf("\nErro ao abrir o arquivo\n\n");
    }
    else{
        for(i=0;i<qreg;i++){
            fseek(fptr,i*sizeof(livro),0);
            fread(p,sizeof(livro),1,fptr);

            if(p->reg == reg){
              achou=i;
              i=qreg;
            }
        }
      fclose(fptr);
    }

    return achou;
}

int validaLivro(livro *pLivro){

    FILE *fptr=NULL;
    int i, k, achouE=-1, achouR= -1,j, condiLivro = -1;
    long int qtd;
    char titulo[80], tituloAux [80], *troca;

    setlocale(LC_ALL, "Portuguese");
    printf("\n Informe o T�tulo do livro: ");
    gets(titulo);
    fflush(stdin);
    qtd = verifica_livro();

    troca = titulo;
    for(i=0; i< strlen(titulo); i++){
        *(troca+i) = toupper(*(troca+i));
    }

    if((fptr=fopen("livro.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquivo");
    else{
        for(i=0;i<qtd;i++){

            fseek(fptr,i*sizeof(livro),0);
            fread(pLivro,sizeof(livro),1,fptr);

            strcpy(tituloAux, pLivro->titulo);
            troca = tituloAux;
            for(k=0; k< strlen(tituloAux); k++){
                *(troca+k) = toupper(*(troca+k));
            }

            if(strcmp(tituloAux,titulo) == 0){

                if(((pLivro->status)->sigla) == 'L'){
                    achouE = i;
                    condiLivro = 0;
                    break;
                }
            }
        }

        if(achouE == -1){

            for(i=0;i<qtd;i++){

                fseek(fptr,i*sizeof(livro),0);
                fread(pLivro,sizeof(livro),1,fptr);

                strcpy(tituloAux, pLivro->titulo);
                troca = tituloAux;
                for(k=0; k< strlen(tituloAux); k++){
                    *(troca+k) = toupper(*(troca+k));
                }

                if(strcmp(tituloAux,titulo) == 0){

                    if(((pLivro->status+1)->sigla) == 'L'){
                        achouR = i;
                        condiLivro = 1;
                        break;
                    }
                }
            }
        }

        if (achouR == -1 && achouE == -1){
            printf("\n Nenhum livro Encontrado.");
            tiraCursor();
            printf("\n\n Pressione qualquer tecla para voltar para o menu...");
            getch();
            condiLivro = 2;
        }


        fclose(fptr);
    }

    return condiLivro;
}
void proibeAluno(void){

    setlocale(LC_ALL, "Portuguese");
    cabecalho(5);
    printf("\n O aluno n�o pode emprestar nem reservar livros");
    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

int validaAluno (aluno *p){

    FILE *fptr=NULL;
    int i, achou=-1,j, retorna = -1, flag = 0;
    long int qtd;
    char ra[7];

    qtd = verifica_aluno();
    setlocale(LC_ALL, "Portuguese");
    cabecalho(5);
    printf("\n Informe o RA do Aluno: ");
    gets(ra);
    fflush(stdin);

    if((fptr=fopen("aluno.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtd;i++){
            fseek(fptr,i*sizeof(aluno),0);
            fread(p,sizeof(aluno),1,fptr);
            if(strcmp(p->RA,ra) == 0){
                achou = i;

                if (p->emprestado < 3){
                    retorna = 0; // retorna = 0;
                    flag = 1;
                }

                if (p->reservado < 1){
                    if (flag == 1){
                         retorna = 2; // retorna = 2;
                    }
                    else{
                        retorna = 1;  // retorna = 1;
                    }
                }

                break;
            }

        }
        if(achou == -1){
            printf("\n Nenhum aluno encontrado");
            tiraCursor();
            printf("\n\n Pressione qualquer tecla para voltar para o menu...");
            getch();
            retorna = 3;
        }

        fclose(fptr);
    }
    return retorna;
}

void busca_aluno_ra(aluno *p){

    FILE *fptr=NULL;
    int i, achou=-1,j, livros=0;
    long int qtd;
    char ra[7];

    qtd = verifica_aluno();
    setlocale(LC_ALL, "Portuguese");
    cabecalho(2);
    printf("\n Informe o RA do Aluno: ");
    gets(ra);
    fflush(stdin);

    if((fptr=fopen("aluno.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtd;i++){
            fseek(fptr,i*sizeof(aluno),0);
            fread(p,sizeof(aluno),1,fptr);
            if(strcmp(p->RA,ra) == 0){
                achou = i;
                printf("\n Nome do aluno: %s", p-> nome);
                printf("\n RA: %s", p-> RA);
                printf("\n N�mero de livros emprestados: %d", p-> emprestado);
                printf("\n N�mero de livros reservados: %d", p-> reservado);
                livros = (p-> emprestado) + (p-> reservado);

                if (livros != 0){
                    printf("\n\n Livros emprestados/reservados: ");
                    for(j=0; j<4; j++){

                        if (((p-> tabela+j)-> sigla) != 'L'){
                            printf("\n Status: %c - Registro: %d", (p-> tabela+j) -> sigla, (p-> tabela+j) -> reg);
                        }
                    }
                }
            }

        }
        if(achou == -1)
            printf("\n Nenhum aluno encontrado");
        fclose(fptr);
    }

    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

void busca_livro_status(livro *p, char status){

    FILE *fptr=NULL;
    int i, achou=-1,j;
    long int qtd;
    qtd = verifica_livro();

    cabecalho(4);

    if((fptr=fopen("livro.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtd;i++){
            fseek(fptr,i*sizeof(livro),0);
            fread(p,sizeof(livro),1,fptr);
            for(j=0;j<2;j++){
                if((((p->status+j)->sigla) == status) && (strcmp(p->titulo, "@") != 0)){
                    achou = j;

                    printf("\n Nome do livro: %s", p-> titulo);
                    printf("\n Autor: %s", p-> autor);
                    printf("\n N�mero de registro: %d", p-> reg);
                    if(((p-> status) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para empr�stimo");
                        break;
                    }
                    else if(((p-> status+j) -> sigla) == 'E'){
                        printf("\n\n O livro est� emprestado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data do empr�stimo: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data da devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+j) -> sigla) == 'R'){
                        printf("\n\n O livro est� reservado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data prevista para retirada: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data prevista para devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+1) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para reserva");
                        break;
                    }
                    printf ("\n ----------------------------------------- \n");
                }

            }
        }

        if(achou == -1)
        printf("\n Nenhum livro encontrado");
        fclose(fptr);
    }



    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

void busca_livro_titulo(livro *p){

    FILE *fptr=NULL;
    int i, k, achou=-1,j;
    long int qtd;
    char titulo[80], tituloAux [80], *troca;

    troca = titulo;

    qtd = verifica_livro();
    cabecalho(4);
    printf("\n Informe o T�tulo do livro: ");
    gets(titulo);
    fflush(stdin);

    for(i=0; i< strlen(titulo); i++){
        *(troca+i) = toupper(*(troca+i));
    }

    if((fptr=fopen("livro.bin","rb")) == NULL)
        printf("\n Erro ao abrir o arquvio");
    else{
        for(i=0;i<qtd;i++){

            fseek(fptr,i*sizeof(livro),0);
            fread(p,sizeof(livro),1,fptr);

            strcpy(tituloAux, p->titulo);
            troca = tituloAux;
            for(k=0; k< strlen(tituloAux); k++){
                *(troca+k) = toupper(*(troca+k));
            }

            if((strcmp(tituloAux,titulo) == 0) && (strcmp(p->titulo, "@") != 0)){
                achou = i;
                printf("\n Nome do livro: %s", p-> titulo);
                printf("\n Autor: %s", p-> autor);
                printf("\n N�mero de registro: %d", p-> reg);

                for(j=0; j<2; j++){
                    if(((p-> status) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para empr�stimo");
                        break;
                    }
                    else if(((p-> status+j) -> sigla) == 'E'){
                        printf("\n\n O livro est� emprestado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data do empr�stimo: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data da devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+j) -> sigla) == 'R'){
                        printf("\n\n O livro est� reservado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data prevista para retirada: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data prevista para devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+1) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para reserva");
                        break;
                    }
                }
                printf ("\n ----------------------------------------- \n");
            }
        }
        if(achou == -1)
            printf("\n Nenhum livro encontrado");
        fclose(fptr);
    }

    tiraCursor();
    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

int verifica_livro(void){

    long int cont = 0;
    FILE *fptr = NULL;
    if ((fptr = fopen("livro.bin", "rb")) == NULL)
        return cont;
    else{
        fseek(fptr, 0, 2);
        cont = ftell(fptr) / sizeof(livro);
        fclose(fptr);
        return cont;
    }
}

int verifica_aluno(void){

    long int cont = 0;
    FILE *fptr = NULL;
    if ((fptr = fopen("aluno.bin", "rb")) == NULL)
        return cont;
    else{
        fseek(fptr, 0, 2);
        cont = ftell(fptr) / sizeof(aluno);
        fclose(fptr);
        return cont;
    }
}

void cadastra_aluno(aluno *paluno){

    int i=0, controle = 0, flag = 0;

    do{
        cabecalho(1);
        if (flag == 1){
            printf("\n Por favor digite uma informa��o v�lida \n");
        }
        else if (flag == 2){
            printf("\n O RA j� est� em uso, por favor tente novamente \n");
        }
        else if (flag == 3){
            printf("\n O formato do RA � inv�lido \n");
        }

        printf("\n Digite o nome do Aluno: ");
        gets(paluno->nome);

        if (validaTexto(paluno ->nome) == 1){
            flag = 1;
            continue;
        }

        printf("\n Digite o RA do Aluno: ");
        gets(paluno->RA);

        if (validaTexto(paluno ->RA) == 1){
            flag = 1;
            continue;
        }

        if (validaRA(paluno ->RA) == 1){
            flag = 2;
            continue;
        }
        else if(validaRA(paluno ->RA) == 2){
            flag = 3;
            continue;
        }
        controle = 1;

    }while (controle == 0);

    paluno->emprestado = 0;
    paluno->reservado = 0;

    for(i=0;i<4;i++){
        (paluno->tabela+i)->sigla = 'L';
    }
    grava_aluno(paluno, "ab", 1);

    tiraCursor();
    printf("\n\n Pressione qualquer tecla para continuar...");
    getch();
}

int validaTexto (char *str){

    if (strlen(str) == 0){
        return 1;
    }
    else{
        return 0;
    }
}
int validaRA(char *RA){

    FILE *fptr=NULL;
    int i, cont;
    cont = verifica_aluno();
    aluno *p = NULL;

    aloca_aluno(&p, 1);

    if (cont == 0){
        return 0;
    }
    if (strlen(RA) != 6){
        return 2;
    }

    if((fptr=fopen("aluno.bin","rb")) == NULL){
        printf("\n Erro ao abrir o arquvio");
    }
    else{
        for(i=0; i<cont; i++){
            fseek(fptr,i*sizeof(aluno),0);
            fread(p,sizeof(aluno),1,fptr);
            if(strcmp(p->RA,RA) == 0){
                return 1;
            }
        }
        fclose(fptr);
        return 0;
    }
}

void cadastraLivro(livro *plivro){

    int i=0, reg, controle = 0, flag = 0;

    reg = verifica_livro();
    reg++;
    (plivro->reg) = reg;

    do{
        cabecalho(3);
        if (flag == 1){
            printf("\n Por favor digite uma informa��o v�lida \n");
        }

        printf("\n Digite o T�tulo do livro: ");
        gets(plivro->titulo);

        if (validaTexto(plivro ->titulo) == 1){
            flag = 1;
            continue;
        }

        printf("\n Digite o Autor do livro: ");
        gets(plivro->autor);

        if (validaTexto(plivro ->autor) == 1){
            flag = 1;
            continue;
        }
        controle = 1;
    }while (controle == 0);

    for(i=0;i<2;i++){
        (plivro->status+i)->sigla = 'L';
    }

    grava_livro(plivro, "ab", 1);
    tiraCursor();

    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

void aloca_aluno(aluno **paluno, int tam){

    if ((*paluno = (aluno *)realloc(*paluno, tam * sizeof(aluno))) == NULL){
        printf("Erro ao alocar. \nEncerrando...");
        exit(1);
    }
}

void aloca_livro(livro **plivro, int tam){

    if ((*plivro = (livro *)realloc(*plivro, tam * sizeof(livro))) == NULL){
        printf("Erro ao alocar. \nEncerrando...");
        exit(1);
    }
}

void grava_aluno(aluno *p, char *str, int pos){

    FILE *fptr = NULL;

    if ((fptr = fopen("aluno.bin", str)) == NULL)
        printf("\nErro ao abrir o arquivo");
    else{
        if (strcmp(str, "rb+") == 0)
            fseek(fptr, pos * sizeof(aluno), 0);
        fwrite(p, sizeof(aluno), 1, fptr);
    }
    fclose(fptr);
}

void grava_livro(livro *p, char *str, int pos){

    FILE *fptr = NULL;

    if ((fptr = fopen("livro.bin", str)) == NULL)
        printf("\nErro ao abrir o arquivo");
    else{
        if (strcmp(str, "rb+") == 0)
            fseek(fptr, pos * sizeof(livro), 0);
        fwrite(p, sizeof(livro), 1, fptr);
    }
    fclose(fptr);
}

void consultaTotalLivros(livro *p){

    int i, j, tamanho;
    FILE *fptr=NULL;

    setlocale(LC_ALL, "Portuguese");
    cabecalho(4);
    tamanho = verifica_livro();

    if((fptr=fopen("livro.bin","rb"))==NULL){
        printf("\nErro ao abrir o arquivo");
    }
    else{
        for(i=0;i<tamanho;i++){

            fseek(fptr,i*sizeof(livro),0);
            fread(p,sizeof(livro),1,fptr);

            if(strcmp(p->titulo, "@") != 0){
                printf("\n Nome do livro: %s", p-> titulo);
                printf("\n Autor: %s", p-> autor);
                printf("\n N�mero de registro: %d", p-> reg);

                for(j=0; j<2; j++){
                    if(((p-> status) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para empr�stimo");
                        break;
                    }
                    else if(((p-> status+j) -> sigla) == 'E'){
                        printf("\n\n O livro est� emprestado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data do empr�stimo: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data da devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+j) -> sigla) == 'R'){
                        printf("\n\n O livro est� reservado");
                        printf("\n RA do aluno: %s", (p-> status+j) -> RA);
                        printf("\n Data prevista para retirada: %d/%d", (p-> status+j) -> dia_ret, (p-> status+j) -> mes_ret);
                        printf("\n Data prevista para devolu��o: %d/%d", (p-> status+j) -> dia_dev, (p-> status+j) -> mes_dev);
                    }
                    else if(((p-> status+1) -> sigla) == 'L'){
                        printf("\n O livro est� dispon�vel para reserva");
                        break;
                    }
                }
                printf ("\n ----------------------------------------- \n");
            }

        }
        fclose(fptr);
    }


    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

void consultaTotalAlunos(aluno *p){

    int i, j, livros = 0, tamanho;
    FILE *fptr=NULL;

    setlocale(LC_ALL, "Portuguese");
    cabecalho(2);
    tamanho = verifica_aluno();

    if((fptr=fopen("aluno.bin","rb"))==NULL){
        printf("\nErro ao abrir o arquivo");
    }
    else{
        for(i=0;i<tamanho;i++){

            fseek(fptr,i*sizeof(aluno),0);
            fread(p,sizeof(aluno),1,fptr);
            printf("\n Nome do aluno: %s", p-> nome);
            printf("\n RA: %s", p-> RA);
            printf("\n N�mero de livros emprestados: %d", p-> emprestado);
            printf("\n N�mero de livros reservados: %d", p-> reservado);
            livros = (p-> emprestado) + (p-> reservado);

            if (livros != 0){
                printf("\n\n Livros emprestados/reservados: ");
                for(j=0; j<4; j++){

                    if (((p-> tabela+j)-> sigla) != 'L'){
                        printf("\n Status: %c - Registro: %d", (p-> tabela+j) -> sigla, (p-> tabela+j) -> reg);
                    }
                }
            }

            printf ("\n ----------------------------------------- \n");
        }
    }


    printf("\n\n Pressione qualquer tecla para voltar para o menu...");
    getch();
}

void calculaData (livro *p, char selec, int diaDevolucao, int mesDevolucao){

    struct tm *data_hora_atual;
    time_t segundos;
    int dias_mes [13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int *pDias = dias_mes;

    time(&segundos);
    data_hora_atual = localtime(&segundos);

    if (selec == 'E'){
        (p->status)-> dia_ret= data_hora_atual->tm_mday;
        (p->status)-> mes_ret= data_hora_atual->tm_mon+1;

        if (((p->status)-> dia_ret)+7 > *(pDias+((p->status)-> mes_ret))){

            (p->status) -> dia_dev = ((p->status)-> dia_ret+7)- *(pDias+((p->status)-> mes_ret));

            if ((p->status)-> mes_ret == 12){
                (p->status)-> mes_dev = 1;
            }
            else{
                (p->status)-> mes_dev = (p->status)-> mes_ret+1;
            }
        }
        else{
            (p->status) -> dia_dev = (p->status)-> dia_ret+7;
            (p->status)-> mes_dev = (p->status)-> mes_ret;
        }

        (p->status)-> sigla = 'E';
    }
    else if (selec == 'R'){
        if (((p->status)-> dia_dev) == *(pDias+((p->status)-> mes_dev))){
            (p->status+1)-> dia_ret = 1;

            if (((p->status)-> mes_dev) == 12){
                (p->status+1)-> mes_ret = 1;
            }
            else{
                (p->status+1)-> mes_ret = ((p->status)-> mes_dev) +1;
            }
        }
        else {
            (p->status+1)-> dia_ret = ((p->status)-> dia_dev) + 1;
            (p->status+1)-> mes_ret = (p->status)-> mes_dev;
        }

        if (((p->status+1)-> dia_ret)+7 > *(pDias+((p->status+1)-> mes_ret))){

            (p->status+1) -> dia_dev = ((p->status+1)-> dia_ret+7)- *(pDias+((p->status+1)-> mes_ret));

            if ((p->status+1)-> mes_ret == 12){
                (p->status+1)-> mes_dev = 1;
            }
            else{
                (p->status+1)-> mes_dev = (p->status+1)-> mes_ret+1;
            }
        }
        else{
            (p->status+1) -> dia_dev = (p->status+1)-> dia_ret+7;
            (p->status+1)-> mes_dev = (p->status+1)-> mes_ret;
        }

        (p->status+1)-> sigla = 'R';
    }
    else if (selec == 'B'){

        if (diaDevolucao+1 > *(pDias+mesDevolucao)){
            diaDevolucao = 1;

            if(mesDevolucao == 12){
                mesDevolucao = 1;
            }
            else{
                mesDevolucao ++;
            }
        }
        else{
            diaDevolucao ++;
        }

        (p->status)-> dia_ret= diaDevolucao;
        (p->status)-> mes_ret= mesDevolucao;

        if (((p->status)-> dia_ret)+7 > *(pDias+((p->status)-> mes_ret))){

            (p->status) -> dia_dev = ((p->status)-> dia_ret+7)- *(pDias+((p->status)-> mes_ret));

            if ((p->status)-> mes_ret == 12){
                (p->status)-> mes_dev = 1;
            }
            else{
                (p->status)-> mes_dev = (p->status)-> mes_ret+1;
            }
        }
        else{
            (p->status) -> dia_dev = (p->status)-> dia_ret+7;
            (p->status)-> mes_dev = (p->status)-> mes_ret;
        }

        (p->status)-> sigla = 'E';
    }
}

int menu(void){

    fflush(stdin);
	setlocale(LC_ALL, "Portuguese");
	int navega=1, recolhe;

    do{
        cabecalho(0);
        printf("  Use w para ir para cima e s para baixo \n");
        tiraCursor();
        opcaoMenuPrincipal(navega);
        fflush(stdin);
        recolhe = getch();
        recolhe = toupper(recolhe);

        if (recolhe == 87){
            if(navega == 1){
                navega =8;
            }
            else{
                navega --;
            }
        }

        if (recolhe == 83){
            if(navega == 8){
                navega =1;
            }
            else{
                navega ++;
            }
        }

    }while (recolhe != 13);

    mostraCursor();
    if (navega == 8){
        tiraCursor();
        cabecalho(0);
        printf("\n At� a pr�xima \n");
    }

    return navega;
}

void opcaoMenuPrincipal(int selec){

    switch(selec){
        case 1:
        printf("\n  >Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 2:
        printf("\n   Cadastra Aluno");
        printf("\n  >Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 3:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n  >Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 4:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n  >Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 5:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n  >Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 6:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n  >Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 7:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n  >Excluir um Livro");
        printf("\n ------------------");
        printf("\n   Sair");
        break;
        case 8:
        printf("\n   Cadastra Aluno");
        printf("\n   Consulta Aluno");
        printf("\n ------------------");
        printf("\n   Cadastra Livro");
        printf("\n   Consulta Livro");
        printf("\n ------------------");
        printf("\n   Empr�stimo de Livros");
        printf("\n   Devolu��o de Livros");
        printf("\n   Excluir um Livro");
        printf("\n ------------------");
        printf("\n  >Sair");
        break;
        default:
        break;
    }

}

void tiraCursor(){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

void mostraCursor(){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 5;
   info.bVisible = TRUE;
   SetConsoleCursorInfo(consoleHandle, &info);
}

int opcaoConsulta(int selec){

    setlocale(LC_ALL, "Portuguese");
	int navega=1, recolhe;

    do{
        cabecalho(selec);
        printf("  Use w para ir para cima e s para baixo \n");
        tiraCursor();

        if (navega == 1){
            printf("\n  >Consulta Total");
            printf("\n   Consulta Parcial");
        }
        else{
            printf("\n   Consulta Total");
            printf("\n  >Consulta Parcial");
        }

        fflush(stdin);
        recolhe = getch();
        recolhe = toupper(recolhe);

        if (recolhe == 87){
            if(navega == 1){
                navega =2;
            }
            else{
                navega --;
            }
        }

        if (recolhe == 83){
            if(navega == 2){
                navega =1;
            }
            else{
                navega ++;
            }
        }



    }while (recolhe != 13);

    return navega;
}

int opcaoParcialLivros(void){


    setlocale(LC_ALL, "Portuguese");
	int navega=1, recolhe;

    do{
        cabecalho(4);
        printf("  Use w para ir para cima e s para baixo \n");
        tiraCursor();

        if (navega == 1){
            printf("\n  >Procurar por T�tulo");
            printf("\n   Consultar por Status");
        }
        else{
            printf("\n   Procurar por T�tulo");
            printf("\n  >Consultar por Status");
        }

        fflush(stdin);
        recolhe = getch();
        recolhe = toupper(recolhe);

        if (recolhe == 87){
            if(navega == 1){
                navega =2;
            }
            else{
                navega --;
            }
        }

        if (recolhe == 83){
            if(navega == 2){
                navega =1;
            }
            else{
                navega ++;
            }
        }



    }while (recolhe != 13);

    return navega;

}
char opcaoStatusLivro (void){

    setlocale(LC_ALL, "Portuguese");
	int navega=1, recolhe;
	char escolha;

    do{
        cabecalho(4);
        printf("  Use w para ir para cima e s para baixo \n");
        tiraCursor();

        if (navega == 1){
            printf("\n  >Livros Dispon�veis");
            printf("\n   Livros Emprestados");
            printf("\n   Livros Reservados");
            escolha = 'L';
        }
        else if (navega == 2){
            printf("\n   Livros Dispon�veis");
            printf("\n  >Livros Emprestados");
            printf("\n   Livros Reservados");
            escolha = 'E';
        }
        else{
            printf("\n   Livros Dispon�veis");
            printf("\n   Livros Emprestados");
            printf("\n  >Livros Reservados");
            escolha = 'R';
        }

        fflush(stdin);
        recolhe = getch();
        recolhe = toupper(recolhe);

        if (recolhe == 87){
            if(navega == 1){
                navega =3;
            }
            else{
                navega --;
            }
        }

        if (recolhe == 83){
            if(navega == 3){
                navega =1;
            }
            else{
                navega ++;
            }
        }



    }while (recolhe != 13);

    return escolha;

}
void cabecalho(int selec){

    setlocale(LC_ALL, "Portuguese");
    system("cls");

    switch(selec){
    case 0:
    printf("\n\t\t\tBiblioteca Facens");
	printf("\n ===============================================================\n");
	break;
	case 1:
    printf("\n               Biblioteca Facens / Cadastra Aluno");
	printf("\n ===============================================================\n");
	break;
	case 2:
    printf("\n                    Biblioteca Facens / Consulta Aluno");
	printf("\n ========================================================================\n");
	break;
	case 3:
    printf("\n                    Biblioteca Facens / Cadastra Livro");
	printf("\n ========================================================================\n");
	break;
	case 4:
    printf("\n                    Biblioteca Facens / Consulta Livro");
	printf("\n ========================================================================\n");
	break;
	case 5:
    printf("\n                    Biblioteca Facens / Empr�stimos de Livros");
	printf("\n ========================================================================\n");
	break;
	case 6:
    printf("\n                    Biblioteca Facens / Devolu��o de Livros");
	printf("\n ========================================================================\n");
	break;
	case 7:
    printf("\n                    Biblioteca Facens / Excluir um Livro");
	printf("\n ========================================================================\n");
	break;
    }
}

void gotoxy(int x, int y) {
  COORD c;
  c.X = x - 1;
  c.Y = y - 1;
  SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c);
}
