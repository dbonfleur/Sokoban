#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string.h>
#include <conio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#define DIR 0.006
#define HOST "localhost"
#define USER "root"
#define PASS "D4rkS1d30fM00n"
#define DB "sokoban"

ALLEGRO_BITMAP *img=NULL;
ALLEGRO_BITMAP *men=NULL;
ALLEGRO_BITMAP *caixa=NULL;
ALLEGRO_BITMAP *fmenu=NULL;
ALLEGRO_BITMAP *space=NULL;
ALLEGRO_BITMAP *setas=NULL;
ALLEGRO_BITMAP *mov=NULL;
ALLEGRO_BITMAP *mini=NULL;
ALLEGRO_DISPLAY *janela=NULL;
ALLEGRO_FONT *fonte=NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer=NULL;
MYSQL cnx;

int quant;
int tami;
int tamj;
int pi;
int pj;
int LARG_TELA;
int ALT_TELA;
int BIT_PI;
int BIT_PJ;

int soma;
int **mapa;
int fase=1;
int sair=0;
int venc=0;
int flag2=0;
int movs=0;
int usr;

int altura_sprite=50, largura_sprite=50;
int colunas_folha=4, coluna_atual=0;
int linhas_folha=2, linha_atual=0;
int regiao_x_folha=0, regiao_y_folha=0;
int frames_sprite=6, cont_frames=0;
int vel_x_sprite=0, vel_y_sprite=0;

struct Cax {
    int bpi;
    int bpj;
    int pi;
    int pj;
};

typedef struct Cax CAX;
CAX **cax;

char* mysql_pesq(char *comando) {
    int res, i;
    char linha[500];
    MYSQL_RES *pesq;
    MYSQL_ROW linhas;
    res=mysql_query(&cnx, comando);
    if(!res) {
        pesq=mysql_store_result(&cnx);
        if(pesq) {
            while((linhas=mysql_fetch_row(pesq))!=NULL) {
                for(i=0;i<mysql_num_fields(pesq);i++) {
                    strcpy(linha, linhas[i]);
                }
            }
            mysql_free_result(pesq);
        }
    }
    return linha;
}

void alocar_pont() {
    int res, i;
    char comando[60];
    sprintf(comando, "select tb_cax.quant from tb_cax where tb_cax.cod_map=%d", fase);
    quant=atoi(mysql_pesq(comando));
    cax = malloc(quant * sizeof(CAX*));
    for(i=0;i<quant;i++) {
        cax[i] = malloc(sizeof(CAX));
    }
}

void libera_pont() {
    int i;
    for(i=0;i<quant;i++) {
        free(cax[i]);
    }
    free(cax);
}

void infos_map() {
    int i, j, cont=0;
	char map[500], comando[100], aux[5];
	sprintf(comando, "select tb_map.tami from tb_map where cod_map=%d", fase);
	tami=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.tamj from tb_map where cod_map=%d", fase);
	tamj=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.pi from tb_map where cod_map=%d", fase);
	pi=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.pj from tb_map where cod_map=%d", fase);
	pj=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.larg_tl from tb_map where cod_map=%d", fase);
	LARG_TELA=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.alt_tl from tb_map where cod_map=%d", fase);
	ALT_TELA=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.bit_pi from tb_map where cod_map=%d", fase);
	BIT_PI=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.bit_pj from tb_map where cod_map=%d", fase);
	BIT_PJ=atoi(mysql_pesq(comando));
	sprintf(comando, "select tb_map.mapa from tb_map where cod_map=%d", fase);
	strcpy(map, mysql_pesq(comando));
	mapa = (int **) calloc (tami, sizeof(int *));
    if (mapa == NULL) {
		printf ("Memoria Insuficiente!\n");
		exit(1);
    }
	for (i=0;i<tami;i++) {
		mapa[i] = (int*) calloc (tamj, sizeof(int));
		if (mapa[i] == NULL) {
            printf ("Memoria Insuficiente!");
			exit(1);
        }
    }
    for(i=0;i<tami;i++) {
        for(j=0;j<tamj;j++) {
            aux[0]=map[cont];
            aux[1]='\0';
            mapa[i][j]=atoi(aux);
            cont++;
        }
    }
    cont=0;
}

