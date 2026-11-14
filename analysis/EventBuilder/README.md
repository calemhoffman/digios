# Intoduction

This **EventBuilder** use the **BinaryReader** class that read raw binary file. the data can be saved to **Hit** class. 

The **Hit** class only store the GEBHeader and payload. It can decode the payload into **Event** class. 

There are many information in the payload, but only the **id** (board_id * 10 + channel_id), **pre_rise_energy**, **post_rise_energy**, **timestamp** are important.

The **EventBuilder** can also save the trace and it outputs CERN ROOT TTree. 


## EventBuilder

The only event builder (formerly EventBuilder_S; the original EventBuilder, EventBuilder_Q and EventBuilder_A were removed, see git history). It is a derivative of the old EventBuilder_Q (std::priority_queue merge) by applying GeneralSortMapping.h to give gen_runXXX.root. It also added trace analysis. When doing trace analysis, it allows multi-thread processing that using producer-consumer algorithm. The main() thread is the producer that produce event, this event will to translated to Data structure, and the data will be stored in dataQueue. The dataQueue size is limited by MAX_QUEUE_SIZE to limit the memeory usage. When the dataQueue is full, it will wait 1 milisec and check is there any space to put the data in dataQueue unit the data can be put the dataQueue.  When the dataQueue has data, notify one of the thread to do trace analysis and save the result into outputQueue. When outputQueue has data, a outTreeThread will save the data into TTree. 

event-->data --> dataQueue --> trace analysis(nWorkers) --> outputQueue --> save to TTREE.

### mapping

The mapping header is compiled in. `make EventBuilder MAPPING=../working/GeneralSortMapping_xxx.h` selects it
(default `../working/GeneralSortMapping.h`); the binary is rebuilt only when the mapping or a source changes.
`Armory/process_Sort` picks the mapping per run from `working/mappingByRun.sh` if that file exists.
The mapping used is printed at start and saved in the output root file as TMacro `mapping`.


# Experimental Parquet format (not implemented)

parquet format is also coulomb wise 

## install the parquest package

https://arrow.apache.org/install/

```sh
sudo apt install -y -V libarrow-dev libparquet-dev
```