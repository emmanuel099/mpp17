library(ggplot2)
library(dplyr)

read_file_data <- function(fname, algo) {
  df <- read.table(list.files(pattern = glob2rx(fname)), header=FALSE, sep=";")
  names(df) <- c("strategy", "height", "threads", "items", "init_items", "scale", "rep", "total_time", "total_throughput", "inserts", 
                 "failed_inserts", "insert_retries", "insert_throughput", "removals", "failed_removals", "remove_retries", "remove_throughput",
                 "finds", "find_retries", "find_throughput")
  df$algorithm <- algo
  df
}

df1 <- read_file_data("LazySkipList*.csv", "LazySkipList")
df2 <- read_file_data("LockFreeSkipList*.csv", "LockFreeSkipList")
df3 <- read_file_data("ConcurrentSkipList*.csv", "ConcurrentSkipList")
df4 <- read_file_data("MMLazySkipList*.csv", "MMLazySkipList")
df5 <- read_file_data("SequentialSkipList*.csv", "SequentialSkipList")

df1 <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df1, median)
df2 <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df2, median)
df3 <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df3, median)
df4 <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df4, median)
df5 <- aggregate(cbind(total_throughput)~strategy+threads+scale+init_items+algorithm, data=df5, median)


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
        
        p1 <- ggplot(df, aes(x=threads, y=total_throughput, group=algorithm, color=algorithm)) + 
          geom_point() +
          geom_line() +
          theme_bw() +
          theme(axis.text.x = element_text(angle = 60, hjust=1),
                legend.position="right") +
          ylab("throughput [Ops/s]") +
          xlab("threads")
        plot(p1)
        
        filename <- paste(cur_strat , "_", cur_scale, "_", cur_init_items, ".pdf", sep="")
        filename <- gsub("/", "," , filename)
        filename <- gsub("%", "p", filename)
        
        pdf(paste("plots/", filename, sep=""), width=6, height=4)
        plot(p1)
        dev.off()
      }
  }
}
