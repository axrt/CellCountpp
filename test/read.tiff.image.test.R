image.file<-"inst/extradata//control_sample.tif"
img.mtx<-read.tiff.image(image.file = image.file)
head(img.mtx)
max(img.mtx)
min(img.mtx)
img.mtx<-read.tiff.image(image.file = image.file,normalize = FALSE)
head(img.mtx)
max(img.mtx)
min(img.mtx)
