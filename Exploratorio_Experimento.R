library(readxl)
library(ggplot2)
library(tidyverse)
library(dplyr)

Datos <- read_excel("Maestría Inteligencia Artificial/Computación Alto Desempeño/HPC_Project/Datos.xlsx")

# Libraries

# Distrubuciones tamaño 1000
p<-Datos %>% filter(Size_1000==1) %>% ggplot(aes(x=log(Time_m), color=Size_1000, fill=Method))+
  geom_density(alpha=0.6)+xlim(0,5)
p

# Distrubuciones tamaño 2000
p<-Datos %>% filter(Size_1000==2) %>% ggplot(aes(x=log(Time_m), color=Size_1000, fill=Method))+
  geom_density(alpha=0.6)+xlim(0,10)
p


# Distrubuciones tamaño 4000
p<-Datos %>% filter(Size_1000==4) %>% ggplot(aes(x=log(Time_m), color=Size_1000, fill=Method))+
  geom_density(alpha=0.6)+xlim(0,10)
p

#Distrubuciones tamaño 8000
p<-Datos %>% filter(Size_1000==8) %>% ggplot(aes(x=log(Time_m), color=Size_1000, fill=Method))+
  geom_density(alpha=0.6)+xlim(5,10)
p

# Aunque en los gráficos por el momento no se encuentra descontado el efecto de cada uno de los nodos podemos evienciar
# que el metodo MM2f es el mas eficiente, eso se nota por que tienen en todas las distibuciones los primeros picos
# comparados con los otros métodos matriciales

agg_tbl<-Datos %>% select(Method,Size_1000,TH, Time_m)%>%
group_by(Method,TH,Size_1000) %>% summarise(avg = mean(Time_m)) %>% as.data.frame()

# Puede comprobarse que el metodo MM2f es superior.

p <- ggplot(agg_tbl, aes(x=log(avg), y=Method)) + geom_boxplot()+xlim(0,10)
p


# Se eviencia una diferencia en los metodos, a partir de un promedio de los tiempo
agg_tbl<-Datos %>% select(Method, Time_m)%>%
  group_by(Method) %>% summarise(avg = mean(Time_m), median_time = median(Time_m), std_time = sd(Time_m))  %>% 
  as.data.frame()

# Se evidencia que a medida que avanzan los nodos hay un menor tiempo, siento un nodo el mayor tiempo de procesamiento
t1<-with(Datos, tapply(Time_m, list(Method, TH), FUN=mean))
table1<-prop.table(t1, margin = 1) %>% round(2) # margin = 1 for row
# Se rechaza la hipotesis de similitud por medio del Chi Cuadrado
chires <- chisq.test(table1)
chires
mosaicplot(table1)   

# Lo modelos de MM2f son los modelos con menor variación y mayor eficiencia
table1<-prop.table(t1, margin = 2) %>% round(2) # margin = 1 for row
# Se rechaza la hipotesis de similitud por medio del Chi Cuadrado
chires <- chisq.test(table1)
chires
mosaicplot(table1)  



# Se evidencia que el numero de nodos si afecta el proceso
agg_tbl<-Datos %>% select(TH, Time_m)%>%
  group_by(TH) %>% summarise(avg = mean(Time_m), median_time = median(Time_m), std_time = sd(Time_m))  %>% 
  as.data.frame()

# Se evidencia que a medida que avanzan los nodos hay un menor tiempo, siento un nodo el mayor tiempo de procesamiento
t1<-with(Datos, tapply(Time_m, list(Method, Size_1000), FUN=mean))
table1<-prop.table(t1, margin = 1) %>% round(2) # margin = 1 for row
# Se rechaza la hipotesis de similitud por medio del Chi Cuadrado
chires <- chisq.test(table1)
chires
mosaicplot(table1)   

# Lo modelos de MM2f son los modelos con menor variación y mayor eficiencia
table1<-prop.table(t1, margin = 2) %>% round(2) # margin = 1 for row
# Se rechaza la hipotesis de similitud por medio del Chi Cuadrado
chires <- chisq.test(table1)
chires
mosaicplot(table1)  



