library(tiff)
img<-read.tiff.image(image.file = "inst/extradata/ischemia_sample.tif")
library(reshape2)
img.sorted<-extract.x.y(img.mtx = img)
img.sorted<-as.matrix(img.sorted)
img.mtx.melt<-melt(img)
colnames(img.mtx.melt)<-c("X","Y","V")
cluster.list<-get.clusters(img.mtx = img, sorted.x.y = img.sorted,intensity.cutoff = 0.1,mean.width = 45,var.width = 15)

library(grid)
library(gridExtra)

mles<-get.MLEs(cluster.list)
colnames(mles)<-c("X","Y","V")
library(ggplot2)

png(file = "overlay.ischemia.png",width =2048,height=2048)
ggplot() + geom_tile(data=img.mtx.melt,mapping=aes(y=Y,x=X,fill=V))+
  scale_fill_gradient(low="black", high="green")+
  geom_point(data=mles, mapping=aes(y=Y,x=X),size=5,colour="red")
dev.off()