void libera_mapa () {
    int  i;
    for (i=0; i<tami; i++) {
        free(mapa[i]);
    }
    free(mapa);
    mapa=NULL;
}

void error_msg(char *text){
    //al_show_native_message_box(janela,"ERRO","Ocorreu o seguinte erro e o programa sera finalizado:",text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int ini_alle() {
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }
    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar o Addon");
        return 0;
    }
    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }
    al_init_font_addon();
    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }
    fonte = al_load_font("neoletters.ttf", 36, 0);
    if (!fonte){
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }
    fila_eventos = al_create_event_queue();
    if (!fila_eventos){
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }
    al_set_window_title(janela, "Sokoban");
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    return 1;
}

int ini_parametros() {
    infos_map();
    char skb[20];
    janela = al_create_display(LARG_TELA, ALT_TELA);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 490, 170);
    sprintf(skb, "sokoban%d.png", fase);
    img=al_load_bitmap(skb);
	if(!img) {
		error_msg("Erro ao carregar o sokoban.png");
		al_destroy_display(janela);
        return 0;
	}
	al_draw_bitmap(img, 0, 0, 0);
	men=al_load_bitmap("men_fre.png");
	if(!men) {
		error_msg("Erro ao carregar o men_fre.png");
		al_destroy_display(janela);
        return 0;
	}
	al_draw_bitmap(men, BIT_PJ, BIT_PI, 0);
	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    alocar_pont();
    ini_cax();
    al_flip_display();
    return 1;
}

void destroi() {
    al_destroy_bitmap(men);
    al_destroy_bitmap(img);
    al_destroy_bitmap(caixa);
    al_destroy_display(janela);
    libera_mapa();
    libera_pont();
}

void ini_cax() {
    int i, j, res, cont=0, cont2=0;
    char bpi[500], bpj[500], pi[500], pj[500], aux[6], comando[100];
    sprintf(comando, "select tb_cax.soma from tb_cax where tb_cax.cod_map=%d", fase);
    soma=atoi(mysql_pesq(comando));
    for(j=0;j<4;j++) {
        switch(j) {
        case 0:
            sprintf(comando, "select tb_cax.bpi from tb_cax where tb_cax.cod_map=%d", fase);
            strcpy(bpi, mysql_pesq(comando));
            i=0;
            do {
                if(bpi[i]!=',' && bpi[i]!='\0') {
                    aux[cont2]=bpi[i];
                    cont2++;
                } else {
                    aux[cont2]='\0';
                    cax[cont]->bpi=atoi(aux);
                    cont2=0;
                    cont++;
                }
            }while(bpi[i++]!='\0');
            cont2=0;
            cont=0;
            break;
        case 1:
            sprintf(comando, "select tb_cax.bpj from tb_cax where tb_cax.cod_map=%d", fase);
            strcpy(bpj, mysql_pesq(comando));
            i=0;
            do {
                if(bpj[i]!=',' && bpj[i]!='\0') {
                    aux[cont2]=bpj[i];
                    cont2++;
                } else {
                    aux[cont2]='\0';
                    cax[cont]->bpj=atoi(aux);
                    cont++;
                    cont2=0;
                }
            }while(bpj[i++]!='\0');
            cont2=0;
            cont=0;
            i=0;
            break;
        case 2:
            sprintf(comando, "select tb_cax.pi from tb_cax where tb_cax.cod_map=%d", fase);
            strcpy(pi, mysql_pesq(comando));
            i=0;
            do {
                if(pi[i]!=',' && pi[i]!='\0') {
                    aux[cont2]=pi[i];
                    cont2++;
                } else {
                    aux[cont2]='\0';
                    cax[cont]->pi=atoi(aux);
                    cont++;
                    cont2=0;
                }
            }while(pi[i++]!='\0');
            cont2=0;
            cont=0;
            break;
        case 3:
            sprintf(comando, "select tb_cax.pj from tb_cax where tb_cax.cod_map=%d", fase);
            strcpy(pj, mysql_pesq(&comando));
            i=0;
            do {
                if(pj[i]!=',' && pj[i]!='\0') {
                    aux[cont2]=pj[i];
                    cont2++;
                } else {
                    aux[cont2]='\0';
                    cax[cont]->pj=atoi(aux);
                    cont++;
                    cont2=0;
                }
            }while(pj[i++]!='\0');
            cont2=0;
            cont=0;
            break;
        }
    }
    caixa=al_load_bitmap("caixa.png");
    if(!caixa) {
		error_msg("Erro ao carregar o caixa.png");
		al_destroy_display(janela);
	}
	des_cax();
}

