hash = []

def insertEncadeamento():
    for i in range(0, tam):
        hash.append([])
    elem = int(input("Insira um novo número: "))
    while(elem != -1):
        ind = elem % tam
        hash[ind].append(elem)
        print(hash)
        elem = int(input("Insira um novo número: "))
    print(hash)

def insertLinear():
    for i in range(0, tam):
        hash.append(-1)
    elem = int(input("Insira um novo número: "))
    while(elem != -1):
        i = 0
        ind = (elem + i) % tam
        while hash[ind] != -1:
            print("Sofreu colisao\n")
            i += 1
            ind = (elem + i) % tam
        hash[ind] = elem
        print(hash)
        elem = int(input("Insira um novo número: "))
    print(hash)

def insertQuadratico():
    for i in range(0, tam):
        hash.append(-1)
    elem = int(input("Insira um novo número: "))
    while(elem != -1):
        i = 0
        ind = (elem + (i ** 2)) % tam
        while hash[ind] != -1:
            print("Sofreu colisao\n")
            i += 1
            ind = (elem + (i ** 2)) % tam
        hash[ind] = elem
        print(hash)
        elem = int(input("Insira um novo número: "))
    print(hash)

def buscar():
    if tipo == 1:
        chave = int(input("Digite a chave que deseja buscar: "))
        while chave != -1:
            ind = chave % tam
            if len(hash[ind]) == 0:
                print("Chave nao encontrada")
            else:
                print("Chave encontrada no indice %d" % ind)
            chave = int(input("Digite a chave que deseja buscar: "))
    elif tipo == 2:
        chave = int(input("Digite a chave que deseja buscar: "))
        while chave != -1:
            i = 0
            ind = (chave + i) % tam
            while hash[ind] != -1 and hash[ind] != chave:
                i += 1
                ind = (chave + i) % tam
            if hash[ind] == -1 or hash[ind] != chave:
                print("Chave nao encontrada")
            else:
                print("Chave encontrada no indice %d" % ind)
            chave = int(input("Digite a chave que deseja buscar: "))
    elif tipo == 3:
        chave = int(input("Digite a chave que deseja buscar: "))
        while chave != -1:
            i = 0
            ind = (chave + (i ** 2)) % tam
            while hash[ind] != -1 and hash[ind] != chave:
                i += 1
                ind = (chave + (i ** 2)) % tam
            if hash[ind] == -1 or hash[ind] != chave:
                print("Chave nao encontrada")
            else:
                print("Chave encontrada no indice %d" % ind)
            chave = int(input("Digite a chave que deseja buscar: "))

def insertDuplo():
    return True

tam = int(input("Tamanho da hash: "))
tipo = int(input("1 - Encadeamento\n2 - Reespalhamento Linear\n3 - Reespalhamento Quadrático\n4 - Reespalhamento Duplo\n"))

if(tipo == 1):
    insertEncadeamento()
elif(tipo == 2):
    insertLinear()
elif(tipo == 3):
    insertQuadratico()

cont = int(input("Deseja fazer busca? 1 - Sim 2 - Nao\n"))
if cont == 1:
    buscar()
