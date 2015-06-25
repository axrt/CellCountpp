source("test/read.tiff.image.test.R")
img.mtx.melt<-extract.x.y(img.mtx = img.mtx)
img.mtx.melt<-extract.x.y(img.mtx = img.mtx, sort.by.intensity.val = TRUE)
