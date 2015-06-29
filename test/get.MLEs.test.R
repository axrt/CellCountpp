img<-read.tiff.image(image.file = "inst/extradata/control_sample.tif")
library(reshape2)
img.sorted<-extract.x.y(img.mtx = img)
img.sorted<-as.matrix(img.sorted)
cluster.list<-get.clusters(img.mtx = img, sorted.x.y = img.sorted,intensity.cutoff = 0.4,mean.width = 45,var.width = 8)

library(grid)
library(gridExtra)

mles<-get.MLEs(cluster.list)
library(ggplot2)
png(file = "overlay.png",width =2048,height=2048)
ggplot() + geom_tile(data=img.mtx.melt,mapping=aes(y=Y,x=X,fill=V))+
  scale_fill_gradient(low="black", high="green")+
  geom_point(data=mles, mapping=aes(y=Y,x=X),size=5,colour="red")
dev.off()