void des_cax() {
    int i;
    for(i=0;i<quant;i++) {
        al_draw_bitmap(caixa, cax[i]->bpj, cax[i]->bpi, 0);
    }
}

void anima_mov(int direcao) {
    cont_frames++;
    if (cont_frames >= frames_sprite){
        cont_frames=0;
        coluna_atual++;
        if (coluna_atual >= colunas_folha){
            coluna_atual=0;
            linha_atual = (linha_atual+1) % linhas_folha;
            regiao_y_folha = linha_atual * altura_sprite;
        }
        regiao_x_folha = coluna_atual * largura_sprite;
    }
    al_draw_bitmap(img, 0, 0, 0);
    if(direcao) {
        al_draw_bitmap_region(mov,regiao_x_folha,regiao_y_folha,largura_sprite,altura_sprite,BIT_PJ,BIT_PI,0);
        des_cax();
    } else {
        des_cax();
        al_draw_bitmap_region(mov,regiao_x_folha,regiao_y_folha,largura_sprite,altura_sprite,BIT_PJ,BIT_PI,0);
    }
    al_flip_display();
    al_rest(DIR);
}

int test_venc() {
    int i, aux, som=0;
    for(i=0;i<quant;i++) {
        aux=som+mapa[cax[i]->pi][cax[i]->pj];
        som=aux;
    }
    if(som==soma) {
        return 1;
    } else {
        return 0;
    }
}

int menu() {
    int tec=1, ent=0;
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 390, 180);
    fmenu=al_load_bitmap("menu.png");
	if(!fmenu) {
		error_msg("Erro ao carregar o menu.png");
		al_destroy_display(janela);
        return 0;
	}
	al_draw_bitmap(fmenu,0,0,0);
    al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 300, ALLEGRO_ALIGN_CENTRE, "Instrucoes");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 350, ALLEGRO_ALIGN_CENTRE, "Recordes");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 400, ALLEGRO_ALIGN_CENTRE, "Sair");
    al_flip_display();
	while (!ent){
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                case ALLEGRO_KEY_UP:
                    if(tec>1) {
                        tec--;
                    }
                    break;
                case ALLEGRO_KEY_DOWN:
                    if(tec<4) {
                        tec++;
                    }
                    break;
                case ALLEGRO_KEY_ENTER:
                    ent=1;
                    break;
                }
            }
            if(!ent) {
                switch(tec) {
                    case 1:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 300, ALLEGRO_ALIGN_CENTRE, "Instrucoes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 350, ALLEGRO_ALIGN_CENTRE, "Recordes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 400, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                    case 2:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 300, ALLEGRO_ALIGN_CENTRE, "Instrucoes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 350, ALLEGRO_ALIGN_CENTRE, "Recordes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 400, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                    case 3:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 300, ALLEGRO_ALIGN_CENTRE, "Instrucoes");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 350, ALLEGRO_ALIGN_CENTRE, "Recordes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 400, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                    case 4:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 300, ALLEGRO_ALIGN_CENTRE, "Instrucoes");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 350, ALLEGRO_ALIGN_CENTRE, "Recordes");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 400, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                }
            }
        }
	}
	al_destroy_bitmap(fmenu);
	al_destroy_display(janela);
    return tec;
}

void inst() {
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
    }
    al_set_window_position(janela, 390, 180);
    space=al_load_bitmap("space.png");
    if(!space) {
        error_msg("Erro ao carregar o space.png");
        al_destroy_display(janela);
    }
    setas=al_load_bitmap("setas.png");
    if(!setas) {
        error_msg("Erro ao carregar o setas.png");
        al_destroy_display(janela);
    }
    al_draw_bitmap(space,0,0,0);
    al_draw_bitmap(setas,100,30,0);
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 420, 80, ALLEGRO_ALIGN_CENTRE, "- (Movimento)");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 320, 200, ALLEGRO_ALIGN_CENTRE, "R - (Repetir)");
    al_flip_display();
    al_rest(2.5);
    al_destroy_bitmap(setas);
    al_destroy_bitmap(space);
    al_destroy_display(janela);
}

