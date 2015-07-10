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

window$setVisible(TRUE)

