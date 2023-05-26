// Precificacao.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ql/quantlib.hpp>

// using namespace std;
using namespace QuantLib;


class Titulo {
public:
    std::string nome;
    int id;
    Real valor_nominal;
    int prazo;
    Real taxa_juros;
    Real priceCall;
};


std::vector<Titulo> lerArquivo(std::vector<Titulo>& titulos) {
    std::ifstream arquivo("dados_titulos.csv");
    //    std::vector<Titulo> titulos;


    if (!arquivo) {
        std::cerr << "Falha ao abrir o arquivo!" << std::endl;
    }

    std::string linha;
    int cont = 0;

    while (std::getline(arquivo, linha)) {
        std::istringstream dados(linha);
        Titulo titulo;

        std::string id, nome, valor_nominal, prazo, taxa_juros;

        if (cont != 0) {
            if (getline(dados, id, ','), getline(dados, nome, ',') && getline(dados, valor_nominal, ',') && getline(dados, prazo, ','), getline(dados, taxa_juros)) {
                titulo.id = stoi(id);
                titulo.nome = nome;
                titulo.id = stoi(id);
                titulo.valor_nominal = stof(valor_nominal);
                titulo.prazo = stoi(prazo);
                titulo.taxa_juros = stof(taxa_juros);
                titulos.push_back(titulo);
            }
        }
        cont++;
    }
    return titulos;
}

void imprimirTitulos(std::vector<Titulo> titulos) {
    for (int i = 0; i < titulos.size(); i++) {
        std::cout << titulos[i].id << " | " << titulos[i].nome << " | " << titulos[i].valor_nominal << " | " << titulos[i].prazo << " | " << titulos[i].taxa_juros << std::endl;
    }
}

void blackSholesPrecificacao(std::vector<Titulo> titulos) {


    Option::Type tipoOpcao = Option::Call;
    Volatility volatilidadeMercado = 0.0;
    Real taxaDividendos = 0.02;

    try {
        for (Titulo t : titulos) {
            Date dataHoje = Date::todaysDate(); // DATA DE HOJE
            Date dataVencimentoAtivo = dataHoje + Period(t.prazo, Months);
            Real taxa_juros_sem_risco = t.taxa_juros;

            // CONFIGURAÇÃO DO CALENDÁRIO
            Settings::instance().evaluationDate() = dataHoje = dataHoje;
            Calendar calendario = TARGET();

            // CURVA DA TAXA DE JUROS SEM RISCO
            DayCounter contagemDias = Actual365Fixed();
            Handle<YieldTermStructure> curvaTaxaJuros(std::make_shared<FlatForward>(0, calendario, taxa_juros_sem_risco, contagemDias));

            // CURVA DIVIDENDOS
            Handle<YieldTermStructure> curvaTaxaDividendos(std::make_shared<FlatForward>(0, calendario, taxaDividendos, contagemDias));

            // CONFIGURAÇÃO DO MÉTODO DE PRECIFICAÇÃO BLACK-SCHOLES
            Handle<BlackVolTermStructure> volatilidadeCurva(
                std::make_shared<BlackConstantVol>(0, calendario, volatilidadeMercado, contagemDias));
            std::shared_ptr<BlackScholesMertonProcess> metodo(
                std::make_shared<BlackScholesMertonProcess>(
                    Handle<Quote>(std::make_shared<SimpleQuote>(0.0)),
                    curvaTaxaDividendos, curvaTaxaJuros, volatilidadeCurva));

            std::shared_ptr<PlainVanillaPayoff> recompensa(std::make_shared<PlainVanillaPayoff>(tipoOpcao, t.valor_nominal));
            std::shared_ptr<Exercise> compraEuropeia(std::make_shared<EuropeanExercise>(dataVencimentoAtivo));
            VanillaOption opcao(recompensa, compraEuropeia);
            opcao.setPricingEngine(std::make_shared<AnalyticEuropeanEngine>(metodo));

            Real precificacao = opcao.NPV();
            std::cout << "A precificação do ativo: " << t.nome << " e: " << precificacao << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

int main()
{
    std::vector<Titulo> titulos;
    // LÊ O ARQUIVO E SALVA OS DADOS EM UM VETOR DE TÍTULOS
    lerArquivo(titulos);

    // IMPRIME OS TÍTULOS
    imprimirTitulos(titulos);

}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