int user() {
    int tec=1, ent=0;
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 390, 180);
    fmenu=al_load_bitmap("space.png");
	if(!fmenu) {
		error_msg("Erro ao carregar o space.png");
		al_destroy_display(janela);
        return 0;
	}
	al_draw_bitmap(fmenu,0,0,0);
    al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 180, ALLEGRO_ALIGN_CENTRE, "Criar usuario");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 230, ALLEGRO_ALIGN_CENTRE, "Existente");
    al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 280, ALLEGRO_ALIGN_CENTRE, "Sair");
    al_flip_display();
    while (ent!=1){
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                case ALLEGRO_KEY_UP:
                    if(tec>1) {
                        tec--;
                    }
                    break;
                case ALLEGRO_KEY_DOWN:
                    if(tec<3) {
                        tec++;
                    }
                    break;
                case ALLEGRO_KEY_ENTER:
                    ent=1;
                    break;
                }
            }
            if(!ent) {
                switch(tec) {
                    case 1:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 180, ALLEGRO_ALIGN_CENTRE, "Criar usuario");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 230, ALLEGRO_ALIGN_CENTRE, "Existente");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 280, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                    case 2:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 180, ALLEGRO_ALIGN_CENTRE, "Criar usuario");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 230, ALLEGRO_ALIGN_CENTRE, "Existente");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 280, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                    case 3:
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 180, ALLEGRO_ALIGN_CENTRE, "Criar usuario");
                        al_draw_text(fonte, al_map_rgb(207, 207, 207), 325, 230, ALLEGRO_ALIGN_CENTRE, "Existente");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 325, 280, ALLEGRO_ALIGN_CENTRE, "Sair");
                        al_flip_display();
                        break;
                }
            } else {
                al_destroy_display(janela);
                al_destroy_bitmap(fmenu);
                switch(tec) {
                    case 1:
                        return cria_user();
                        break;
                    case 2:
                        return exist_user();
                        break;
                    case 3:
                        return -1;
                }
            }
        }
	}

}

int cria_user() {
    MYSQL_RES *pesq;
    MYSQL_ROW linhas;
    int tec=0, cont=0, res, num;
    char let[3], nome[15], comando[100];
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 390, 180);
    fmenu=al_load_bitmap("space.png");
	if(!fmenu) {
		error_msg("Erro ao carregar o space.png");
		al_destroy_display(janela);
        return 0;
	}
	al_draw_bitmap(fmenu, 0, 0, 0);
    al_draw_text(fonte, al_map_rgb(148, 0, 211), 150, 175, ALLEGRO_ALIGN_CENTRE, "Nome: ");
    al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 250, ALLEGRO_ALIGN_CENTRE, "*Aperte enter,");
    al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 290, ALLEGRO_ALIGN_CENTRE, "para continuar*");
    al_flip_display();
    while (tec!=67) {
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                tec=evento.keyboard.keycode;
                if(tec>0 && tec<27) {
                    let[0]=96+tec;
                    let[1]='\0';
                    if(cont<14) {
                        nome[cont]=let[0];
                        nome[cont+1]='\0';
                        al_draw_bitmap(fmenu, 0, 0, 0);
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 150, 175, ALLEGRO_ALIGN_CENTRE, "Nome: ");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 250, ALLEGRO_ALIGN_CENTRE, "*Aperte enter,");
                        al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 290, ALLEGRO_ALIGN_CENTRE, "para continuar*");
                        al_draw_textf(fonte, al_map_rgb(148, 0, 211), 200, 175, 0, "%s", nome);
                        al_flip_display();
                        cont++;
                    }
                } else if(tec==63 && cont>0) {
                    cont--;
                    nome[cont]='\0';
                    al_draw_bitmap(fmenu, 0, 0, 0);
                    al_draw_text(fonte, al_map_rgb(148, 0, 211), 150, 175, ALLEGRO_ALIGN_CENTRE, "Nome: ");
                    al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 250, ALLEGRO_ALIGN_CENTRE, "*Aperte enter,");
                    al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 290, ALLEGRO_ALIGN_CENTRE, "para continuar*");
                    al_draw_textf(fonte, al_map_rgb(148, 0, 211), 200, 175, 0, "%s", nome);
                    al_flip_display();
                } else if(tec==59) {
                    return -1;
                }
            }
        }
    }
    if(cont!=0) {
        sprintf(comando, "insert into tb_user(nome) values ('%s')", nome);
        res=mysql_query(&cnx, comando);
        if(!res) {
            sprintf(comando, "select tb_user.cod_user from tb_user where tb_user.nome='%s'", nome);
            res=mysql_query(&cnx, comando);
            if(!res) {
                pesq=mysql_store_result(&cnx);
                if(pesq) {
                    while((linhas=mysql_fetch_row(pesq))!=NULL) {
                        num=atoi(linhas[0]);
                    }
                    al_destroy_display(janela);
                    al_destroy_bitmap(fmenu);
                    mysql_free_result(pesq);
                    return num;
                }
            }
        }
    } else {
        al_destroy_display(janela);
        al_destroy_bitmap(fmenu);
        return 0;
    }
}

