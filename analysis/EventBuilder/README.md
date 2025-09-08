# Intoduction

This **EventBuilder** use the **BinaryReader** class that read raw binary file. the data can be saved to **Hit** class. 

The **Hit** class only store the GEBHeader and payload. It can decode the payload into **Event** class. 

There are many information in the payload, but only the **id** (board_id * 10 + channel_id), **pre_rise_energy**, **post_rise_energy**, **timestamp** are important.

The **EventBuilder** can also save the trace and it outputs CERN ROOT TTree. 


## EventBuilder

This is the original EventBuilder. The files are grouped by the DigiID and scanned. it grapes a batch of data in the frist file of each digiID. Use the FindEarliestTime() method and looping all files to build event. It has no intermediate file output but directly save the runXXX.root. Compatable with existing analysis code. The processing speed is ~ 10 times faster due to reducing I/O.

## EventBuilder_Q

This is a derivative of the EventBuilder by using std::piority_queue. The scanning of files is running in parallel to speed thing up. By using piority_queue, each file push data to the queue and the queue always give the smallest timesatmp data. That reduced the looping of all file to find the earleist time. The output is also compatable with existing code.

## EventBuilder_S

This is a derivative of the EventBuilder_Q by applying GeneralSortMapping.h to give gen_runXXX.root. It also added trace analysis. When doing trace analysis, it allows multi-thread processing that using producer-consumer algorithm. The main() thread is the producer that produce event, this event will to translated to Data structure, and the data will be stored in dataQueue. The dataQueue size is limited by MAX_QUEUE_SIZE to limit the memeory usage. When the dataQueue is full, it will wait 1 milisec and check is there any space to put the data in dataQueue unit the data can be put the dataQueue.  When the dataQueue has data, notify one of the thread to do trace analysis and save the result into outputQueue. When outputQueue has data, a outTreeThread will save the data into TTree. 

event-->data --> dataQueue --> trace analysis(nWorkers) --> outputQueue --> save to TTREE.


# Experimental Parquet format (not implemented)

parquet format is also coulomb wise 

## install the parquest package

https://arrow.apache.org/install/

```sh
sudo apt install -y -V libarrow-dev libparquet-dev
```