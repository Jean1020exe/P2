#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CLIENTES      100
#define MAX_PRODUTOS      200
#define MAX_PEDIDOS       500
#define MAX_ITENS_PEDIDO   20
#define TAM_NOME          100
#define TAM_EMAIL          80
#define TAM_CPF            15
#define TAM_CARTAO         20
#define TAM_STRING         50

typedef enum {
    CARTAO_CREDITO = 1,
    CARTAO_DEBITO,
    PIX,
    BOLETO
} FormaPagamento;

typedef enum {
    PENDENTE = 1,
    APROVADO,
    CANCELADO,
    ENVIADO,
    ENTREGUE
} StatusPedido;

typedef struct {
    int    id;
    char   nome[TAM_NOME];
    char   email[TAM_EMAIL];
    char   cpf[TAM_CPF];
    char   telefone[TAM_STRING];
    char   endereco[TAM_NOME];
} Cliente;

typedef struct {
    int    id;
    char   nome[TAM_NOME];
    char   categoria[TAM_STRING];
    double preco;
    int    estoque;
} Produto;

typedef struct {
    int    produto_id;
    char   nome_produto[TAM_NOME];
    int    quantidade;
    double preco_unitario;
    double subtotal;
} ItemPedido;

typedef struct {
    char   numero[TAM_CARTAO];
    char   titular[TAM_NOME];
    char   validade[10];
    char   cvv[5];
    int    parcelas;
} DadosCartao;

typedef struct {
    char   chave[TAM_EMAIL];
} DadosPix;

typedef struct {
    FormaPagamento forma;
    DadosCartao    cartao;
    DadosPix       pix;
    double         valor_total;
    int            aprovado;
} Pagamento;

typedef struct {
    int          id;
    int          cliente_id;
    char         cliente_nome[TAM_NOME];
    ItemPedido   itens[MAX_ITENS_PEDIDO];
    int          num_itens;
    double       subtotal;
    double       frete;
    double       desconto;
    double       total;
    Pagamento    pagamento;
    StatusPedido status;
    char         data_hora[30];
} Pedido;


typedef struct {
    int index;
    int qtd;
    double val;
} AuxVendas;

Cliente clientes[MAX_CLIENTES];
Produto produtos[MAX_PRODUTOS];
Pedido pedidos[MAX_PEDIDOS];

int num_clientes = 0;
int num_produtos = 0;
int num_pedidos  = 0;

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_tela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void) {
    printf("\n  Pressione ENTER para continuar...");
    getchar();
}

void obter_data_hora(char *buf, int tam) {
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", t);
}

