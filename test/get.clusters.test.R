img<-read.tiff.image(image.file = "inst/extradata/control_sample.tif")
library(reshape2)
img.sorted<-extract.x.y(img.mtx = img)
img.sorted<-as.matrix(img.sorted)
cluster.list<-get.clusters(img.mtx = img, sorted.x.y = img.sorted,intensity.cutoff = 0.7,mean.width = 25,var.width = 10)

