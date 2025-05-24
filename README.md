# 42sh — A POSIX-Compliant Shell

## 📝 Description

42sh est un shell UNIX minimal implémenté en C, visant à respecter la spécification POSIX Shell Command Language. Il reproduit le comportement des fonctionnalités communes des shells, notamment l'exécution de commandes, les redirections, les structures de contrôle (if, while, for) et les commandes intégrées de base comme cd, exit et echo.

Ce projet a été développé dans le cadre du module de Programmation Système UNIX de l'EPITA.

## ⚙️ Fonctionnalités

- Moteur d'exécution basé sur un Lexer / Parser / AST (Abstract Syntax Tree)
- Exécution de commandes avec contrôle des descripteurs de fichiers
- Commandes intégrées: cd, exit, echo, true, false, export, unset, . (dot)
- Structures conditionnelles: if, elif, else, fi
- Boucles: while, until, for
- Redirections: >, <, >>, etc.
- Pipelines: cmd1 | cmd2
- Opérateurs logiques: &&, ||
- Gestion des variables et substitution
- Règles de citation: guillemets simples et doubles
- Commentaires
- Sous-shells et substitution de commandes
- Gestion des alias
- Optionnel: Affichage de l'AST pour le débogage

## 🔧 Installation & Compilation

### Prérequis
- Compilateur GCC
- Autotools (autoconf, automake, etc.)

### Étapes
```
autoreconf --install
./configure
make
```

Cette procédure générera le binaire `src/42sh`.

## ▶️ Utilisation

```
./42sh [OPTIONS] [SCRIPT] [ARGUMENTS...]
```

### Modes d'entrée
- **Entrée en chaîne de caractères**: `./42sh -c "echo hello world"`
- **Fichier script**: `./42sh script.sh`
- **Entrée standard**: `./42sh < script.sh` ou `cat script.sh | ./42sh` 

## 🧪 Tests

Pour exécuter la suite de tests:
```
make check
```

Assurez-vous que toutes les fonctionnalités sont soigneusement testées, y compris l'analyse des commandes, l'exécution et la gestion des erreurs.

## 🛑 Fonctions interdites

Il est interdit d'utiliser:
- glob(3)
- regexec(3)
- wordexp(3)
- popen(3)
- syscall(2)
- system(3)

## 🧠 Conseils pour le développement

- Utilisez `bash --posix` pour comparer les comportements.
- Implémentez un afficheur (--pretty-print ou PRETTY_PRINT=1) pour déboguer l'AST.
- Utilisez fmemopen(3) pour gérer les entrées de chaînes.
- Nettoyez toute la mémoire et les descripteurs de fichiers — les fuites seront pénalisées.

## 📁 Structure du projet

```
.
├── src/
│   └── *.c, *.h   # Fichiers d'implémentation
├── tests/
│   └── ...        # Tests fonctionnels et unitaires
├── Makefile.am
├── configure.ac
```

## 📚 Références

- Spécification POSIX Shell: https://pubs.opengroup.org/onlinepubs/9699919799/
- Crafting Interpreters (conception du parser): https://craftinginterpreters.com/
- Guide Autotools: https://www.lrde.epita.fr/~adl/dl/autotools-handout-4.pdf