void linha(char c, int n) {
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

void SalvarDados (void){
    FILE *f = fopen("dados_da_loja.dat", "wb");
    if (f) {
        fwrite(&num_clientes, sizeof(int), 1, f);
        fwrite(clientes, sizeof(clientes), num_clientes, f);
        fwrite(&num_produtos, sizeof(int), 1, f);
        fwrite(produtos, sizeof(produtos), num_produtos, f);
        fwrite(&num_pedidos, sizeof(int), 1, f);
        fwrite(pedidos, sizeof(pedidos), num_pedidos, f);
    }
}

void CarregarDados (void){
    FILE *f = fopen("dados_da_loja.dat", "wb");
    if (f) {
        fwrite(&num_clientes, sizeof(int), 1, f);
        fwrite(clientes, sizeof(clientes), num_clientes, f);
        fwrite(&num_produtos, sizeof(int), 1, f);
        fwrite(produtos, sizeof(produtos), num_produtos, f);
        fwrite(&num_pedidos, sizeof(int), 1, f);
        fwrite(pedidos, sizeof(pedidos), num_pedidos, f);
    }
}

const char *forma_pagamento_str(FormaPagamento f) {
    switch (f) {
        case CARTAO_CREDITO: return "Cartao de Credito";
        case CARTAO_DEBITO:  return "Cartao de Debito";
        case PIX:            return "PIX";
        case BOLETO:         return "Boleto";
        default:             return "Desconhecido";
    }
}

const char *status_str(StatusPedido s) {
    switch (s) {
        case PENDENTE:   return "Pendente";
        case APROVADO:   return "Aprovado";
        case CANCELADO:  return "Cancelado";
        case ENVIADO:    return "Enviado";
        case ENTREGUE:   return "Entregue";
        default:         return "Desconhecido";
    }
}

int buscar_cliente(int id) {
    for (int i = 0; i < num_clientes; i++)
        if (clientes[i].id == id) return i;
    return -1;
}

int buscar_produto(int id) {
    for (int i = 0; i < num_produtos; i++)
        if (produtos[i].id == id) return i;
    return -1;
}

int buscar_pedido(int id) {
    for (int i = 0; i < num_pedidos; i++)
        if (pedidos[i].id == id) return i;
    return -1;
}

int processar_pagamento(Pagamento *pag) {
    if (pag->forma == PIX || pag->forma == CARTAO_DEBITO) {
        pag->aprovado = 1;
    } else {
        pag->aprovado = (rand() % 10 != 0) ? 1 : 0;
    }
    return pag->aprovado;
}

void cadastrar_cliente(void) {
    if (num_clientes >= MAX_CLIENTES) {
        printf("  [!] Limite de clientes atingido.\n");
        return;
    }
    Cliente *c = &clientes[num_clientes];
    c->id = num_clientes + 1;

    printf("\n  === CADASTRO DE CLIENTE ===\n");
    printf("  Nome completo : "); scanf(" %99[^\n]", c->nome);
    printf("  E-mail        : "); scanf(" %79[^\n]", c->email);
    printf("  CPF           : "); scanf(" %14[^\n]", c->cpf);
    printf("  Telefone      : "); scanf(" %49[^\n]", c->telefone);
    printf("  Endereco      : "); scanf(" %99[^\n]", c->endereco);
    limpar_buffer();

    num_clientes++;
    printf("\n  [✓] Cliente cadastrado com ID #%d.\n", c->id);
}

void listar_clientes(void) {
    printf("\n  === CLIENTES CADASTRADOS ===\n");
    if (num_clientes == 0) { printf("  Nenhum cliente cadastrado.\n"); return; }

    linha('-', 70);
    printf("  %-4s  %-25s  %-30s  %-14s\n", "ID", "Nome", "E-mail", "CPF");
    linha('-', 70);
    for (int i = 0; i < num_clientes; i++) {
        Cliente *c = &clientes[i];
        printf("  %-4d  %-25s  %-30s  %-14s\n", c->id, c->nome, c->email, c->cpf);
    }
    linha('-', 70);
}

void ver_cliente(void) {
    int id;
    printf("\n  ID do cliente: "); 
    if (scanf("%d", &id) != 1) { limpar_buffer(); return; }
    limpar_buffer();

    int idx = buscar_cliente(id);
    if (idx < 0) { printf("  [!] Cliente nao encontrado.\n"); return; }

    Cliente *c = &clientes[idx];
    printf("\n");
    linha('=', 50);
    printf("  DETALHES DO CLIENTE #%d\n", c->id);
    linha('=', 50);
    printf("  Nome     : %s\n", c->nome);
    printf("  E-mail   : %s\n", c->email);
    printf("  CPF      : %s\n", c->cpf);
    printf("  Telefone : %s\n", c->telefone);
    printf("  Endereco : %s\n", c->endereco);
    linha('-', 50);

    printf("\n  Pedidos realizados:\n");
    int achou = 0;
    for (int i = 0; i < num_pedidos; i++) {
        if (pedidos[i].cliente_id == id) {
            printf("    Pedido #%d  |  %s  |  R$ %.2f  |  %s\n",
                   pedidos[i].id, pedidos[i].data_hora, pedidos[i].total, status_str(pedidos[i].status));
            achou = 1;
        }
    }
    if (!achou) printf("    Nenhum pedido.\n");
}

void cadastrar_produto(void) {
    if (num_produtos >= MAX_PRODUTOS) {
        printf("  [!] Limite de produtos atingido.\n");
        return;
    }
    Produto *p = &produtos[num_produtos];
    p->id = num_produtos + 1;

    printf("\n  === CADASTRO DE PRODUTO ===\n");
    printf("  Nome      : "); scanf(" %99[^\n]", p->nome);
    printf("  Categoria : "); scanf(" %49[^\n]", p->categoria);
    printf("  Preco (R$): "); 
    while (scanf("%lf", &p->preco) != 1) { printf("  Valor invalido. Preco: "); limpar_buffer(); }
    printf("  Estoque   : "); 
    while (scanf("%d", &p->estoque) != 1) { printf("  Valor invalido. Estoque: "); limpar_buffer(); }
    limpar_buffer();

    num_produtos++;
    printf("\n  [✓] Produto cadastrado com ID #%d.\n", p->id);
}

void listar_produtos(void) {
    printf("\n  === PRODUTOS DISPONIVEIS ===\n");
    if (num_produtos == 0) { printf("  Nenhum produto cadastrado.\n"); return; }

    linha('-', 75);
    printf("  %-4s  %-30s  %-15s  %10s  %8s\n", "ID", "Nome", "Categoria", "Preco", "Estoque");
    linha('-', 75);
    for (int i = 0; i < num_produtos; i++) {
        Produto *p = &produtos[i];
        printf("  %-4d  %-30s  %-15s  R$%8.2f  %8d\n", p->id, p->nome, p->categoria, p->preco, p->estoque);
    }
    linha('-', 75);
}

void editar_estoque(void) {
    int id, delta;
    printf("\n  ID do produto : "); 
    if (scanf("%d", &id) != 1) { limpar_buffer(); return; }
    int idx = buscar_produto(id);
    if (idx < 0) { printf("  [!] Produto nao encontrado.\n"); limpar_buffer(); return; }

    printf("  Ajuste (+/-) : "); 
    if (scanf("%d", &delta) != 1) { limpar_buffer(); return; }
    limpar_buffer();

    int novo = produtos[idx].estoque + delta;
    if (novo < 0) { printf("  [!] Estoque insuficiente.\n"); return; }
    produtos[idx].estoque = novo;
    printf("  [✓] Estoque atualizado: %d unidades.\n", produtos[idx].estoque);
}

void coletar_pagamento(Pagamento *pag, double total) {
    printf("\n  === FORMA DE PAGAMENTO ===\n");
    printf("  1. Cartao de Credito\n");
    printf("  2. Cartao de Debito\n");
    printf("  3. PIX\n");
    printf("  4. Boleto\n");
    printf("  Escolha: ");
    int op; 
    while (scanf("%d", &op) != 1 || op < 1 || op > 4) { printf("  Opcao invalida. Escolha: "); limpar_buffer(); }
    limpar_buffer();
    
    pag->forma = (FormaPagamento)op;
    pag->valor_total = total;

    switch (pag->forma) {
        case CARTAO_CREDITO:
        case CARTAO_DEBITO:
            printf("  Numero do cartao  : "); scanf(" %19[^\n]", pag->cartao.numero);
            printf("  Nome do titular   : "); scanf(" %99[^\n]", pag->cartao.titular);
            printf("  Validade (MM/AA)  : "); scanf(" %9s", pag->cartao.validade);
            printf("  CVV               : "); scanf(" %4s", pag->cartao.cvv);
            limpar_buffer();
            if (pag->forma == CARTAO_CREDITO) {
                printf("  Parcelas (1-12)   : "); 
                while (scanf("%d", &pag->cartao.parcelas) != 1) { limpar_buffer(); }
                limpar_buffer();
                if (pag->cartao.parcelas < 1)  pag->cartao.parcelas = 1;
                if (pag->cartao.parcelas > 12) pag->cartao.parcelas = 12;
            } else {
                pag->cartao.parcelas = 1;
            }
            break;

        case PIX:
            printf("  Chave PIX (CPF/e-mail/telefone): ");
            scanf(" %79[^\n]", pag->pix.chave);
            limpar_buffer();
            break;

        case BOLETO:
            printf("  [i] Boleto gerado. Vencimento em 3 dias uteis.\n");
            break;
    }
}

void realizar_pedido(void) {
    if (num_pedidos >= MAX_PEDIDOS) {
        printf("  [!] Limite de pedidos atingido.\n");
        return;
    }

    listar_clientes();
    printf("\n  ID do cliente: ");
    int cid; 
    if (scanf("%d", &cid) != 1) { limpar_buffer(); return; }
    int cidx = buscar_cliente(cid);
    if (cidx < 0) { printf("  [!] Cliente nao encontrado.\n"); limpar_buffer(); return; }

    Pedido *p = &pedidos[num_pedidos];
    memset(p, 0, sizeof(Pedido));
    p->id = num_pedidos + 1;
    p->cliente_id = cid;
    strcpy(p->cliente_nome, clientes[cidx].nome);
    obter_data_hora(p->data_hora, sizeof(p->data_hora));
    p->status = PENDENTE;

    printf("\n  === ADICIONAR ITENS AO CARRINHO ===\n");
    int continuar = 1;
    while (continuar && p->num_itens < MAX_ITENS_PEDIDO) {
        listar_produtos();
        printf("  ID do produto (0 = finalizar): ");
        int pid; 
        if (scanf("%d", &pid) != 1) { limpar_buffer(); continue; }
        if (pid == 0) { limpar_buffer(); break; }

        int pidx = buscar_produto(pid);
        if (pidx < 0) { printf("  [!] Produto nao encontrado.\n"); limpar_buffer(); continue; }
        if (produtos[pidx].estoque == 0) { printf("  [!] Sem estoque.\n"); limpar_buffer(); continue; }

        printf("  Quantidade: ");
        int qty; 
        if (scanf("%d", &qty) != 1) { limpar_buffer(); continue; }
        limpar_buffer();
        
        if (qty <= 0) { printf("  [!] Quantidade invalida.\n"); continue; }
        if (qty > produtos[pidx].estoque) {
            printf("  [!] Estoque insuficiente (%d disponiveis).\n", produtos[pidx].estoque);
            continue;
        }

        int ja_no_carrinho = 0;
        for (int i = 0; i < p->num_itens; i++) {
            if (p->itens[i].produto_id == pid) {
                p->itens[i].quantidade += qty;
                p->itens[i].subtotal = p->itens[i].quantidade * p->itens[i].preco_unitario;
                ja_no_carrinho = 1;
                printf("  [✓] Quantidade atualizada: %d unidades.\n", p->itens[i].quantidade);
                break;
            }
        }
        if (!ja_no_carrinho) {
            ItemPedido *item = &p->itens[p->num_itens];
            item->produto_id = pid;
            strcpy(item->nome_produto, produtos[pidx].nome);
            item->quantidade     = qty;
            item->preco_unitario = produtos[pidx].preco;
            item->subtotal       = qty * produtos[pidx].preco;
            p->num_itens++;
            printf("  [✓] Item adicionado.\n");
        }

        printf("  Adicionar mais itens? (1=Sim / 0=Nao): ");
        if (scanf("%d", &continuar) != 1) { continuar = 0; }
        limpar_buffer();
    }

    if (p->num_itens == 0) {
        printf("  [!] Pedido cancelado (nenhum item).\n");
        return;
    }

    p->subtotal = 0.0;
    for (int i = 0; i < p->num_itens; i++)
        p->subtotal += p->itens[i].subtotal;

    p->frete = (p->subtotal >= 150.0) ? 0.0 : 15.90;
    p->desconto = 0.0;

    printf("\n  Codigo de cupom (ENTER = nenhum): ");
    char cupom[20] = {0}; 
    scanf(" %19[^\n]", cupom);
    limpar_buffer();
    
    if (strcmp(cupom, "DESCONTO10") == 0) {
        p->desconto = p->subtotal * 0.10;
        printf("  [✓] Cupom DESCONTO10 aplicado: -R$ %.2f\n", p->desconto);
    }

    p->total = p->subtotal + p->frete - p->desconto;

    printf("\n");
    linha('=', 55);
    printf("  RESUMO DO PEDIDO\n");
    linha('=', 55);
    printf("  %-30s  %5s  %10s\n", "Produto", "Qtd", "Subtotal");
    linha('-', 55);
    for (int i = 0; i < p->num_itens; i++) {
        printf("  %-30s  %5d  R$%8.2f\n", p->itens[i].nome_produto, p->itens[i].quantidade, p->itens[i].subtotal);
    }
    linha('-', 55);
    printf("  %-38s  R$%8.2f\n", "Subtotal", p->subtotal);
    printf("  %-38s  R$%8.2f\n", "Frete", p->frete);
    if (p->desconto > 0)
        printf("  %-38s -R$%8.2f\n", "Desconto", p->desconto);
    linha('-', 55);
    printf("  %-38s  R$%8.2f\n", "TOTAL", p->total);
    linha('=', 55);

    coletar_pagamento(&p->pagamento, p->total);

    if (p->pagamento.forma == PIX) {
        double desc_pix = p->total * 0.05;
        printf("  [✓] Desconto PIX 5%%: -R$ %.2f\n", desc_pix);
        p->desconto += desc_pix;
        p->total    -= desc_pix;
        p->pagamento.valor_total = p->total;
    }

    printf("\n  Processando pagamento...\n");
    if (processar_pagamento(&p->pagamento)) {
        p->status = APROVADO;
        for (int i = 0; i < p->num_itens; i++) {
            int pidx = buscar_produto(p->itens[i].produto_id);
            if (pidx >= 0) produtos[pidx].estoque -= p->itens[i].quantidade;
        }
        printf("  [✓] PAGAMENTO APROVADO!\n");
        printf("  Pedido #%d registrado com sucesso.\n", p->id);
        if (p->pagamento.forma == CARTAO_CREDITO && p->pagamento.cartao.parcelas > 1) {
            printf("  Parcelado em %dx de R$ %.2f\n", p->pagamento.cartao.parcelas, p->total / p->pagamento.cartao.parcelas);
        }
    } else {
        p->status = CANCELADO;
        printf("  [✗] PAGAMENTO RECUSADO. Pedido cancelado.\n");
    }

    num_pedidos++;
}

void listar_pedidos(void) {
    printf("\n  === PEDIDOS REGISTRADOS ===\n");
    if (num_pedidos == 0) { printf("  Nenhum pedido registrado.\n"); return; }

    linha('-', 80);
    printf("  %-4s  %-20s  %-20s  %10s  %-10s  %-10s\n", "ID", "Cliente", "Data/Hora", "Total", "Pagamento", "Status");
    linha('-', 80);
    for (int i = 0; i < num_pedidos; i++) {
        Pedido *p = &pedidos[i];
        printf("  %-4d  %-20s  %-20s  R$%7.2f  %-10s  %-10s\n",
               p->id, p->cliente_nome, p->data_hora, p->total, forma_pagamento_str(p->pagamento.forma), status_str(p->status));
    }
    linha('-', 80);
}

void ver_pedido(void) {
    int id;
    printf("\n  ID do pedido: "); 
    if (scanf("%d", &id) != 1) { limpar_buffer(); return; }
    limpar_buffer();
    
    int idx = buscar_pedido(id);
    if (idx < 0) { printf("  [!] Pedido nao encontrado.\n"); return; }

    Pedido *p = &pedidos[idx];
    printf("\n");
    linha('=', 60);
    printf("  PEDIDO #%d  —  %s\n", p->id, p->data_hora);
    linha('=', 60);
    printf("  Cliente : %s (ID %d)\n", p->cliente_nome, p->cliente_id);
    printf("  Status  : %s\n", status_str(p->status));
    linha('-', 60);
    printf("  %-28s  %5s  %10s  %12s\n", "Produto", "Qtd", "Unit.", "Subtotal");
    linha('-', 60);
    for (int i = 0; i < p->num_itens; i++) {
        printf("  %-28s  %5d  R$%8.2f  R$%10.2f\n",
               p->itens[i].nome_produto, p->itens[i].quantidade, p->itens[i].preco_unitario, p->itens[i].subtotal);
    }
    linha('-', 60);
    printf("  %-44s  R$%10.2f\n", "Subtotal", p->subtotal);
    printf("  %-44s  R$%10.2f\n", "Frete",    p->frete);
    if (p->desconto > 0)
        printf("  %-44s -R$%10.2f\n", "Descontos", p->desconto);
    linha('-', 60);
    printf("  %-44s  R$%10.2f\n", "TOTAL", p->total);
    linha('=', 60);
    printf("  Pagamento : %s\n", forma_pagamento_str(p->pagamento.forma));
    if (p->pagamento.forma == CARTAO_CREDITO || p->pagamento.forma == CARTAO_DEBITO) {
        int len = (int)strlen(p->pagamento.cartao.numero);
        printf("  Cartao    : **** **** **** %s\n", len >= 4 ? p->pagamento.cartao.numero + len - 4 : p->pagamento.cartao.numero);
        if (p->pagamento.forma == CARTAO_CREDITO)
            printf("  Parcelas  : %dx de R$ %.2f\n", p->pagamento.cartao.parcelas, p->total / p->pagamento.cartao.parcelas);
    }
    printf("  Situacao  : %s\n", p->pagamento.aprovado ? "Aprovado" : "Recusado");
    linha('=', 60);
}

void atualizar_status_pedido(void) {
    int id;
    printf("\n  ID do pedido: "); 
    if (scanf("%d", &id) != 1) { limpar_buffer(); return; }
    int idx = buscar_pedido(id);
    if (idx < 0) { printf("  [!] Pedido nao encontrado.\n"); limpar_buffer(); return; }

    printf("  Status atual: %s\n", status_str(pedidos[idx].status));
    printf("  Novo status:\n");
    printf("    1. Pendente\n");
    printf("    2. Aprovado\n");
    printf("    3. Cancelado\n");
    printf("    4. Enviado\n");
    printf("    5. Entregue\n");
    printf("  Escolha: ");
    int op; 
    if (scanf("%d", &op) != 1 || op < 1 || op > 5) { printf("  [!] Opcao invalida.\n"); limpar_buffer(); return; }
    limpar_buffer();
    
    pedidos[idx].status = (StatusPedido)op;
    printf("  [✓] Status atualizado para: %s\n", status_str(pedidos[idx].status));
}

void relatorio_vendas(void) {
    printf("\n");
    linha('=', 55);
    printf("  RELATORIO DE VENDAS\n");
    linha('=', 55);

    double total_geral = 0.0;
    int aprovados = 0, cancelados = 0, enviados = 0, entregues = 0;
    double total_cc = 0, total_cd = 0, total_pix = 0, total_bol = 0;

    for (int i = 0; i < num_pedidos; i++) {
        Pedido *p = &pedidos[i];
        if (p->status == CANCELADO) { cancelados++; continue; }
        total_geral += p->total;
        if (p->status == APROVADO)  aprovados++;
        if (p->status == ENVIADO)   enviados++;
        if (p->status == ENTREGUE)  entregues++;

        switch (p->pagamento.forma) {
            case CARTAO_CREDITO: total_cc  += p->total; break;
            case CARTAO_DEBITO:  total_cd  += p->total; break;
            case PIX:            total_pix += p->total; break;
            case BOLETO:         total_bol += p->total; break;
        }
    }

    printf("  Pedidos totais     : %d\n", num_pedidos);
    printf("  Aprovados          : %d\n", aprovados);
    printf("  Enviados           : %d\n", enviados);
    printf("  Entregues          : %d\n", entregues);
    printf("  Cancelados         : %d\n", cancelados);
    linha('-', 55);
    printf("  Receita total      : R$ %.2f\n", total_geral);
    linha('-', 55);
    printf("  Por forma de pagamento:\n");
    printf("    Cartao Credito   : R$ %.2f\n", total_cc);
    printf("    Cartao Debito    : R$ %.2f\n", total_cd);
    printf("    PIX              : R$ %.2f\n", total_pix);
    printf("    Boleto           : R$ %.2f\n", total_bol);
    linha('=', 55);

    printf("\n  TOP 5 PRODUTOS MAIS VENDIDOS:\n");
    linha('-', 45);

    int vend_qty[MAX_PRODUTOS] = {0};
    double vend_val[MAX_PRODUTOS] = {0.0};

    for (int i = 0; i < num_pedidos; i++) {
        if (pedidos[i].status == CANCELADO) continue;
        for (int j = 0; j < pedidos[i].num_itens; j++) {
            int pidx = buscar_produto(pedidos[i].itens[j].produto_id);
            if (pidx >= 0) {
                vend_qty[pidx] += pedidos[i].itens[j].quantidade;
                vend_val[pidx] += pedidos[i].itens[j].subtotal;
            }
        }
    }

    AuxVendas aux[MAX_PRODUTOS];
    for (int i = 0; i < num_produtos; i++) {
        aux[i].index = i;
        aux[i].qtd = vend_qty[i];
        aux[i].val = vend_val[i];
    }

    for (int i = 0; i < num_produtos - 1; i++) {
        int mx = i;
        for (int j = i + 1; j < num_produtos; j++) {
            if (aux[j].qtd > aux[mx].qtd) mx = j;
        }
        AuxVendas temp = aux[i];
        aux[i] = aux[mx];
        aux[mx] = temp;
    }

    for (int top = 0; top < 5 && top < num_produtos; top++) {
        if (aux[top].qtd > 0) {
            printf("  %d. %-25s  %4d un.  R$ %.2f\n",
                   top + 1, produtos[aux[top].index].nome, aux[top].qtd, aux[top].val);
        }
    }
    linha('=', 55);
}

void carregar_dados_exemplo(void) {
    strcpy(clientes[0].nome,      "Ana Souza");
    strcpy(clientes[0].email,     "ana.souza@email.com");
    strcpy(clientes[0].cpf,       "111.222.333-44");
    strcpy(clientes[0].telefone,  "(11) 91234-5678");
    strcpy(clientes[0].endereco,  "Rua das Flores, 10 - SP");
    clientes[0].id = 1;

    strcpy(clientes[1].nome,      "Bruno Lima");
    strcpy(clientes[1].email,     "bruno.lima@email.com");
    strcpy(clientes[1].cpf,       "555.666.777-88");
    strcpy(clientes[1].telefone,  "(21) 99876-5432");
    strcpy(clientes[1].endereco,  "Av. Brasil, 200 - RJ");
    clientes[1].id = 2;

    num_clientes = 2;

    produtos[0] = (Produto){1, "Notebook Pro 15",    "Eletronicos",  3499.90, 10};
    produtos[1] = (Produto){2, "Fone Bluetooth X2",  "Eletronicos",   199.90, 25};
    produtos[2] = (Produto){3, "Mochila Urbana",     "Acessorios",    149.90, 15};
    produtos[3] = (Produto){4, "Teclado Mecanico",   "Perifericos",   359.00, 12};
    produtos[4] = (Produto){5, "Mouse Gamer RGB",    "Perifericos",   129.90, 20};
    produtos[5] = (Produto){6, "Cadeira Ergonomica", "Moveis",        899.00,  5};
    num_produtos = 6;

    printf("  [✓] Dados de exemplo carregados (%d clientes, %d produtos).\n", num_clientes, num_produtos);
}

void menu_clientes(void) {
    int op;
    do {
        printf("\n  ╔══════════════════════════╗\n");
        printf("  ║      CLIENTES            ║\n");
        printf("  ╠══════════════════════════╣\n");
        printf("  ║  1. Cadastrar cliente    ║\n");
        printf("  ║  2. Listar clientes      ║\n");
        printf("  ║  3. Ver detalhes         ║\n");
        printf("  ║  0. Voltar               ║\n");
        printf("  ╚══════════════════════════╝\n");
        printf("  Opcao: "); 
        if (scanf("%d", &op) != 1) { op = -1; }
        limpar_buffer();

        switch (op) {
            case 1: cadastrar_cliente(); break;
            case 2: listar_clientes();   break;
            case 3: ver_cliente();       break;
            case 0: break;
            default: printf("  [!] Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menu_produtos(void) {
    int op;
    do {
        printf("\n  ╔══════════════════════════╗\n");
        printf("  ║      PRODUTOS            ║\n");
        printf("  ╠══════════════════════════╣\n");
        printf("  ║  1. Cadastrar produto    ║\n");
        printf("  ║  2. Listar produtos      ║\n");
        printf("  ║  3. Ajustar estoque      ║\n");
        printf("  ║  0. Voltar               ║\n");
        printf("  ╚══════════════════════════╝\n");
        printf("  Opcao: "); 
        if (scanf("%d", &op) != 1) { op = -1; }
        limpar_buffer();

        switch (op) {
            case 1: cadastrar_produto(); break;
            case 2: listar_produtos();   break;
            case 3: editar_estoque();    break;
            case 0: break;
            default: printf("  [!] Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menu_pedidos(void) {
    int op;
    do {
        printf("\n  ╔══════════════════════════╗\n");
        printf("  ║      PEDIDOS             ║\n");
        printf("  ╠══════════════════════════╣\n");
        printf("  ║  1. Novo pedido          ║\n");
        printf("  ║  2. Listar pedidos       ║\n");
        printf("  ║  3. Ver pedido           ║\n");
        printf("  ║  4. Atualizar status     ║\n");
        printf("  ║  0. Voltar               ║\n");
        printf("  ╚══════════════════════════╝\n");
        printf("  Opcao: "); 
        if (scanf("%d", &op) != 1) { op = -1; }
        limpar_buffer();

        switch (op) {
            case 1: realizar_pedido();         break;
            case 2: listar_pedidos();          break;
            case 3: ver_pedido();              break;
            case 4: atualizar_status_pedido(); break;
            case 0: break;
            default: printf("  [!] Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menu_principal(void) {
    int op;
    do {
        limpar_tela();
        printf("\n");
        linha('=', 50);
        printf("     🛒  SIMULADOR DE LOJA VIRTUAL\n");
        linha('=', 50);
        printf("  1. Clientes\n");
        printf("  2. Produtos\n");
        printf("  3. Pedidos / Transacoes\n");
        printf("  4. Relatorio de vendas\n");
        printf("  5. Carregar dados de exemplo\n");
        printf("  0. Sair\n");
        linha('-', 50);
        printf("  Opcao: "); 
        if (scanf("%d", &op) != 1) { op = -1; }
        limpar_buffer();

        switch (op) {
            case 1: menu_clientes();    break;
            case 2: menu_produtos();    break;
            case 3: menu_pedidos();     break;
            case 4: relatorio_vendas(); pausar(); break;
            case 5: carregar_dados_exemplo(); pausar(); break;
            case 0: printf("\n  Ate logo!\n\n"); break;
            default: printf("  [!] Opcao invalida.\n"); pausar();
        }
    } while (op != 0);
}

int main(void) {
    srand((unsigned)time(NULL));
    menu_principal();
    return 0;
}