int exist_user() {
    int quant, res, cod, tec, y=100, i;
    char nome[15];
    MYSQL_RES *pesq;
    MYSQL_ROW linhas;
    quant=atoi(mysql_pesq("select COUNT(*) from tb_user"));
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 390, 180);
    fmenu=al_load_bitmap("space.png");
	if(!fmenu) {
		error_msg("Erro ao carregar o space.png");
		al_destroy_display(janela);
        return 0;
	}
    res=mysql_query(&cnx, "select * from tb_user");
    if(!res) {
        pesq=mysql_store_result(&cnx);
        if(pesq) {
            if(!quant) {
                al_draw_bitmap(fmenu,0,0,0);
                al_draw_text(fonte, al_map_rgb(148, 0, 211), 320, 220, ALLEGRO_ALIGN_CENTRE, "Sem Registros!");
                al_flip_display();
                al_rest(2);
                al_destroy_bitmap(fmenu);
                al_destroy_display(janela);
                return 0;
            } else {
                al_draw_bitmap(fmenu,0,0,0);
                al_draw_text(fonte, al_map_rgb(148, 0, 211), 150, 50, ALLEGRO_ALIGN_CENTRE, "Codigo");
                al_draw_text(fonte, al_map_rgb(148, 0, 211), 370, 50, 0, "Nome");
            }
            while((linhas=mysql_fetch_row(pesq))!=NULL) {
                for(i=0;i<mysql_num_fields(pesq);i++) {
                    if(!i%2) {
                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 150, y, ALLEGRO_ALIGN_CENTRE, "%s", linhas[i]);
                    } else {
                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 350, y, 0, "%s", linhas[i]);
                        y+=40;
                    }
                    al_flip_display();
                }
            }
            mysql_free_result(pesq);
        }
    }
    while (tec!=59) {
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                tec=evento.keyboard.keycode;
                if((tec-37)>0 && (tec-37)<=quant) {
                    al_destroy_bitmap(fmenu);
                    al_destroy_display(janela);
                    return (tec-37);
                } else if((tec-27)>0 && (tec-27)<=quant) {
                    al_destroy_bitmap(fmenu);
                    al_destroy_display(janela);
                    return (tec-27);
                }
            }
        }
    }
    al_destroy_bitmap(fmenu);
    al_destroy_display(janela);
    return 0;
}