agg_tbl<-Datos %>% select(Size_1000, Time_m)%>%
  group_by(Size_1000) %>% summarise(avg = mean(Time_m), median_time = median(Time_m), std_time = sd(Time_m))  %>% 
  as.data.frame()




# Se evidencia que el numero de matrices afecta de forma exponencial la cantidad el tiempo necesario para su ejecución
agg_tbl<-Datos %>% select(Size_1000, Time_m)%>%
  group_by(Size_1000) %>% summarise(avg = mean(Time_m), median_time = median(Time_m), std_time = sd(Time_m))  %>% 
  as.data.frame()


# Regresión MM1f
df1<-Datos %>% filter(Method=="MM1f")
mod1<-lm(log(Time_m) ~ log(Size_1000)*log(TH),df1)
summary(mod1)
plot(mod1)
res <- mod1$residuals
plot(res ~ mod1$fitted.values, pch=21, col="black", bg="salmon",
     xlab="Fitted Value", ylab="Residuals", main="Residual Plot")
abline(h=0, col="darkred", lwd=2)


# Se evidencia que los residuales son normales
shapiro.test(res)
par(mfrow=c(1,2))
hist(res, col='orchid', breaks=10)
qqnorm(res)
qqline(res)
par(mfrow=c(1,1))



# Regresión MM1fu
df1<-Datos %>% filter(Method=="MM1fu")
mod2<-lm(log(Time_m) ~ log(Size_1000)*log(TH),df1)
summary(v)
plot(mod2)
res <- mod2$residuals
plot(res ~ mod2$fitted.values, pch=21, col="black", bg="salmon",
     xlab="Fitted Value", ylab="Residuals", main="Residual Plot")
abline(h=0, col="darkred", lwd=2)

# Se evidencia que los residuales son normales
shapiro.test(res)
par(mfrow=c(1,2))
hist(res, col='orchid', breaks=10)
qqnorm(res)
qqline(res)
par(mfrow=c(1,1))


# Regresión MM2f
df1<-Datos %>% filter(Method=="MM2f")
mod3<-lm(log(Time_m) ~ log(Size_1000)*log(TH),df1)
summary(mod3)
plot(mod3)
res <- mod3$residuals
plot(res ~ mod3$fitted.values, pch=21, col="black", bg="salmon",
     xlab="Fitted Value", ylab="Residuals", main="Residual Plot")
abline(h=0, col="darkred", lwd=2)

# Se evidencia que los residuales son normales
shapiro.test(res)
par(mfrow=c(1,2))
hist(res, col='orchid', breaks=10)
qqnorm(res)
qqline(res)
par(mfrow=c(1,1))



# por los valores del coeficiente en el intercepto,  se evidencia que si existe una gran diferencia entra los 
exp(mod1$coefficients)[1]
exp(mod2$coefficients)[1]
exp(mod3$coefficients)[1]



df1<-Datos %>% filter(Method=="MM2f", Size_1000==1)
agg_tbl<-df1 %>% select(TH, Time_m)%>% group_by(TH) %>% summarise(avg = mean(Time_m))  %>%  as.data.frame()
ggplot(data=agg_tbl, aes(x=avg, y=TH)) +geom_line(linetype = "dashed")+geom_point()

df1<-Datos %>% filter(Method=="MM2f", Size_1000==2)
agg_tbl<-df1 %>% select(TH, Time_m)%>% group_by(TH) %>% summarise(avg = mean(Time_m))  %>%  as.data.frame()
ggplot(data=agg_tbl, aes(x=avg, y=TH)) +geom_line(linetype = "dashed")+geom_point()

df1<-Datos %>% filter(Method=="MM2f", Size_1000==4)
agg_tbl<-df1 %>% select(TH, Time_m)%>% group_by(TH) %>% summarise(avg = mean(Time_m))  %>%  as.data.frame()
ggplot(data=agg_tbl, aes(x=avg, y=TH)) +geom_line(linetype = "dashed")+geom_point()

df1<-Datos %>% filter(Method=="MM2f", Size_1000==8)
agg_tbl<-df1 %>% select(TH, Time_m)%>% group_by(TH) %>% summarise(avg = mean(Time_m))  %>%  as.data.frame()
ggplot(data=agg_tbl, aes(x=avg, y=TH)) +geom_line(linetype = "dashed")+geom_point()

