import sys

def dobijIndeksPrijelaza(L, p):
    for k in range(len(p)):
        for m in range(len(L)):
            if p[k] in L[m]:
                p[k] = m
    return p

def dfsr(st):
    if st in visited:
        return
    visited.add(st)
    for i in range(len(abeceda)):
        dfsr(prijelazi[stanja.index(st)][i])
    return


stanja = []
abeceda = []
prihvatljivo = []
pocetno = 0
prijelazi=[]
content = " "
j=0
for content in sys.stdin:
    content=content[:len(content)-1]
    if(j==0):
        stanja=content.split(",")
    elif(j==1):
        abeceda=content.split(",")
    elif(j==2):
        prihvatljivo=content.split(",")
    elif(j==3):
        pocetno=content
    else:
        if(j==4):
            for i in range(len(stanja)):
                prijelazi.append([])
                for k in range(len(abeceda)):
                    prijelazi[i].append(0)
        if(content!="\n"):
            novo=content.split("->")[1]
            start=content.split("->")[0].split(",")[0]
            simbol=content.split("->")[0].split(",")[1]
            prijelazi[stanja.index(start)][abeceda.index(simbol)]=novo
    j=j+1

visited=set()
dfsr(pocetno)
stanja2=list(visited)
states=[]
states.append([])
states.append([])
for i in range (len(stanja2)):
    if(stanja2[i] in prihvatljivo):
        states[1].append(stanja2[i])
    else:
        states[0].append(stanja2[i])
if (not states[1]):
    states.pop()
novo=1
while(novo):
    newStates=[]
    novo=0
    for i in range(len(states)):
        p = []
        nn=0
        newStates.append([])
        newStates.append([])
        for j in range(len(states[i])):
            if(j==0):
                newStates[len(newStates)-2].append(states[i][j])
                for k in range(len(abeceda)):
                    p.append(prijelazi[stanja.index(states[i][j])][k])
                p=dobijIndeksPrijelaza(states, p)
            else:
                p2=[]
                for k in range(len(abeceda)):
                    p2.append(prijelazi[stanja.index(states[i][j])][k])
                p2=dobijIndeksPrijelaza(states, p2)
                if(p!=p2):
                    newStates[len(newStates)-1].append(states[i][j])
                    nn=1
                else:
                    newStates[len(newStates) - 2].append(states[i][j])
        if(nn==0):
            newStates.pop()
        else:
            novo=1
    states=newStates





stanjan=[]
str=""
for i in range(len(states)):
    states[i].sort()
    if(not states[i]):
        a=1
    else:
        stanjan.append(states[i][0])
for i in range(len(prijelazi)):
    for j in range(len(abeceda)):
        for k in range(len(states)):
            if(prijelazi[i][j] in states[k]):
                prijelazi[i][j]=states[k][0]
for i in range(len(states)):
    if(pocetno in states[i]):
        pocetno=states[i][0]
stanjan.sort()
for i in range(len(stanjan)):
    if(i!=0):
        str+=","
    str+=stanjan[i]
print(str)
str=""
for i in range(len(abeceda)):
    if(i!=0):
        str+=","
    str+=abeceda[i]
print(str)
str=""
prihvatljivon=[]
for i in range (len(prihvatljivo)):
    if prihvatljivo[i] in stanjan:
        prihvatljivon.append(prihvatljivo[i])
for i in range(len(prihvatljivon)):
    if(i!=0):
        str+=","
    str+=prihvatljivon[i]
print(str)
print(pocetno)
prijelazin=[]
str=""
for i in range(len(stanja)):
    if(stanja[i] in stanjan):
        prijelazin.append(prijelazi[i])
for i in range(len(prijelazin)):
    for j in range(len(abeceda)):
        str=""
        str+=stanjan[i]
        str+=","
        str+=abeceda[j]
        str+="->"
        str+=prijelazin[i][j]

        print(str)
