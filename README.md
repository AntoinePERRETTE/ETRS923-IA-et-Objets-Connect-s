# ETRS923 – IA et Objets Connectés

Prédiction météo embarquée sur **NUCLEO-N657X0-Q** (STM32N6, NPU Neural-ART) : acquisition de capteurs, classification du temps par un petit CNN quantifié en int8, et publication des mesures et de la prédiction sur **ThingSpeak**.

Ce dépôt regroupe le travail des quatre TP du module *Systèmes embarqués et IA au plus proche des capteurs (edge AI)* (Université Savoie Mont Blanc, 2025‑2026).

**Équipe :** Antoine PERRETTE · Jeremy DIEVAL · Gregory PONCET

---

## Sommaire

1. [Structure du dépôt](#1-structure-du-dépôt)
2. [TP1 – Problème MNIST](#2-tp1--problème-mnist)
3. [TP2 – Interface capteurs, LED et réseau](#3-tp2--interface-capteurs-led-et-réseau)
4. [TP3 – Cloud versus Edge AI (problème Meteostat)](#4-tp3--cloud-versus-edge-ai-problème-meteostat)
5. [Application finale](#5-application-finale)
6. [Écarts entre les sujets et notre réalisation](#6-écarts-entre-les-sujets-et-notre-réalisation)
7. [Sobriété, limites et perspectives (TP4 / P_TEDS)](#7-sobriété-limites-et-perspectives-tp4--p_teds)
8. [Mode d'emploi du code, TP par TP](#8-mode-demploi-du-code-tp-par-tp)

Légende des statuts : ✅ réalisé · ⚠️ partiel · ❌ non réalisé · ❓ à confirmer par l'équipe.

---

## 1. Structure du dépôt

```
.
├── ETRS923.ipynb                  # Notebook Colab du TP1 (MNIST), à la racine
├── Application finale/
│   └── Application/               # Projet STM32CubeIDE final (IA embarquée + capteurs + ThingSpeak)
├── TP_1/                          # Fichiers Python du TP1
│   ├── callbackPlots.py           # Callback Keras (courbe d'accuracy en direct) + comptage neurones/synapses
│   ├── callbacks_torch.py         # Équivalent PyTorch du précédent
│   ├── MNIST_cnn_functions.py     # Briques CNN bas niveau TensorFlow (poids, biais, conv, pooling)
│   ├── npu_callbacks.py           # Couche NpuLinear + callback, test sur NPU Intel (bonus)
│   ├── npu_TB.py                  # Banc d'essai MatMul sur NPU Intel (bonus)
│   ├── model_Accu0.7150.keras     # CNN Keras sauvegardé (dropout 0,8)
│   ├── model_Accu0.9653.keras     # SimpleRNN Keras sauvegardé
│   ├── analyse_modele_PyTorch_Keras.pdf  # Synthèse des résultats Keras vs PyTorch
│   └── resultat/                  # Graphiques, CSV et logs générés par le notebook
├── TP_2/
│   ├── LED_Blink/                 # Valide la chaîne de compilation sur la NUCLEO-N657X0-Q
│   ├── Interface_Capteurs/        # Lecture des capteurs I²C, affichage console
│   └── Test_Ethernet/Application/ # ThreadX + NetXDuo : DHCP, DNS, envoi des mesures à ThingSpeak
├── meteostat.py                   # Script Colab Meteostat : données, entraînement, export ONNX int8
├── LICENSE
└── README.md
```

---

## 2. TP1 – Problème MNIST

### 2.1 Démarche

Le TP consiste à classer les images MNIST (28 × 28, 10 classes) avec un réseau dense, puis un réseau convolutionnel, en faisant varier les hyperparamètres. Tout est dans `ETRS923.ipynb` (exécutable sur Google Colab). Les expériences sont reproductibles : les graines sont fixées (20 puis 42 selon les cellules).

Le notebook suit le plan du sujet :

| Cellule | Contenu |
|---|---|
| 1.a | Réseau à deux couches (entrée → softmax), sans couche cachée |
| 1.b / 1.c / 1.d | Réseau dense à couches cachées avec ReLU / tanh / sigmoid |
| 1.e | Grille complète activation × optimiseur × architecture (2, 3 et 4 couches), avec un modèle de classification et un modèle de régression à un seul neurone de sortie |
| Bonus | MLP et CNN en PyTorch |

Les paramètres communs : 10 époques, batch de 32, entrées normalisées (÷255) et aplaties en 784. La validation est faite sur le jeu de test MNIST (voir les limites en 2.5).

### 2.2 Résultats

**1.a – Softmax seul** (adam, `categorical_crossentropy`, 5 époques) : **92,42 %** pour 7 850 synapses (784 × 10 + 10).

**1.b à 1.d – Activation et optimiseur** (architecture 300‑100, test accuracy) :

| Activation | Adam | SGD | RMSprop | Adagrad |
|---|---|---|---|---|
| ReLU | 97,62 % | 96,85 % | 97,61 % | 93,08 % |
| tanh | 97,50 % | 95,82 % | 97,96 % | 92,31 % |
| sigmoid | 97,71 % | 90,38 % | 97,84 % | 74,32 % |

**1.e – Architectures** (ReLU, source : `analyse_modele_PyTorch_Keras.pdf`, graine 42) :

| Architecture | Synapses (poids + biais) | Adam | RMSprop |
|---|---|---|---|
| 300‑100 | 266 610 | 97,73 % | 97,85 % |
| 392‑196‑98 | 405 044 | 97,98 % | 97,73 % |
| 512‑256‑128‑64 | 575 050 | 98,10 % | 97,85 % |

Les nombres de synapses sont calculés à partir des dimensions des couches (même formule que `count_neurons_and_synapses`). Les valeurs diffèrent légèrement entre les deux tableaux (par exemple 97,62 % contre 97,73 % pour ReLU/Adam/300‑100) car les graines ne sont pas les mêmes.

**Réseaux de comparaison (PyTorch, Adam)** :

| Modèle | Détail | Paramètres | Test accuracy |
|---|---|---|---|
| MLP | 784‑392‑196‑98‑10, ReLU, dropout 0,5, 15 époques | 405 044 | 98,23 % |
| CNN | 4 conv (6, 12, 24, 48 filtres) + 2 max‑pool + dense 108‑27‑10, ReLU, dropout 0,2, 12 époques | ≈ 64 987 | **98,92 %** |

### 2.3 Réponses aux questions du sujet

**Choix de la fonction d'activation (1.a à 1.e)**

- Sans couche cachée, le modèle est un classifieur linéaire : il atteint 92,4 % avec très peu de synapses, mais il ne peut pas aller plus loin car les chiffres ne sont pas linéairement séparables.
- Dès qu'on ajoute une couche cachée, on passe à environ 97,5–98 %. Au‑delà de 2 couches, le gain est faible (97,7 % → 98,1 % en passant de 266 k à 575 k synapses) alors que le coût mémoire double. Pour un système embarqué, l'architecture 300‑100 est donc déjà un bon compromis.
- ReLU et tanh convergent bien. La sigmoid s'effondre avec les optimiseurs simples (90,4 % avec SGD et 74,3 % avec Adagrad contre 96,9 % et 93,1 % pour ReLU) : sa dérivée est faible aux extrémités (gradient qui s'évanouit) et elle converge plus lentement. ReLU est aussi la moins coûteuse à calculer, ce qui compte sur microcontrôleur.
- Nombre d'époques : les courbes de validation plafonnent après 6 à 10 époques. Au‑delà, l'écart entre accuracy d'entraînement (≈ 99,3–99,5 %) et de validation (≈ 97,7–98,1 %) montre un début de surapprentissage.

**Choix de l'algorithme d'optimisation (2)**

Adam et RMSprop sont à égalité et nettement devant SGD, puis Adagrad. Avec les taux d'apprentissage par défaut de Keras et 10 époques seulement, SGD n'a pas fini de converger. Le taux d'apprentissage d'Adagrad décroît à mesure que les gradients s'accumulent, ce qui ralentit fortement l'apprentissage. Nous avons donc concentré la grille 1.e sur Adam et RMSprop.

**Choix de la fonction coût (3)**

- `categorical_crossentropy` (étiquettes one‑hot, sortie softmax) est celle utilisée pour toute la partie classification.
- Nous l'avons comparée à un modèle de régression (un neurone de sortie, MSE, étiquettes de 0 à 9). Il est nettement moins bon : la meilleure variante atteint une MAE de 0,13 chiffre (sigmoid, Adam, 4 couches), les autres se situent entre 0,13 et 0,27. La régression suppose à tort un ordre entre les classes (confondre un 3 avec un 4 serait « moins grave » qu'avec un 9), et la MSE n'est pas une mesure d'accuracy.
- `binary_crossentropy` ne convient qu'à deux classes (ou à du multi‑label) et n'a pas été testée sur MNIST. `sparse_categorical_crossentropy` est mathématiquement équivalente à la catégorielle mais prend des étiquettes entières, ce qui évite le one‑hot. C'est ce que fait `nn.CrossEntropyLoss` de PyTorch (sur logits) dans nos réseaux PyTorch.

### 2.4 Réseau convolutionnel et régularisation

- **Efficacité** : le CNN (≈ 65 k paramètres) fait mieux que le MLP (405 k) avec 6 fois moins de paramètres. Les convolutions partagent leurs poids et exploitent la structure spatiale que l'aplatissement en 784 valeurs détruit. C'est l'argument le plus fort pour l'embarqué : moins de mémoire et de calcul, pour une meilleure accuracy.
- **Dropout** : avec 0,2 dans le CNN et 0,5 dans le MLP, l'écart entre entraînement et validation reste faible. À l'inverse, un dropout trop agressif dégrade le modèle : le CNN sauvegardé dans `model_Accu0.7150.keras` (3 convolutions à 30, 50 et 80 filtres, dense 500, **dropout 0,8**) n'atteint que 71,5 %. Le dropout est donc à régler avec la taille du réseau : un réseau plus grand en supporte davantage.
- **Taille du réseau** : un réseau trop gros surapprend, un réseau trop petit sous‑apprend. Le bon point de fonctionnement se trouve en augmentant progressivement la capacité tant que l'accuracy de validation progresse.
- **Variante récurrente** : `model_Accu0.9653.keras` est un `SimpleRNN` de 128 unités suivi d'un softmax (96,53 %), moins précis que le CNN. Il illustre pourquoi le CNN est le bon choix pour des images.

### 2.5 Bonus et limites

- Les mêmes expériences ont été refaites avec PyTorch (MLP et CNN ci‑dessus), avec des résultats du même ordre que Keras.
- `npu_TB.py` et `npu_callbacks.py` explorent l'accélération des multiplications matricielles sur le **NPU Intel** d'un PC portable (bibliothèque `intel_npu_acceleration_library`). Dans l'état actuel du code, `NpuLinear` calcule le produit matriciel avec PyTorch dans les deux branches (NPU disponible ou non) : le NPU n'est donc pas réellement sollicité dans l'entraînement. Ce bonus n'a aucun lien avec le NPU Neural‑ART de la carte STM32N6.
- La validation utilise le jeu de test. Les chiffres de test sont donc légèrement optimistes, puisque les hyperparamètres ont été choisis en les regardant. Un vrai jeu de validation séparé serait préférable.
- `count_neurons_and_synapses` ne compte que les couches denses ; les paramètres du CNN ont été calculés à la main.

---

## 3. TP2 – Interface capteurs, LED et réseau

Matériel : **NUCLEO-N657X0-Q** (STM32N657, avec NPU Neural‑ART) + carte d'extension capteurs, câble Ethernet, liaison série via l'USB du ST‑LINK (USART1, 115 200 bauds, 8N1).

### 3.1 Statut par question

| Question du sujet | Statut | Où |
|---|---|---|
| 1.a Allumer les trois LED | ✅ | `LED_Blink` |
| 1.b Chenillard temporisé | ✅ | `LED_Blink` (délai réglable par la variable `delay`, 500 ms dans la version déposée, 3 000 ms pour la consigne du sujet) |
| 1.c Logs console `<ROUGE>`, `<BLEU>`, `<VERT>` | ✅ | `LED_Blink` (les messages `<début d'application>` et `<début de chenillard>` ne sont pas émis) |
| 2.a Lecture des capteurs, logs console | ✅ | `Interface_Capteurs`, `Test_Ethernet` (`sensor_task.c`) |
| 2.b LED d'état (rouge = lecture, vert = attente) | ❓ | `Interface_Capteurs` |
| 3.a OS temps réel | ✅ | **ThreadX** (voir 3.3) |
| 3.b Pile IP avec DHCP et ICMP, sockets | ✅ | **NetXDuo** (DHCP, ICMP, TCP, DNS) |
| 3.c Ping vers google.com | ⚠️ | ICMP est activé, mais aucun ping sortant n'est codé. La connectivité Internet est prouvée par la résolution DNS de `api.thingspeak.com` et les requêtes HTTP vers ThingSpeak |
| 3.d LED bleue = communication réseau | ❓ | Non présente dans `Test_Ethernet` ; à vérifier dans l'application finale |
| 3.e Logs d'état des cycles | ⚠️ | Logs DHCP, DNS, état du lien Ethernet et statut d'envoi ThingSpeak ; les libellés exacts du sujet ne sont pas repris |

### 3.2 LED_Blink et Interface_Capteurs

`LED_Blink` valide la chaîne complète (CubeIDE, compilation, flash, UART). Le programme configure les trois LED de la carte (GPIOG), initialise l'USART1 à 115 200 bauds, puis fait tourner les LED en boucle en envoyant `<ROUGE>`, `<BLEU>` et `<VERT>` sur la console.

`Interface_Capteurs` permet de tester les capteurs avant de les intégrer au reste du projet : les valeurs mesurées sont écrites dans la console série.

### 3.3 Test_Ethernet : ThreadX + NetXDuo + ThingSpeak

**Pourquoi ThreadX et NetXDuo plutôt que FreeRTOS et LwIP ?** Le sujet demande FreeRTOS et LwIP, mais LwIP n'est pas disponible pour cette carte. La pile réseau fournie par ST pour le STM32N6 est **NetXDuo**, qui s'appuie sur **ThreadX** : nous l'avons donc utilisée. Elle remplit les mêmes fonctions (DHCP, ICMP, TCP, DNS, sockets).

**Architecture logicielle** (`app_netxduo.c`) : trois threads ThreadX.

| Thread | Rôle |
|---|---|
| `NxAppThread` | Démarre le client DHCP, attend l'adresse IP via un sémaphore, puis réveille le thread TCP |
| `AppTCPThread` (pile de 6 Ko) | Initialise les capteurs, résout `api.thingspeak.com` par DNS (serveur 8.8.8.8), puis boucle : lecture des capteurs, envoi à ThingSpeak, attente de 20 minutes |
| `AppLinkThread` | Surveille le câble Ethernet toutes les 6 s et relance le DHCP en cas de reconnexion |

**Capteurs** (`sensor_task.c/.h`) : trois capteurs I²C (bus `hi2c1`) avec les drivers de ST.

| Capteur | Grandeur | Configuration |
|---|---|---|
| LPS22DF | Pression (hPa) et température | 4 Hz, moyenne sur 16 échantillons |
| STTS22H | Température | 1 Hz |
| SHT40AD1B | Humidité (%) et température | Mesure à la demande |

Les fonctions `SENSORS_getTemperature()` (moyenne des trois températures pour réduire le bruit et le biais d'un capteur isolé), `SENSORS_getHumidity()` et `SENSORS_getPressure()` fournissent les valeurs utilisées par le modèle.

**Envoi vers ThingSpeak** (`thingspeak.c/.h`) : module indépendant du transport, qui construit les requêtes HTTP (`GET /update`, ou `POST bulk_update.json` pour envoyer jusqu'à 10 mesures d'un coup) et analyse la réponse. Quelques choix notables :

- Pas de `printf("%f")` : la newlib‑nano de CubeIDE ne le gère pas par défaut, les flottants sont donc formatés à la main (2 décimales).
- Le transport (`ts_transport` dans `app_netxduo.c`) ouvre une socket TCP, envoie la requête, lit la réponse jusqu'à la fermeture, puis libère la socket.
- Statuts retournés : `0` OK, `-1` buffer trop petit, `-2` erreur réseau, `-3` code HTTP inattendu, `-4` refus de ThingSpeak (intervalle trop court ou mauvaise clé).

Champs du canal : `field1` = température, `field2` = humidité, `field3` = pression. Période d'envoi : **20 minutes**, ce qui correspond à la période d'échantillonnage attendue par le modèle (voir section 4).

---

## 4. TP3 – Cloud versus Edge AI (problème Meteostat)

### 4.1 Statut par question

| Question du sujet | Statut | Commentaire |
|---|---|---|
| 1.a Entraîner un modèle de classification météo | ✅ | `meteostat.py` (entraîné avec PyTorch) |
| 1.b Compromis classes / taille / accuracy, justification des hyperparamètres | ⚠️ | Choix justifiés ci‑dessous, mais pas de balayage systématique du nombre de classes |
| 2.a Export du modèle (vers ONNX) | ✅ | ONNX float32 puis int8 ; l'import dans MATLAB n'a pas été fait |
| 2.b Script MATLAB d'inférence | ❌ | Priorité donnée au projet final |
| 2.c Classification sur les données ThingSpeak, canal dédié | ❌ | Idem |
| 2.d API TalkBack | ❌ | Idem |
| 3.a Export ONNX pour X‑CUBE‑AI / STM32Cube AI | ✅ | Modèle int8 + fichier de validation `.npz` |
| 3.b Code C d'inférence sur STM32N6 | ⚠️ | Dans l'application finale (voir section 5) |
| 3.c Comparaison avec le cloud (accuracy, temps) | ❌ | La partie cloud n'a pas été réalisée |
| 3.d Mesure de consommation | ❌ | Non réalisée |

### 4.2 Données et classes

Les données viennent de la bibliothèque **Meteostat** (observations horaires de 2015 à 2024) pour **12 villes françaises** (Paris, Brest, Lyon, Marseille, Strasbourg, Chambéry, Toulouse, Lille, Bordeaux, Clermont, Nice, Nantes), avec jusqu'à deux stations par ville. Une station est retenue si elle fournit assez de mesures (au moins 5 000) de code météo et de pression.

- **Entrées : 3 variables** (température, humidité, pression), c'est‑à‑dire exactement ce que mesurent nos capteurs.
- **Fenêtre : 168 heures (7 jours)** par échantillon. Les trous de 3 heures ou moins sont interpolés ; les fenêtres contenant des valeurs manquantes sont écartées.
- **Sortie : 5 classes** (Dégagé, Nuageux, Brouillard, Pluie, Neige), obtenues en regroupant les 27 codes météo (`coco`) de Meteostat. Orage, grêle et averses sont fusionnés avec la pluie ; grésil et neige avec la neige.
- **Séparation temporelle** : entraînement avant 2023, validation en 2023, test en 2024, pour ne pas tester sur des périodes vues à l'entraînement.

**Pourquoi 5 classes et non les 13 du sujet ?** Le sujet propose beaucoup de classes (ciel clair, peu nuageux, partiellement nuageux, averses, orage violent, vent fort…). Or nos capteurs ne mesurent ni la couverture nuageuse fine ni le vent : distinguer « peu nuageux » de « partiellement nuageux » à partir de la température, de l'humidité et de la pression n'est pas réaliste. Chaque classe supplémentaire élargit la couche de sortie, alourdit le modèle et le jeu de données devient plus déséquilibré, pour un gain d'accuracy qu'on ne peut pas obtenir avec ces signaux. Nous avons donc gardé des classes que ces trois grandeurs permettent raisonnablement de séparer.

### 4.3 Modèle et hyperparamètres

CNN 1D (implémenté avec des `Conv2d` de noyau `(1, k)` pour être compatible avec la NPU), environ **30 000 paramètres** :

```
Entrée (3, 1, 168)
 → Conv 3→16  (k=5) + BN + ReLU + MaxPool 2   (168 → 84)
 → Conv 16→32 (k=5) + BN + ReLU + MaxPool 2   (84 → 42)
 → Conv 32→64 (k=5) + BN + ReLU + MaxPool 3   (42 → 14)
 → Conv 64→64 (k=3) + BN + ReLU               (14)
 → Flatten (896) → Dropout 0,3 → Linear 896→5
```

Seules des opérations bien supportées par la NPU sont utilisées (convolution, BatchNorm, ReLU, MaxPool, Linear).

| Choix | Valeur | Justification |
|---|---|---|
| Normalisation | T : (T − 10) / 15 ; HR : (HR − 50) / 30 | Ramener les entrées à des ordres de grandeur voisins |
| Pression | Centrée sur la moyenne de la fenêtre, ÷ 10 | La **tendance** de pression (chute, hausse) renseigne sur le temps, alors que sa valeur absolue dépend de l'altitude et du capteur |
| Augmentation | Décalage aléatoire de la température (σ = 1 °C) et de l'humidité (σ = 3 %) | Nos capteurs ne sont pas les stations d'aéroport de Meteostat ; cela rend le modèle tolérant aux biais de capteur |
| Perte | Entropie croisée pondérée (poids ∝ 1/√effectif) | Compenser le déséquilibre entre classes (le brouillard et la neige sont rares) |
| Optimiseur | AdamW (lr 2e‑3, weight decay 1e‑4) + OneCycleLR (max 3e‑3) | Convergence rapide et régularisation |
| Entraînement | 20 époques, batch de 512, 300 000 échantillons tirés par époque | |
| Sélection du modèle | Meilleur **F1 macro** en validation | L'accuracy seule est trompeuse avec des classes déséquilibrées |
| Dropout | 0,3 | Limiter le surapprentissage |

### 4.4 Export pour la carte

1. Export **ONNX float32** (opset 13, entrée `(1, 3, 1, 168)`).
2. **Quantification statique int8** (format QDQ, par canal) calibrée sur 500 fenêtres d'entraînement.
3. Comparaison float32 / int8 sur 3 000 fenêtres du jeu de test 2024 (accuracy et F1 macro) pour vérifier que la quantification ne dégrade pas le modèle.
4. Génération de `weather_val.npz` (300 échantillons, clés `m_inputs_1` et `m_outputs_1`) pour valider le modèle dans STM32Cube AI Studio.
5. Une fonction `predict_from_buffer` **simule la carte** : elle prend un tampon de 504 mesures (une toutes les 20 minutes), les moyenne par groupes de 3 pour obtenir 168 valeurs horaires, applique la même normalisation et lance l'inférence. Le code C embarqué doit reproduire ce prétraitement à l'identique.

| Résultat (test 2024) | Valeur |
|---|---|
| Accuracy float32 / int8 | ❓ (affichée en fin d'exécution du Colab) |
| F1 macro float32 / int8 | ❓ |
| Taille estimée du modèle | ≈ 120 ko en float32, ≈ 30 ko en int8 (30 k paramètres) |

---

## 5. Application finale

> **Statut : en cours de finalisation.** Le code source du dossier `Application finale/Application` sera publié une fois la version stable. Cette section décrit l'architecture visée.

L'application assemble les briques des TP précédents sur la NUCLEO-N657X0-Q :

```
Capteurs I²C ──► tampon circulaire ──► moyenne horaire ──► normalisation ──► CNN int8 (NPU) ──► classe météo
(T, HR, P)       504 mesures           (3 × 20 min)       (identique au      STM32Cube AI /          │
toutes les       = 7 jours                                  Colab)             ST Edge AI              ▼
20 min                                                                                      ThingSpeak (HTTP)
                                                                                   mesures + prédiction
```

- Les mesures, le prétraitement et le modèle reprennent exactement la chaîne validée dans le Colab (`predict_from_buffer`).
- Les threads ThreadX/NetXDuo et le module `thingspeak.c` du TP2 assurent la communication ; l'envoi publie les mesures et la prédiction sur le canal.
- **Contrainte de démarrage** : le modèle a besoin de 7 jours d'historique (504 mesures). Tant que le tampon n'est pas rempli, aucune prédiction fiable n'est possible.

---

## 6. Écarts entre les sujets et notre réalisation

Plusieurs consignes des sujets ne correspondaient pas au matériel ou aux outils disponibles. Voici ce que nous avons constaté et comment nous l'avons traité.

| Sujet | Constat | Notre choix |
|---|---|---|
| TP2 et cours | Décrivent la N657X0 comme un Cortex‑M33 à 160 MHz avec 320 ko de RAM, ce qui ne correspond pas à la fiche technique ST du STM32N657 (Cortex‑M55, fréquence et mémoire nettement supérieures) | Se référer à la documentation ST |
| TP2 §3 | Cite la NUCLEO‑F746ZQ / STM32F746G‑DISCO, alors que la carte du module est la NUCLEO‑N657X0‑Q | Travail sur la N657X0‑Q |
| TP2 §3 | Demande FreeRTOS + LwIP, non disponibles pour cette carte | ThreadX + NetXDuo |
| TP2 §2 | Liste les capteurs de la carte IKS01A3 (LSM6DSO, LIS2MDL, LIS2DW12, HTS221, LPS22HH, STTS751) | Les capteurs réellement présents sont **LPS22DF, STTS22H et SHT40AD1B** |
| TP2 §2 | Exemple de câblage I²C2 (PA11/PA12) | Notre projet utilise `hi2c1` |
| TP3 | Parle de TensorFlow → ONNX → MATLAB | Entraînement PyTorch avec export ONNX direct |
| Cours / TP3 | Le cours décrit Meteostat comme une prédiction de température (fenêtre de 24 h, sortie sigmoïde), le TP3 comme une classification | Classification 5 classes sur 7 jours |

---

## 7. Sobriété, limites et perspectives (TP4 / P_TEDS)

Principe retenu : l'IA n'a de sens que si le problème n'a pas de solution analytique simple. C'est le cas ici, la relation entre température, humidité, pression et temps qu'il fait n'est pas une formule.

**Ce que nous avons fait pour limiter le coût**

- Un petit modèle (≈ 30 k paramètres) plutôt qu'un gros réseau : le TP1 montre qu'un CNN bien conçu bat un MLP six fois plus gros.
- Une **quantification int8** : 4 fois moins de mémoire que le float32, calcul adapté à la NPU.
- Des classes limitées à ce que les capteurs permettent de distinguer (5 au lieu de 13).
- Une mesure et un envoi toutes les 20 minutes seulement : une période plus longue permet de mettre la carte au repos entre les échantillons.
- Une inférence locale : seules quelques valeurs partent sur le réseau, au lieu d'envoyer toutes les données brutes à traiter dans le cloud.

**Limites actuelles**

- Les mesures partent en **HTTP non chiffré** (port 80) avec la clé d'écriture ThingSpeak dans la requête. Pour un usage réel, il faudrait TLS.
- Le serveur DNS (8.8.8.8) est fixé en dur ; il vaudrait mieux utiliser celui fourni par le DHCP.
- Les données d'entraînement viennent de stations d'aéroport, alors que nos capteurs sont d'une autre nature (placement, échauffement de la carte) ; l'augmentation de données atténue ce décalage sans le supprimer.
- La consommation de la carte n'a pas été mesurée, la comparaison énergétique avec une solution cloud reste à faire.

**Perspectives** : finaliser l'application embarquée, réaliser la partie MATLAB/ThingSpeak (inférence cloud, TalkBack) pour comparer les deux approches en accuracy, latence et énergie, mesurer la consommation avec et sans inférence, ajouter TLS.

---

## 8. Mode d'emploi du code, TP par TP

### Prérequis généraux

- **Python 3.10+** avec `tensorflow`, `torch`, `torchvision`, `numpy`, `pandas`, `matplotlib`, `scikit-learn` (TP1 et TP3). Le plus simple est d'utiliser Google Colab.
- **STM32CubeIDE** pour les projets des TP2 et de l'application finale, plus une carte NUCLEO‑N657X0‑Q (câble USB pour le ST‑LINK).
- Un terminal série (115 200 bauds, 8N1) pour lire les logs.

### TP1 – MNIST

1. Ouvrir `ETRS923.ipynb` dans Colab (le lien se trouve dans l'en‑tête du notebook).
2. Exécuter les cellules dans l'ordre. La première clone ce dépôt et ajoute `TP_1` au `sys.path` pour importer `callbackPlots.py`.
3. Chaque section produit ses résultats : courbes d'accuracy en direct, graphiques `.png`, tableau `multi_architecture_test_results.csv` et logs `.txt`.
4. La grille complète de la cellule 1.e prend plusieurs minutes ; utiliser un GPU Colab pour les parties PyTorch.

Fichiers utilitaires :

| Fichier | Utilisation |
|---|---|
| `callbackPlots.py` | `AccuracyPlotCallback()` à passer à `model.fit(..., callbacks=[...])` ; `count_neurons_and_synapses(model)` affiche neurones et synapses des couches denses. Nécessite Jupyter/Colab (affichage via IPython) |
| `callbacks_torch.py` | Mêmes fonctions pour PyTorch : appeler `on_epoch_end(epoch, {'accuracy':…, 'val_accuracy':…})` à chaque époque |
| `MNIST_cnn_functions.py` | Fonctions TensorFlow bas niveau (`weight_variable`, `conv_layer`, `max_pool_2x2`, `full_layer`…). Non importées par le notebook final |
| `npu_TB.py` | `python npu_TB.py` : mesure le temps de 20 produits matriciels sur NPU Intel. Requiert un PC Intel avec NPU et `intel_npu_acceleration_library` |
| `npu_callbacks.py` | Fournit `NpuLinear` (remplaçant de `nn.Linear`) et un callback d'accuracy ; même prérequis |
| `model_Accu*.keras` | Chargement : `tf.keras.models.load_model("TP_1/model_Accu0.9653.keras")` |

### TP2 – LED, capteurs, réseau

Pour chaque projet : *File → Import → Existing Projects into Workspace*, sélectionner le dossier, puis *Build* (marteau) et *Run* ou *Debug*.

**`LED_Blink`**
1. Compiler et flasher.
2. Ouvrir le terminal série : les messages `<ROUGE>`, `<BLEU>`, `<VERT>` défilent au rythme du chenillard.
3. Régler la vitesse avec la variable `delay` (en ms) dans `main.c`.

**`Interface_Capteurs`**
1. Monter la carte capteurs sur la NUCLEO.
2. Compiler, flasher, ouvrir le terminal série.
3. Les mesures sont affichées dans la console. Si un capteur n'est pas détecté, vérifier le câblage et les adresses I²C (7 bits dans les drivers, souvent décalées d'un bit pour la HAL).
4. Pour afficher des flottants avec `printf`, activer `-u _printf_float` dans les options du compilateur (CubeIDE : *Project Settings → MCU Settings*).

**`Test_Ethernet`**
1. Créer un canal ThingSpeak avec 3 champs (température, humidité, pression) et noter la **Write API Key**.
2. Dans `app_netxduo.c`, renseigner la constante `TS_WRITE_KEY` avec votre clé. Ne publiez pas votre clé sur un dépôt public.
3. Brancher la carte sur un réseau avec DHCP et accès Internet.
4. Compiler, flasher, ouvrir le terminal série. Séquence attendue :
   ```
   Looking for DHCP server ..
   STM32 IpAddress: 192.168.x.x
   Creating DNS client
   Looking for DNS host name
   ThingSpeak status: 0
   ```
   Le statut `0` signifie que ThingSpeak a accepté la mesure. Un `-4` indique un refus (clé erronée, ou envoi à moins de 15 s du précédent en compte gratuit).
5. Une mesure est envoyée toutes les 20 minutes. Pour tester plus vite, réduire `1200` dans `tx_thread_sleep(1200 * TX_TIMER_TICKS_PER_SECOND)` (minimum 15 s).
6. Adapter l'envoi à un autre transport : `thingspeak.c` ne dépend que de la fonction `ts_transport` fournie à `ts_send_update(...)`.

Utilisation du module ThingSpeak dans un autre projet :

```c
ts_sample_t s;
ts_sample_init(&s, 0);
ts_sample_set(&s, 1, temperature);   // field1
ts_sample_set(&s, 2, humidity);      // field2
ts_sample_set(&s, 3, pressure);      // field3
ts_status_t st = ts_send_update(ts_transport, NULL, TS_WRITE_KEY, &s);
```

### TP3 – Entraînement Meteostat et export

1. Ouvrir `meteostat.py` dans un notebook Colab (le script est issu d'un notebook ; les lignes `!pip` et `google.colab.files` supposent Colab) avec un GPU si possible.
2. Exécuter les cellules dans l'ordre :
   - **Extraction** : téléchargement des données Meteostat, construction des fenêtres et séparation temporelle. Les paramètres à modifier se trouvent en tête : `CITIES`, `W` (taille de fenêtre, 168), `HORIZON_H` (0 = temps actuel ; 3 ou 6 pour prédire à 3 h ou 6 h).
   - **Entraînement** : affiche la perte, l'accuracy et le F1 macro par époque, puis le rapport par classe et la matrice de confusion sur 2024. Sauvegarde `weather_cnn.pt`.
   - **Export** : produit `weather_cnn.onnx`, `weather_cnn_int8.onnx` et `weather_val.npz`, puis télécharge les deux derniers.
   - **Simulation de la carte** : `predict_from_buffer(buf)` prend un tampon `(504, 3)` (°C, %, hPa) et affiche les trois classes les plus probables.
3. Importer `weather_cnn_int8.onnx` dans **STM32Cube AI Studio** (ou X‑CUBE‑AI), cible STM32N6, puis valider avec `weather_val.npz`.

Si vous changez `W` ou la normalisation, il faut changer le prétraitement côté C de la même façon, sinon les prédictions de la carte seront fausses.

### Application finale

Ouvrir `Application finale/Application` dans STM32CubeIDE, renseigner la clé ThingSpeak comme pour `Test_Ethernet`, compiler et flasher. Le détail du fonctionnement sera complété lors de la publication du code source.

---

## Licence

Voir le fichier [`LICENSE`](LICENSE). Les drivers de capteurs et les composants ThreadX/NetXDuo/HAL restent soumis à leurs licences d'origine (STMicroelectronics).
