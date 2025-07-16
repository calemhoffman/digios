# Intoduction

This **EventBuilder** use the **BinaryReader** class that read raw binary file. the data can be saved to **Hit** class. 

The **Hit** class only store the GEBHeader and payload. It can decode the payload into **Event** class. 

There are many information in the payload, but only the **id** (board_id * 10 + channel_id), **pre_rise_energy**, **post_rise_energy**, **timestamp** are important.

The **EventBuilder** can also save the trace and it outputs CERN ROOT TTree. 


## EventBuilder


## EventBuilder_Q


## EventBuilder_S



# Experimental Parquet format (not implemented)

parquet format is also coulomb wise 

## install the parquest package

https://arrow.apache.org/install/

```sh
sudo apt install -y -V libarrow-dev libparquet-dev
```