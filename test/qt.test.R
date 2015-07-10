library(qtbase)

window<-Qt$QWidget()

window$setWindowTitle("Test Main Window")
layout<-Qt$QGridLayout()
window$setLayout(layout)

scroll.area <-Qt$QScrollArea()
layout$addWidget(scroll.area)

label<-Qt$QLabel()
label$pixmap<-Qt$QPixmap("inst/extradata/ischemia_sample.png")

scroll.area$setWidget(label)



zoom.image<-function(x=2.0){
  
  label$resize(x*label$pixmap$size())
  update.scroll.bar<-function(scbr){
    scbr$value <- x*sb$value+(x-1)*sb*sb$pageStep/2
  }
  update.scroll.bar(scroll.area$horizontalScrollBar()) 
  update.scroll.bar(scroll.area$verticalScrollBar()) 
}

qconnect(x=label, signal="clicked",handler = function(i){print(i)})
window$setVisible(TRUE)