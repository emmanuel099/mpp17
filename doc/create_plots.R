library(ggplot2)
library(dplyr)

lower_ci <- function(a) {
  return(mean(a) - qt(0.975, df=length(a)-1)*sd(a)/sqrt(length(a)))
}

upper_ci <- function(a) {
  return(mean(a) + qt(0.975, df=length(a)-1)*sd(a)/sqrt(length(a)))
}

read_file_data <- function(fname, algo) {
  df <- read.table(list.files(pattern = glob2rx(fname)), header=FALSE, sep=";")
  names(df) <- c("strategy", "height", "threads", "items", "init_items", "scale", "rep", "total_time", "total_throughput", "inserts", 
                 "failed_inserts", "insert_retries", "insert_throughput", "removals", "failed_removals", "remove_retries", "remove_throughput",
                 "finds", "find_retries", "find_throughput")
  df$algorithm <- algo
  
  df1 <- aggregate(cbind(total_throughput, insert_retries, remove_retries, find_retries)~strategy+threads+scale+init_items+algorithm, data=df, mean)
  df1$lower_ci <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df, lower_ci)$total_throughput
  df1$upper_ci <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df, upper_ci)$total_throughput
  
  df1$insert_retries_lower_ci <- aggregate(cbind(insert_retries)~strategy+threads+scale+init_items+algorithm, data=df, lower_ci)$insert_retries
  df1$insert_retries_upper_ci <- aggregate(cbind(insert_retries)~strategy+threads+scale+init_items+algorithm, data=df, upper_ci)$insert_retries
  
  df1$remove_retries_lower_ci <- aggregate(cbind(remove_retries)~strategy+threads+scale+init_items+algorithm, data=df, lower_ci)$remove_retries
  df1$remove_retries_upper_ci <- aggregate(cbind(remove_retries)~strategy+threads+scale+init_items+algorithm, data=df, upper_ci)$remove_retries
  
  df1$find_retries_lower_ci <- aggregate(cbind(find_retries)~strategy+threads+scale+init_items+algorithm, data=df, lower_ci)$find_retries
  df1$find_retries_upper_ci <- aggregate(cbind(find_retries)~strategy+threads+scale+init_items+algorithm, data=df, upper_ci)$find_retries
  
  return(df1)
}

df1 <- read_file_data("LazySkipList*.csv", "LazySkipList")
df2 <- read_file_data("LockFreeSkipList*.csv", "LockFreeSkipList")
df3 <- read_file_data("ConcurrentSkipList*.csv", "ConcurrentSkipList")
df4 <- read_file_data("MMLazySkipList*.csv", "MMLazySkipList")
df5 <- read_file_data("SequentialSkipList*.csv", "SequentialSkipList")

strategies <- c("interleaving insert - no failed inserts", "interleaving remove - no failed removes", 
                "mixed workload - 70% insert / 30% remove", "mixed workload - 50% insert / 20% remove / 30% search")

scales <- c("strong")
init_items <- c(0)

for (cur_strat in strategies) {
    for (cur_scale in scales) {
      for (cur_init_items in init_items) {
        df1.1 <- subset(df1, scale == cur_scale & strategy == cur_strat & init_items == cur_init_items)
        df2.1 <- subset(df2, scale == cur_scale & strategy == cur_strat & init_items == cur_init_items)
        df3.1 <- subset(df3, scale == cur_scale & strategy == cur_strat & init_items == cur_init_items)
        df4.1 <- subset(df4, scale == cur_scale & strategy == cur_strat & init_items == cur_init_items)
        df5.1 <- subset(df5, scale == cur_scale & strategy == cur_strat & init_items == cur_init_items)
        
        df <- rbind(df1.1, df2.1, df3.1, df4.1, df5.1)
        
        # throughput plot
        p1 <- ggplot(df, aes(x=threads, y=total_throughput / 1000, group=algorithm, color=algorithm)) + 
          geom_errorbar(aes(ymin=lower_ci/1000, ymax=upper_ci/1000), width=1) +
          geom_point() +
          geom_line() +
          theme_bw() +
          theme(axis.text.x = element_text(angle = 60, hjust=1),
                legend.position="right") +
          ylab("throughput [ops/msec]") +
          xlab("threads")
        plot(p1)
        
        filename <- paste(cur_strat , "_", cur_scale, "_", cur_init_items, ".pdf", sep="")
        filename <- gsub("/", "," , filename)
        filename <- gsub("%", "p", filename)
        filename <- gsub(" ", "_", filename)
        
        pdf(paste("plots/", filename, sep=""), width=6, height=4)
        plot(p1)
        dev.off()
      }
  }
}


strategies <- c("interleaving insert - no failed inserts", "interleaving remove - no failed removes", 
                "mixed workload - 70% insert / 30% remove", "mixed workload - 70% insert / 30% remove",
                "mixed workload - 50% insert / 20% remove / 30% search", "mixed workload - 50% insert / 20% remove / 30% search", 
                "mixed workload - 50% insert / 20% remove / 30% search")

columns <- c("insert_retries", "remove_retries", "insert_retries", "remove_retries", "insert_retries", "remove_retries", "find_retries")
axis_desc <- c("insert", "remove", "insert", "remove", "insert", "remove", "find")


cur_init_items <- 0
cur_scale <- "strong"
for (i in seq(1, length(strategies))) {

  df1.1 <- subset(df1, scale == cur_scale & strategy == strategies[i] & init_items == cur_init_items)
  df2.1 <- subset(df2, scale == cur_scale & strategy == strategies[i] & init_items == cur_init_items)
  df4.1 <- subset(df4, scale == cur_scale & strategy == strategies[i] & init_items == cur_init_items)
  
  df <- rbind(df1.1, df2.1, df4.1)
  
  p2 <- ggplot(df, aes(x=threads, y=df[, columns[i]], group=algorithm, fill=algorithm, width=5)) + 
     geom_bar(stat="identity", position=position_dodge(), color="black") +
     geom_errorbar(aes(ymin=df[, paste(columns[i], "_lower_ci", sep="")], ymax=df[, paste(columns[i], "_upper_ci", sep="")], width=1),
                   position=position_dodge(5)) +
     theme_bw() +
     theme(axis.text.x = element_text(angle = 60, hjust=1),
           legend.position="top") +
     ylab(paste("#retries /", axis_desc[i])) +
     xlab("threads")
   plot(p2)
   
   filename <- paste(columns[i], "_", strategies[i] , "_", cur_scale, "_", cur_init_items, ".pdf", sep="")
   filename <- gsub("/", "," , filename)
   filename <- gsub("%", "p", filename)
   filename <- gsub(" ", "_", filename)
   
   pdf(paste("plots/", filename, sep=""), width=6, height=4)
   plot(p2)
   dev.off()
 }