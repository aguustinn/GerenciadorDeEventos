# Sistema de Eventos Críticos – Árvore AVL

## 📌 Descrição do Sistema

Este sistema foi desenvolvido em linguagem C para gerenciar eventos críticos de uma cidade, como acidentes de trânsito, falhas em semáforos, quedas de energia, alagamentos e incêndios.

A estrutura de dados utilizada é uma **Árvore AVL**, que mantém os dados sempre balanceados, garantindo eficiência nas operações.

Cada evento possui:

* ID (único)
* Tipo
* Severidade (1 a 5)
* Data e hora
* Região
* Status (Ativo ou Resolvido)

### Funcionalidades principais:

* Inserir eventos
* Buscar evento por ID
* Atualizar status e severidade
* Remover eventos resolvidos
* Listar eventos por:

  * Severidade
  * Região
  * Intervalo de IDs
* Exibir métricas da árvore

---

## 🛠️ Como Compilar e Executar

### Compilar:

```bash id="u7z9b3"
gcc GerenciadorDeEventos.c -o GerenciadorDeEventos
```

### Executar:

**Windows:**

```bash id="s6y2xt"
GerenciadorDeEventos.exe
```

**Linux/macOS:**

```bash id="9j5fke"
./GerenciadorDeEventos
```

---

## ⚠️ Observação

O programa funciona por meio de um menu interativo no terminal e permanece em execução até o usuário escolher sair.
