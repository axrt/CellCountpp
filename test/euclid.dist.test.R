a<-seq(1,2.5,0.5)
b<-rep(2,4)
euclid.dist(Rx = a, Ry = b)
dist(rbind(a,b))

euclid.dist(rep(1,5),rep(1,6))
euclid.dist(rep("a",4),rep("b",4))

mini.df<-rbind(a,b)
system.time(sapply(1:1e5,function(i){
  return(dist(mini.df))
}))

system.time(sapply(1:1e5,function(i){
  return(euclid.dist(a,b))
}))