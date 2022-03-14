#def computeBest(minDelta):
#def findPossibleDelta(cur, end, wanted, minRatio):
#  delta=cur-end
#  print(cur,end,delta)
def findMaxDiv(value):
  ret=1
  subDiv=1
  while not value%10:
    value/=10
    ret*=10
  if not value%25:
    ret*=100
    subDiv=4
  elif not value%5:
    ret*=10
    subDiv=2
  elif not value%2:
    ret*=10
    subDiv=5
  return (ret,subDiv)
def enumSteps(cur):
  maxDiv,subDiv=findMaxDiv(cur)
  if subDiv==2:
    yield int(cur-maxDiv/subDiv)
    if not maxDiv%(subDiv*2):
      yield int(cur-maxDiv/(subDiv*2))
    maxDiv/=10
    subDiv=1
  elif subDiv==4:
    yield int(cur-maxDiv/subDiv)
    maxDiv/=10
    subDiv=1
  elif subDiv==5:
    yield int(cur-maxDiv/subDiv)
    maxDiv/=10
    subDiv=1    
  #if not maxDiv%subDiv and maxDiv/subDiv>cur:
  #  yield maxDiv//subDiv

  while True:
    if not maxDiv%subDiv:
      yield int(cur-maxDiv/subDiv)
    if subDiv==5:
      if maxDiv==1:
        break
      subDiv=1
      maxDiv=int(maxDiv/10)
    elif subDiv==4:
      subDiv=5
    elif subDiv==2:
      subDiv=4
    elif subDiv==1:
      subDiv=2
    else:
      raise "Invalid subdiv"%(subDiv,)
res=100

minRatio=7.53/7.4
minRatio=minRatio*0.999
cur=100*res/10
end=1*res
#
delta=cur-end
next=int(cur/minRatio)
#maxDiv=findMaxDiv(cur)
#print(cur,maxDiv)
while True:
  valid=None
  for step in enumSteps(cur):
    if step>=end:
      #print("Test",cur/res,end/res,step/res,cur/step)
      if cur/step>minRatio:
        valid=step
      else:
        break
  if valid is None:
    break
  cur=valid
  print("Step",valid)
#findPossibleDelta(cur,end, next, minRatio)

##from decimal import Decimal, getcontext
##
###divisions={1: [2, 4, 5], 2: [2], 3:[2], 4:[2,4], 5:[2, 5], 6:[2,3], 7:[7], 8: [2,4], 9: [3,9]}
##
##
####def toInt(n):
####  return Decimal(n)
####
####divisor=Decimal("10.00000")
####
##
###x=Decimal( 
###divisions=[Decimal(1),Decimal(10)]
##def lastDigits(n):
##  n=Decimal(n).normalize()
##  s=-n.adjusted()+len(n.as_tuple().digits)-1
##  return int(n.scaleb(s))
###def dividers(i):
###  ret=[]
###  m=int(i**.5)
###  if i==1:
###    ret=[(2,10),(4,10),(5,10)]
###  else if i<10:
###    for i in range(j
###  for j in range(1,m+1):
###    if not i%j:
###      ret.append(int(i/j))
###  return ret
##def calcWorstRatio(cur):
##  worstRatio=None
##  worstIndex=None
##  for i in range(len(cur)-1):
##    ratio=float(cur[i+1]/cur[i])
##    if worstRatio is None or ratio>worstRatio:
##      worstRatio=ratio
##      worstIndex=i
##  return (worstRatio,worstIndex)
##
##active={None:set([(Decimal(1), Decimal(10))])}
##done=set()
###def addTest(test):
##curN=1
##maxN=100
##while curN<maxN:
##  curN+=1
##  curIndex=max(active)
##  cur=active[curIndex].pop()
##  if not active[curIndex]:
##    del active[curIndex]
##  worstRatio,worstIndex=calcWorstRatio(cur)
##  #if curN>maxN-1:
##  print(worstRatio, cur)
##
##  begin=cur[worstIndex]
##  end=cur[worstIndex+1]
##  delta=end-begin
##  lastDelta=lastDigits(delta)
##  if lastDelta==1:
##    lastDelta=10
##  if lastDelta<=10:
##    for i in range(1,lastDelta):
##      new=list(cur)
##      new.insert(worstIndex+1, (begin+delta*i/lastDelta).normalize())
##      new=tuple(new)
##      if new not in done:
##        newWorstRatio,newWorstIndex=calcWorstRatio(new)
##        active.setdefault(newWorstRatio, set()).add(new)
##        done.add(new)
##  #break
###print(active)
###print(dividers(104))
####  #print(n, s, )#int(n.shift(s).to_integral_exact()))
####
#####test("10.00000")
#####test(Decimal(10).normalize())
#####test("10")
#####test("0.00104")
#####test("1.04")
####
####active={}
####done=set()
####
####def addTest(nFrom, nTo):
####  cur=(nFrom,nTo)
####  if cur not in done:
####    delta=(Decimal(nTo)-Decimal(nFrom)).normalize()
####    deltaOrder=-delta.adjusted()+len(delta.as_tuple().digits)
####    if deltaOrder not in active:
####      active[deltaOrder]=set()
####    active[deltaOrder].add(cur)
####    
####  
####addTest(1,  10)
####
####print(active)
####n=0
####while active:
####  n+=1
####  if n>2: break
####  curOrd=max(active)
####  (begin,end)=active[curOrd].pop()
####  if not active[curOrd]:
####    del active[curOrd]
####  delta=end-begin
####  deltaNor=lastDigits(delta)
####  print(begin,end,deltaNor)
####  #(begin,end)=active.pop()
####  #delta=(end-begin).normalize()
####  #print(begin,end,delta)
##