void movimentos() {
    int teste, res, i;
    char comando[100];
    MYSQL_RES *pesq;
    MYSQL_ROW linhas;
    sprintf(comando, "select COUNT(*) from tb_movs where cod_map=%d and cod_user=%d", fase, usr);
    res=mysql_query(&cnx, comando);
    if(!res) {
        pesq=mysql_store_result(&cnx);
        if(pesq) {
            while((linhas=mysql_fetch_row(pesq))!=NULL) {
                for(i=0;i<mysql_num_fields(pesq);i++) {
                    teste=atoi(linhas[i]);
                }
            }
            mysql_free_result(pesq);
        }
    }
    if(!teste) {
        sprintf(comando, "insert into tb_movs(cod_map, cod_user, movimentos) values (%d, %d, %d)", fase, usr, movs);
        res=mysql_query(&cnx, comando);
    } else {
        sprintf(comando, "select tb_movs.movimentos from tb_movs where cod_map=%d and cod_user=%d", fase, usr);
        res=mysql_query(&cnx, comando);
        if(!res) {
            pesq=mysql_store_result(&cnx);
            if(pesq) {
                while((linhas=mysql_fetch_row(pesq))!=NULL) {
                    for(i=0;i<mysql_num_fields(pesq);i++) {
                        teste=atoi(linhas[i]);
                    }
                }
                mysql_free_result(pesq);
            }
        }
        if(movs<teste) {
            sprintf(comando, "update tb_movs set movimentos=%d where cod_user=%d and cod_map=%d", movs, usr, fase);
            mysql_query(&cnx, comando);
        }
    }
}

int mexer(int movi, int box) {
    int frame=0;
    char image[15];
    if(box) {
        sprintf(image, "mov_cax%d.bmp", movi);
        mov=al_load_bitmap(image);
        if (!mov){
            error_msg("Falha ao carregar mov_cax.bmp");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        al_convert_mask_to_alpha(mov,al_map_rgb(255,0,255));
    } else {
        sprintf(image, "mov%d.bmp", movi);
        mov=al_load_bitmap(image);
        if (!mov){
            error_msg("Falha ao carregar mov.bmp");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        al_convert_mask_to_alpha(mov,al_map_rgb(255,0,255));
    }
    while(frame<50) {
        switch(movi) {
        case 1:
            BIT_PI--;
            if(box) {
                if(frame>24) {
                    cax[box-3]->bpi-=2;
                    al_rest(0.004);
                }
            }
            anima_mov(0);
            break;
        case 2:
            BIT_PI++;
            if(box) {
                if(frame>24) {
                    cax[box-3]->bpi+=2;
                    al_rest(0.004);
                }
            }
            anima_mov(1);
            break;
        case 3:
            BIT_PJ--;
            if(box) {
                cax[box-3]->bpj--;
            }
            anima_mov(0);
            break;
        case 4:
            BIT_PJ++;
            if(box) {
                cax[box-3]->bpj++;
            }
            anima_mov(0);
        }
        frame++;
    }
}

void jogo() {
    int tecla=0;
    int flag=0;
    int aux;
    while(sair!=1 && fase<5) {
        if (!ini_parametros()){
            return -1;
        }
        while (sair!=1 && venc!=1){
            while(!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);
                if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                    switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_UP:
                        if(mapa[pi-1][pj]==0) {
                            mapa[pi][pj]=0;
                            pi--;
                            mapa[pi][pj]=2;
                            mexer(1,0);
                            movs++;
                        } else if((mapa[pi-1][pj]>=3 && mapa[pi-1][pj]<=quant+2) && mapa[pi-2][pj]==0) {
                            mapa[pi][pj]=0;
                            pi--;
                            aux=mapa[pi][pj];
                            mapa[pi][pj]=2;
                            mapa[pi-1][pj]=aux;
                            mexer(1,aux);
                            movs++;
                        }
                        al_destroy_bitmap(men);
                        men=al_load_bitmap("men_cos.png");
                        if(!men) {
                            error_msg("Erro ao carregar o men_cos.png");
                            al_destroy_display(janela);
                        }
                        break;
                    case ALLEGRO_KEY_DOWN:
                        if(mapa[pi+1][pj]==0) {
                            mapa[pi][pj]=0;
                            pi++;
                            mapa[pi][pj]=2;
                            mexer(2,0);
                            movs++;
                        } else if((mapa[pi+1][pj]>=3 && mapa[pi+1][pj]<=quant+2) && mapa[pi+2][pj]==0) {
                            mapa[pi][pj]=0;
                            pi++;
                            aux=mapa[pi][pj];
                            mapa[pi][pj]=2;
                            mapa[pi+1][pj]=aux;
                            mexer(2,aux);
                            movs++;
                        }
                        al_destroy_bitmap(men);
                        men=al_load_bitmap("men_fre.png");
                        if(!men) {
                            error_msg("Erro ao carregar o men_fre.png");
                            al_destroy_display(janela);
                        }
                        break;
                    case ALLEGRO_KEY_LEFT:
                        if(mapa[pi][pj-1]==0) {
                            mapa[pi][pj]=0;
                            pj--;
                            mapa[pi][pj]=2;
                            mexer(3,0);
                            movs++;
                        } else if((mapa[pi][pj-1]>=3 && mapa[pi][pj-1]<=quant+2) && mapa[pi][pj-2]==0) {
                            mapa[pi][pj]=0;
                            pj--;
                            aux=mapa[pi][pj];
                            mapa[pi][pj]=2;
                            mapa[pi][pj-1]=aux;
                            mexer(3,aux);
                            movs++;
                        }
                        al_destroy_bitmap(men);
                        men=al_load_bitmap("men_esq.png");
                        if(!men) {
                            error_msg("Erro ao carregar o men_esq.png");
                            al_destroy_display(janela);
                        }
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        if(mapa[pi][pj+1]==0) {
                            mapa[pi][pj]=0;
                            pj++;
                            mapa[pi][pj]=2;
                            mexer(4,0);
                            movs++;
                        } else if((mapa[pi][pj+1]>=3 && mapa[pi][pj+1]<=quant+2) && mapa[pi][pj+2]==0) {
                            mapa[pi][pj]=0;
                            pj++;
                            aux=mapa[pi][pj];
                            mapa[pi][pj]=2;
                            mapa[pi][pj+1]=aux;
                            mexer(4,aux);
                            movs++;
                        }
                        al_destroy_bitmap(men);
                        men=al_load_bitmap("men_dir.png");
                        if(!men) {
                            error_msg("Erro ao carregar o men_dir.png");
                            al_destroy_display(janela);
                        }
                        break;
                    case ALLEGRO_KEY_R:
                        destroi();
                        if (!ini_parametros()){
                            return -1;
                        }
                        movs=0;
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair=1;
                    }
                    al_draw_bitmap(img, 0, 0, 0);
                    al_draw_bitmap(men, BIT_PJ, BIT_PI, 0);
                    des_cax();
                    al_flip_display();
                } else if (evento.type == ALLEGRO_EVENT_KEY_UP){
                    tecla=6;
                } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    sair=1;
                }
                venc=test_venc();
            }
        }
        if(venc) {
            al_rest(1);
            movimentos();
            destroi();
            fase++;
            movs=0;
            venc=0;
        } else {
            destroi();
        }
    }
    sair=0;
    fase=1;
}

void recordes() {
    int tec=1, ent=0, flag=1, sair=0, res, alt, i;
    int larg_mini;
    char img[20], comando[150];
    MYSQL_RES *pesq;
    MYSQL_ROW linhas;
    janela=al_create_display(640,480);
    if (!janela){
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_position(janela, 390, 180);
    fmenu=al_load_bitmap("space.png");
	if(!fmenu) {
		error_msg("Erro ao carregar o space.png");
		al_destroy_display(janela);
        return 0;
	}
	sprintf(img, "mini%d.png", tec);
    mini=al_load_bitmap(img);
    if(!mini) {
        error_msg("Erro ao carregar o mini.png");
        al_destroy_display(janela);
        return 0;
    }
	while (!ent && !sair){
        while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                switch(evento.keyboard.keycode){
                    case ALLEGRO_KEY_RIGHT:
                        if(tec<4) {
                            tec++;
                            al_destroy_bitmap(mini);
                            sprintf(img, "mini%d.png", tec);
                            mini=al_load_bitmap(img);
                            if(!mini) {
                                error_msg("Erro ao carregar o mini.png");
                                al_destroy_display(janela);
                                return 0;
                            }
                            flag=1;
                        }
                        break;
                    case ALLEGRO_KEY_LEFT:
                        if(tec>1) {
                            tec--;
                            al_destroy_bitmap(mini);
                            sprintf(img, "mini%d.png", tec);
                            mini=al_load_bitmap(img);
                            if(!mini) {
                                error_msg("Erro ao carregar o mini.png");
                                al_destroy_display(janela);
                                return 0;
                            }
                            flag=1;
                        }
                        break;
                    case ALLEGRO_KEY_ENTER:
                        alt=50;
                        al_draw_bitmap(fmenu,0,0,0);
                        al_draw_textf(fonte, al_map_rgb(148, 0, 211), 120, alt, 0, "Nomes");
                        al_draw_textf(fonte, al_map_rgb(148, 0, 211), 400, alt, ALLEGRO_ALIGN_CENTER, "Movimentos");
                        sprintf(comando, "select tb_user.nome, tb_movs.movimentos from tb_user, tb_movs where tb_movs.cod_user=tb_user.cod_user and tb_movs.cod_map=%d", tec);
                        res=mysql_query(&cnx, comando);
                        alt+=30;
                        if(!res) {
                            pesq=mysql_store_result(&cnx);
                            if(pesq) {
                                while((linhas=mysql_fetch_row(pesq))!=NULL) {
                                    for(i=0;i<mysql_num_fields(pesq);i+=2) {
                                        alt+=40;
                                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 120, alt, 0, "%s", linhas[i]);
                                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 400, alt, ALLEGRO_ALIGN_CENTER, "%s", linhas[i+1]);
                                    }
                                }
                                mysql_free_result(pesq);
                            }
                        }
                        al_flip_display();
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair++;
                }
            }
            if(flag) {
                sprintf(comando, "select tb_map.larg_mini from tb_map where cod_map=%d", tec);
                larg_mini=atoi(mysql_pesq(comando));
                al_draw_bitmap(fmenu,0,0,0);
                al_draw_textf(fonte, al_map_rgb(148, 0, 211), 320, 80, ALLEGRO_ALIGN_CENTER, "Fase %d", tec);
                al_draw_bitmap(mini, (320-(larg_mini/2)), 160, 0);
                al_flip_display();
                flag=0;
            }
        }
	}
	/*
	if(ent) {
        al_draw_bitmap(fmenu,0,0,0);
        al_draw_textf(fonte, al_map_rgb(148, 0, 211), 120, alt, 0, "Nomes");
        al_draw_textf(fonte, al_map_rgb(148, 0, 211), 400, alt, ALLEGRO_ALIGN_CENTER, "Movimentos");
        sprintf(comando, "select tb_user.nome, tb_movs.movimentos from tb_user, tb_movs where tb_movs.cod_user=tb_user.cod_user and tb_movs.cod_map=%d", tec);
        res=mysql_query(&cnx, comando);
        alt+=30;
        if(!res) {
            pesq=mysql_store_result(&cnx);
            if(pesq) {
                while((linhas=mysql_fetch_row(pesq))!=NULL) {
                    for(i=0;i<mysql_num_fields(pesq);i+=2) {
                        alt+=40;
                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 120, alt, 0, "%s", linhas[i]);
                        al_draw_textf(fonte, al_map_rgb(207, 207, 207), 400, alt, ALLEGRO_ALIGN_CENTER, "%s", linhas[i+1]);
                    }
                }
                mysql_free_result(pesq);
            }
        }
        al_flip_display();
        while(!sair) {
            while(!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);
                if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
                    if(evento.keyboard.keycode==59) {
                        sair=1;
                    }
                }
            }
        }
	}
	*/
	al_destroy_display(janela);
}

int fechar() {
    al_destroy_display(janela);
    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    mysql_close(&cnx);
    return 0;
}

int main(void) {
    if (!ini_alle()){
        return -1;
    }
    mysql_init(&cnx)
    mysql_real_connect(&cnx,HOST,USER,PASS,DB,0,NULL,0);
    do {
        usr=user();
        if(usr==-1) {
            return fechar();
        }
    }while(!usr);
    do {
        switch(menu()) {
            case 1:
                jogo();
                break;
            case 2:
                inst();
                break;
            case 3:
                recordes();
                break;
            case 4:
                return fechar();
        }
    }while(1);
}
